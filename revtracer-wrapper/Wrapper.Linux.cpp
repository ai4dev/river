#ifdef __linux__

#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "Wrapper.Global.h"

#include "RevtracerWrapper.h"
#include "../CommonCrossPlatform/Common.h"


typedef void* lib_t;

DLL_LOCAL lib_t lcHandler;
DLL_LOCAL lib_t lpthreadHandler;

// ------------------- Memory allocation ----------------------
typedef void* (*AllocateMemoryHandler)(
	void *addr,
	size_t length, 
	int prot, 
	int flags, 
	int fd, 
	off_t offset
);

AllocateMemoryHandler _virtualAlloc;

void *LinAllocateVirtual(unsigned long size) {
	return _virtualAlloc(NULL, size, 0x7, 0x21, 0, 0); // RWX shared anonymous
}

// ------------------- Memory deallocation --------------------
typedef int (*FreeMemoryHandler)(
	void *addr, 
	size_t length
);

FreeMemoryHandler _virtualFree;

void LinFreeVirtual(void *address) {
	// TODO: implement some size retrieval mechanism
}

// ------------------- Process termination --------------------
typedef void(*TerminateProcessHandler)(
	int status
);

TerminateProcessHandler _terminateProcess;

void LinTerminateProcess(int retCode) {
	_terminateProcess(retCode);
}

void *LinGetTerminationCodeFunc() {
	return (void *)_terminateProcess;
}

// ------------------- Write file -----------------------------
typedef ssize_t(*WriteFileHandler)(
	int fd, 
	const void *buf, 
	size_t count
);

WriteFileHandler _writeFile;

bool WinWriteFile(void *handle, void *buffer, size_t size, unsigned long *written) {
	*written = _writeFile((int)handle, buffer, size);
	return (0 <= written);
}

// ------------------- Error codes ----------------------------

long LinToErrno(long ntStatus) {
	return ntStatus;
}

// ------------------- Formatted print ------------------------
typedef int(*FormatPrintHandler)(
	char *buffer,
	size_t count,
	const char *format,
	va_list argptr
	);

FormatPrintHandler _formatPrint;

int LinFormatPrint(char *buffer, size_t sizeOfBuffer, const char *format, char *argptr) {
	return _formatPrint(buffer, sizeOfBuffer - 1, format, (va_list)argptr);
}

// ------------------- Yield execution ------------------------
typedef int(*YieldExecutionHandler) (void);

YieldExecutionHandler _yieldExecution;

long LinYieldExecution(void) {
	return (long) _yieldExecution();
}


namespace revwrapper {
	extern "C" void InitRevtracerWrapper() {
		lcHandler = GET_LIB_HANDLER("libc.so");
		lpthreadHandler = GET_LIB_HANDLER("libpthread.so");
		// get functionality from ntdll
		_virtualAlloc = (AllocateMemoryHandler)LOAD_PROC(lcHandler, "mmap");
		_virtualFree = (FreeMemoryHandler)LOAD_PROC(lcHandler, "munmap");

		_terminateProcess = (TerminateProcessHandler)LOAD_PROC(lcHandler, "exit");

		_writeFile = (WriteFileHandler)LOAD_PROC(lcHandler, "write");

		_formatPrint = (FormatPrintHandler)LOAD_PROC(lcHandler, "vsnprintf");

		_yieldExecution = (YieldExecutionHandler)LOAD_PROC(lpthreadHandler, "pthread_yield");

		// set global functionality
		allocateVirtual = LinAllocateVirtual;
		freeVirtual = LinFreeVirtual;

		terminateProcess = LinTerminateProcess;
		getTerminationCode = LinGetTerminationCodeFunc;

		toErrno = LinToErrno;
		formatPrint = LinFormatPrint;

		yieldExecution = LinYieldExecution;
	}
};

#endif
