#include "execenv.h"
#include "mm.h"
#include "cb.h"

//#include "lup.h" //for now
#include "revtracer.h"

void *ExecutionEnvironment::operator new(size_t sz){
	void *storage = revtracerAPI.memoryAllocFunc(sz);
	if (NULL == storage) {
		return NULL;
	}
	return storage;
}

void ExecutionEnvironment::operator delete(void *ptr) {
	revtracerAPI.memoryFreeFunc((BYTE *)ptr);
}

ExecutionEnvironment::ExecutionEnvironment(DWORD flags, unsigned int heapSize, unsigned int historySize, unsigned int executionSize, unsigned int trackSize, unsigned int logHashSize, unsigned int outBufferSize) {
	bValid = false;
	generationFlags = flags;
	exitAddr = 0xFFFFCAFE;
	if (0 == heap.Init(heapSize)) {
		return;
	}

	//if (0 == InitBlock(pEnv, logHashSize, historySize)) {
	if (0 == blockCache.Init(&heap, logHashSize, historySize)) {
		heap.Destroy();
		return;
	}

	if (0 == (executionBuffer = (UINT_PTR *)revtracerAPI.memoryAllocFunc(executionSize + trackSize + 4096))) {
		blockCache.Destroy();
		heap.Destroy();
		return;
	}

	if (!codeGen.Init(&heap, &runtimeContext, outBufferSize, generationFlags)) {
		revtracerAPI.memoryFreeFunc(executionBuffer);
		blockCache.Destroy();
		heap.Destroy();
		return;
	}

	if (NULL == (pStack = (unsigned char *)revtracerAPI.memoryAllocFunc(0x100000))) {
		codeGen.Destroy();
		revtracerAPI.memoryFreeFunc(executionBuffer);
		blockCache.Destroy();
		heap.Destroy();
		return;
	}
	rev_memset(pStack, 0, 0x100000);
	runtimeContext.virtualStack = (DWORD)pStack + 0xFFFF0;


	if (NULL == (eStack = (unsigned char *)revtracerAPI.memoryAllocFunc(0x10000))) {
		revtracerAPI.memoryFreeFunc(pStack);
		codeGen.Destroy();
		revtracerAPI.memoryFreeFunc(executionBuffer);
		blockCache.Destroy();
		heap.Destroy();
		return;
	}
	rev_memset(eStack, 0, 0x10000);
	runtimeContext.exceptionStack = (DWORD)eStack + 0xFFF0;

	runtimeContext.execBuff = (DWORD)executionBuffer + executionSize - 4; //TODO: make independant track buffer 
	executionBase = runtimeContext.execBuff;

	runtimeContext.trackStack = (DWORD)executionBuffer + executionSize + trackSize - 4;
	runtimeContext.trackBuff = runtimeContext.trackBase = (DWORD)executionBuffer + executionSize + trackSize + 4096;
	
	ac.Init();
	//this is a major hack...
	// remove after addres tracking is completely decoupled from the reversible tracking
	runtimeContext.taintedAddresses = (UINT_PTR)this;

	bValid = true;
}

/*struct _exec_env *_exec_env::NewEnv(unsigned int heapSize, unsigned int historySize, unsigned int executionSize, unsigned int logHashSize, unsigned int outBufferSize) {
	struct _exec_env *pEnv;

	if (NULL == (pEnv = (struct _exec_env *)EnvMemoryAlloc(sizeof(*pEnv)))) {
		return NULL;
	}

	struct _exec_env *tEnv = new (pEnv)_exec_env(heapSize, historySize, executionSize, logHashSize, outBufferSize);

	if ((NULL == tEnv) || (!tEnv->bValid)) {
		EnvMemoryFree((BYTE *)pEnv);
		return NULL;
	}

	return pEnv;
}*/

ExecutionEnvironment::~ExecutionEnvironment() {
	blockCache.Destroy(); 
	heap.Destroy();

	revtracerAPI.memoryFreeFunc((BYTE *)executionBuffer);

	revtracerAPI.memoryFreeFunc((BYTE *)pStack);
	pStack = NULL;
}

/*void SetUserContext(struct ExecutionEnvironment *pEnv, void *ptr) {
	pEnv->userContext = ptr;
}*/

/*void *AllocUserContext(struct ExecutionEnvironment *pEnv, unsigned int size) {
	if (NULL != pEnv->userContext) {
		return NULL;
	}

	pEnv->userContext = pEnv->heap.Alloc(size);
	rev_memset(pEnv->userContext, 0, size);
	return pEnv->userContext;
}

void DeleteUserContext(struct ExecutionEnvironment *pEnv) {
	if (NULL == pEnv->userContext) {
		return;
	}

	pEnv->heap.Free(pEnv->userContext);
	pEnv->userContext = NULL;
}*/
