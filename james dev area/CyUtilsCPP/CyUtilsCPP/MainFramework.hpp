#include "ConsoleLogger.hpp"
#include "LaunchParams.hpp"
#include "ExecutorPool.hpp"


inline std::string getFileLoc(std::string loc) {
#if USE_WORKSPACE_LAYOUT
	return "../../../main workspace/" + loc;
#else
	return loc;
#endif
}


extern WorkPoolType* WorkPool;
extern params::LaunchParams LaunchParameters;
extern logging::ConsoleLogHandler* LogHandle;

#define INFO(X) LogHandle->logInfo(X)
#define DEBUG(X) LogHandle->logDebug(X)
#define WARN(X) LogHandle->logWarning(X)
#define SEVERE(X) LogHandle->logSevere(X)