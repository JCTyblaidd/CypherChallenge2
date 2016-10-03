#pragma once
#include "RingBuffer.hpp"
#include <vector>
#include <thread>
namespace concurrency {
	template<typename T, uint CACHE_SIZE, PublisherType PUBLISH, WaitType READ_WAIT, WaitType WRITE_WAIT>
	class WorkerPool {
	private:
		RingBuffer<T, CACHE_SIZE, PUBLISH, READ_WAIT, WRITE_WAIT> Buffer;
		std::vector<std::thread*> threads;
		std::atomic<bool> shutdownFlag;
		void(*Consumer)(T t);
		static void _call(WorkerPool* Pool) {
			T nullType = T();
			while (!Pool->shutdownFlag) {
				T t = Pool->Buffer.acquire(nullType);
				Pool->Consumer(t);
			}
		}
	public:
		WorkerPool(uint Size, void(*jobHandle)(T t)) :Consumer(jobHandle), shutdownFlag(false), Buffer(), threads() {
			threads.resize(Size);
			for (uint i = 0; i < Size; i++) {
				std::thread* newThread = new std::thread(_call, this);
				threads[i] = newThread;
				newThread->detach();
			}
		}
		~WorkerPool() {
			shutdownFlag.store(true);
			//Clear//
			for (std::thread* thread : threads) {
				if (thread->joinable()) {
					thread->join();
				}
			}
			//Delete
			for (std::thread* thread : threads) {
				delete thread;
			}
		}
		void pushJob(T t) {
			Buffer.publish(t);
		}
	};
}
