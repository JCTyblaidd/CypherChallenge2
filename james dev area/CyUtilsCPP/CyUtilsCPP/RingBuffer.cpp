#include "RingBuffer.hpp"
namespace concurrency {
	template<>
	void _callAwaitRingbuffer<BUSY_SPIN>(std::mutex& lock, std::condition_variable& var) {
		//DO ABSOLUTELY NOTHING
	}

	template<>
	void _callAwaitRingbuffer<YEILD>(std::mutex& lock, std::condition_variable& var) {
		std::this_thread::yield();
	}

	template<>
	void _callAwaitRingbuffer<SLEEP>(std::mutex& lock, std::condition_variable& var) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(500));
	}

	template<>
	void _callAwaitRingbuffer<BLOCK>(std::mutex& lock, std::condition_variable& var) {
		std::unique_lock<std::mutex> UniqueLock(lock);
		var.wait_for(UniqueLock, std::chrono::milliseconds(100));
	}
}