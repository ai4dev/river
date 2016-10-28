#include "RevtracerWrapper.h"
#include "../CommonCrossPlatform/Common.h"

#include "Wrapper.Global.h"

AllocateVirtualFunc allocateVirtual;
FreeVirtualFunc freeVirtual;

TerminateProcessFunc terminateProcess;
GetTerminationCodeFunc getTerminationCode;

FormatPrintFunc formatPrint;

WriteFileFunc writeFile;
ToErrnoFunc toErrno;

YieldExecutionFunc yieldExecution;

namespace revwrapper {
	DLL_PUBLIC extern void *CallAllocateMemoryHandler(unsigned long dwSize)
	{
		return allocateVirtual(dwSize);
	}

	DLL_PUBLIC extern void CallFreeMemoryHandler(void *address) {
		return freeVirtual(address);
	}

	DLL_PUBLIC extern void CallTerminateProcessHandler(void) {
		terminateProcess(0);
	}

	DLL_PUBLIC extern void *CallGetTerminationCodeHandler(void) {
		return getTerminationCode();
	}

	DLL_PUBLIC extern int CallFormattedPrintHandler(char * buffer, unsigned int sizeOfBuffer, const char * format, char *argptr) {
		return formatPrint(buffer, sizeOfBuffer, format, (char *)argptr);
	}

	DLL_PUBLIC extern bool CallWriteFile(void *handle, void *buffer, unsigned int size, unsigned long *written) {
		return writeFile(handle, buffer, size, written);
	}

	DLL_PUBLIC extern long CallConvertToSystemErrorHandler(long status) {
		return toErrno(status);
	}

	// Used until we replace the ipclib spinlock with signals / events
	DLL_PUBLIC extern long CallYieldExecution(void) {
		return yieldExecution();
	}

}; //namespace revwrapper
