#pragma once
#include "WorkerPool.hpp"

typedef void* RunnableData;
typedef void(*RunnableFunc)(RunnableData DATA);

struct RunnableCache {
	RunnableFunc Func;
	RunnableData Data;
};

void runHandle(RunnableCache Cache) {
	Cache.Func(Cache.Data);
}

typedef concurrency::WorkerPool<RunnableCache, 1024, concurrency::SINGLE_PUBLISHER, concurrency::BLOCK, concurrency::YEILD> WorkPoolType;