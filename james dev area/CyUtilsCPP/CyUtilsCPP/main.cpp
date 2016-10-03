#include "ConsoleLogger.hpp"
#include "WorkerPool.hpp"
#include "LaunchParams.hpp"

typedef void* RunnableData;
typedef void(*RunnableFunc)(RunnableData DATA);

struct RunnableCache {
	RunnableFunc Func;
	RunnableData Data;
};

void runHandle(RunnableCache Cache) {

}

typedef concurrency::WorkerPool<RunnableCache, 1024, concurrency::SINGLE_PUBLISHER, concurrency::BLOCK, concurrency::YEILD> WorkPoolType;

WorkPoolType* WorkPool;
params::LaunchParams LaunchParameters;

int main(int argCount, char *args[]) {
	//Load Parameters//
	uint threadCount = std::thread::hardware_concurrency();
	WorkPool = new WorkPoolType(threadCount, runHandle);
	LaunchParameters.Load(argCount, args);
	//OK Now Find Work

	//Await Shutdown//
	system("pause");
	delete WorkPool;
}