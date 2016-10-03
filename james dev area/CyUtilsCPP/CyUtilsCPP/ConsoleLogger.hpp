#pragma once
#include "Logger.hpp"
#include <mutex>
#include <iostream>

namespace logging {
	class ConsoleLogHandler : public LogHandler {
	private:
		std::mutex lock;
		LogLevel level;
#ifdef _WIN32
		void* consoleHandle;
#endif
		void _setBlue();
		void _setYellow();
		void _setRed();
		void _setWhite();
		void _text(String str);
		void _newLine();
	public:
		ConsoleLogHandler();
		void logDebug(String str) {
			std::lock_guard<std::mutex> guard(lock);
			_setBlue();
			_text("[DEBUG] ");
			_setWhite();
			_text(str);
			_newLine();
		}
		void logInfo(String str) {
			std::lock_guard<std::mutex> guard(lock);
			_setBlue();
			_text("[INFO] ");
			_setWhite();
			_text(str);
			_newLine();
		}
		void logWarning(String str) {
			std::lock_guard<std::mutex> guard(lock);
			_setYellow();
			_text("[WARNING] ");
			_setWhite();
			_text(str);
			_newLine();
		}
		void logSevere(String str) {
			std::lock_guard<std::mutex> guard(lock);
			_setRed();
			_text("[SEVERE] ");
			_setWhite();
			_text(str);
			_newLine();
		}
		void logStartStackTrace() {
			lock.lock();
			_setRed();
			_text("===Stack Trace===");
			_newLine();
		}
		void logStopStackTrace() {
			_setRed();
			_text("=== End Trace ===");
			_newLine();
			_setWhite();
			lock.unlock();
		}
		void logStackTrace(String stack) {
			_setRed();
			_text("  > ");
			_setWhite();
			_text(stack);
			_newLine();
		}
		void setLogLevel(LogLevel level) {
			this->level = level;
		}
		LogLevel getLogLevel() const {
			return level;
		}
	};
}