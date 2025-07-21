#pragma once
#include <Windows.h>

typedef NTSTATUS(NTAPI* fnNtAllocateVirtualMemory)(
    HANDLE    ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T   RegionSize,
    ULONG     AllocationType,
    ULONG     Protect
    );


typedef NTSTATUS(NTAPI* fnNtProtectVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PSIZE_T RegionSize,
    ULONG NewProtection,
    PULONG OldProtection

    );






BOOL install_hooks();
VOID ProcessDetachRoutine();
VOID BlockExecution();


// logger 
void LogMessage(char* message);
void CleanUp();



