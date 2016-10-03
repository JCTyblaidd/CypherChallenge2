#pragma once

#include <cinttypes>
typedef std::uint32_t uint;
#include <atomic>
#include <condition_variable>
#include <thread>

/**

Asynchronous -> Worker Job Delegator

Based of disruptor (java library)
**/
namespace concurrency {

	enum PublisherType {
		SINGLE_PUBLISHER,
		MULTI_PUBLISHER,
	};

	enum WaitType {
		BUSY_SPIN,
		YEILD,
		SLEEP,
		BLOCK,
	};

	template<typename T, uint N, PublisherType PUBLISH, WaitType READ_WAIT, WaitType WRITE_WAIT>
	class RingBuffer;

	//template<typename T, uint N, WaitType WAIT_READ, WaitType WAIT_WRITE, PublisherType PUBLISH>
	//void _publishRingbuffer(RingBuffer<T, N, PUBLISH,WAIT_READ,WAIT_WRITE>& buf,T t);
	template<PublisherType PUBLISH>
	struct _publisher {
		template<typename T, uint N, WaitType WAIT_READ, WaitType WAIT_WRITE>
		void _publishRingbuffer(RingBuffer<T, N, PUBLISH, WAIT_READ, WAIT_WRITE>& buf, T t);
	};

	template<WaitType WAIT>
	void _callAwaitRingbuffer(std::mutex& lock, std::condition_variable& cond_var);

	template<typename T, uint N, PublisherType PUBLISH = SINGLE_PUBLISHER, WaitType READ_WAIT = BLOCK, WaitType WRITE_WAIT = WaitType::YEILD>
	class RingBuffer {
	private:
		static_assert(((N & (N - 1)) == 0) && N != 0, "RingBuffer Must be a Power of 2");
		T* ALLOC;
		static constexpr uint MASK = N - 1;
		_publisher<PUBLISH> publishFunc;
	public:
		std::atomic<uint> writerLoc, readerLoc;
		std::condition_variable writeSignal, readSignal;
		std::mutex writeMutex, readMutex, multiPublishMutex;
		RingBuffer()
			: ALLOC(new T[N]), writerLoc(1), readerLoc(0) {
		}

		uint getSnapshotWaiting() {
			uint W, R;
			W = writerLoc.load(std::memory_order_relaxed);
			R = readerLoc.load(std::memory_order_relaxed);
			if (W > R) {
				return W - R - 1;
			}
			else {
				return N - 1 + W - R;
			}
		}

		T& operator[] (uint index) {
			return ALLOC[index & MASK];
		}
		const T& operator[] (uint index) const {
			return ALLOC[index & MASK];
		}

		void publish(T t) {
			publishFunc._publishRingbuffer<T, N, READ_WAIT, WRITE_WAIT>(*this, t);
		}

		T acquire(T setAfter) {
			uint currentLoc, nextLoc, prevLoc, writeHead;
			T result;
			while (true) {
				currentLoc = readerLoc.load();
				nextLoc = (currentLoc + 1) & MASK;
				while (nextLoc == (writeHead = writerLoc.load())) {
					_callAwaitRingbuffer<READ_WAIT>(readMutex, readSignal);
				}
				//OK: Write Head is Out Of The Way
				result = (*this)[nextLoc];
				//If this caller moves -> Handle It
				bool flag = readerLoc.compare_exchange_weak(currentLoc, nextLoc);
				if (flag) {
					break;
				}
			}
			(*this)[nextLoc] = setAfter;
			//Signal and cleanup//
			prevLoc = (currentLoc - 1) & MASK;
			if (prevLoc == writeHead) {
				writeSignal.notify_all();
			}
			return result;
		}
	};

	template<>
	struct _publisher<SINGLE_PUBLISHER> {
		template<typename T, uint N, WaitType WAIT_READ, WaitType WAIT_WRITE>
		void _publishRingbuffer(RingBuffer<T, N, SINGLE_PUBLISHER, WAIT_READ, WAIT_WRITE>& buf, T t) {
			constexpr uint MASK = N - 1;
			uint currentLoc = buf.writerLoc.load();
			uint nextLoc = (currentLoc + 1) & MASK;
			uint readerLoc;
			while (nextLoc == (readerLoc = buf.readerLoc.load())) {
				_callAwaitRingbuffer<WAIT_WRITE>(buf.writeMutex, buf.writeSignal);
			}
			//We Are OK//
			buf[currentLoc] = t;
			buf.writerLoc.store(nextLoc);
			//Now Signal//
			uint prevLoc = (currentLoc - 1) & MASK;
			if (readerLoc == prevLoc) {
				buf.readSignal.notify_all();
			}
		}
	};

	template<>//TODO: Convert to lockless
	struct _publisher<MULTI_PUBLISHER> {
		template<typename T, uint N, WaitType WAIT_READ, WaitType WAIT_WRITE>
		void _publishRingbuffer(RingBuffer<T, N, MULTI_PUBLISHER, WAIT_READ, WAIT_WRITE>& buf, T t) {
			constexpr uint MASK = N - 1;
			uint currentLoc, nextLoc, readerLoc, prevLoc;
			//while (true) {
			buf.multiPublishMutex.lock();

			currentLoc = buf.writerLoc.load();
			nextLoc = (currentLoc + 1) & MASK;
			while (nextLoc == (readerLoc = buf.readerLoc.load())) {
				_callAwaitRingbuffer<WAIT_WRITE>(buf.writeMutex, buf.writeSignal);
			}
			//OK: Reader Location is OK -> Attempt Write And Advance//
			buf[currentLoc] = t;
			buf.writerLoc.store(nextLoc);

			buf.multiPublishMutex.unlock();
			//}
			prevLoc = (currentLoc - 1) & MASK;
			if (readerLoc == prevLoc) {
				buf.readSignal.notify_all();
			}
		}
	};


	template<>
	extern void _callAwaitRingbuffer<BUSY_SPIN>(std::mutex& lock, std::condition_variable& var);

	template<>
	extern void _callAwaitRingbuffer<YEILD>(std::mutex& lock, std::condition_variable& var);

	template<>
	extern void _callAwaitRingbuffer<SLEEP>(std::mutex& lock, std::condition_variable& var);

	template<>
	extern void _callAwaitRingbuffer<BLOCK>(std::mutex& lock, std::condition_variable& var);


}