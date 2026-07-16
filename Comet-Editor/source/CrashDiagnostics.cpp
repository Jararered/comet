#include "CrashDiagnostics.h"

#include <Windows.h>
#include <DbgHelp.h>

#include <cstdio>

namespace
{
    LONG WINAPI WriteCrashDump(EXCEPTION_POINTERS* exceptionPointers)
    {
        SYSTEMTIME now = {};
        GetLocalTime(&now);

        char dumpName[MAX_PATH] = {};
        std::snprintf(dumpName, sizeof(dumpName), "Comet-Editor-%04u%02u%02u-%02u%02u%02u.dmp", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);

        const HANDLE dumpFile = CreateFileA(dumpName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (dumpFile != INVALID_HANDLE_VALUE)
        {
            MINIDUMP_EXCEPTION_INFORMATION exceptionInfo = {};
            exceptionInfo.ThreadId = GetCurrentThreadId();
            exceptionInfo.ExceptionPointers = exceptionPointers;
            exceptionInfo.ClientPointers = FALSE;

            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile, static_cast<MINIDUMP_TYPE>(MiniDumpWithThreadInfo | MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
                              &exceptionInfo, nullptr, nullptr);
            CloseHandle(dumpFile);
        }

        const unsigned long exceptionCode = exceptionPointers && exceptionPointers->ExceptionRecord ? exceptionPointers->ExceptionRecord->ExceptionCode : 0;
        const void* exceptionAddress = exceptionPointers && exceptionPointers->ExceptionRecord ? exceptionPointers->ExceptionRecord->ExceptionAddress : nullptr;
        std::fprintf(stderr, "FATAL: unhandled exception 0x%08lX at %p; crash dump: %s\n", exceptionCode, exceptionAddress, dumpName);
        std::fflush(stderr);

        return EXCEPTION_EXECUTE_HANDLER;
    }
}

namespace CrashDiagnostics
{
    void Install()
    {
        SetUnhandledExceptionFilter(WriteCrashDump);
    }
}
