#pragma once
#include <cinttypes>
#include <string>
typedef std::string String;

namespace logging {
	enum LogLevel : uint8_t {
		DEBUG,
		INFO,
		WARNING,
		SEVERE
	};
	class LogHandler {
	public:
		virtual void logDebug(String str) = NULL;
		virtual void logInfo(String str) = NULL;
		virtual void logWarning(String str) = NULL;
		virtual void logSevere(String str) = NULL;
		virtual void logStartStackTrace() = NULL;
		virtual void logStopStackTrace() = NULL;
		virtual void logStackTrace(String stack) = NULL;
		virtual void setLogLevel(LogLevel level) = NULL;
		virtual LogLevel getLogLevel() const = NULL;
	};
}