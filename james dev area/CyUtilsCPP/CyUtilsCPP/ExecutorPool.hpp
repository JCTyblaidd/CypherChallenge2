#pragma once
#include "WorkerPool.hpp"

typedef void* RunnableData;
typedef void(*RunnableFunc)(RunnableData DATA);

struct RunnableCache {
	RunnableFunc Func;
	RunnableData Data;
};

void runHandle(RunnableCache Cache);

typedef concurrency::WorkerPool<RunnableCache, 1024, concurrency::SINGLE_PUBLISHER, concurrency::BLOCK, concurrency::YEILD> WorkPoolType;