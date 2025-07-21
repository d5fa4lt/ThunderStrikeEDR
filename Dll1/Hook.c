#include <Windows.h>
#include <stdio.h>
#include "MinHook.h"
#include "Common.h"
#pragma comment (lib, "libMinHook.x64.lib")


fnNtProtectVirtualMemory g_NtProtectVirtualMemory = NULL;
fnNtAllocateVirtualMemory g_NtAllocateVirtualMemory = NULL;
PVOID pNtProtectVirtualMemory = NULL;
PVOID pNtAllocateVirtualMemory = NULL;

g_disableDetection;

NTSTATUS NTAPI hooked_NtProtectVirtualMemory(
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	PSIZE_T RegionSize,
	ULONG NewProtection,
	PULONG OldProtection){
	
	if(((NewProtection & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE) && (!g_disableDetection)) {
		LogMessage("[+] MyEdr: PAGE_EXECUTE_READWRITE detected using ProtectVirtualMemory");
		
	}
	
	return g_NtProtectVirtualMemory(ProcessHandle, BaseAddress, RegionSize, NewProtection, OldProtection);
}


NTSTATUS NTAPI hooked_NtAllocateVirtualMemory(
	HANDLE    ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR ZeroBits,
	PSIZE_T   RegionSize,
	ULONG     AllocationType,
	ULONG     Protect) {

	if ((Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE) {
		LogMessage("[+] MyEdr: PAGE_EXECUTE_READWRITE detected using NtAllocateVirtualMemory");
		
		
	}
	
	
	return g_NtAllocateVirtualMemory(ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);

}



BOOL install_hooks() {

	g_disableDetection = TRUE;
	pNtProtectVirtualMemory = GetProcAddress(GetModuleHandleW(TEXT("ntdll")), "NtProtectVirtualMemory");
	pNtAllocateVirtualMemory = GetProcAddress(GetModuleHandleW(TEXT("ntdll")), "NtAllocateVirtualMemory");
	DWORD 	dwMinHookErr = NULL;


	if ((dwMinHookErr = MH_Initialize()) != MH_OK) {
		LogMessage("[!] MH_Initialize Failed");
		return FALSE;
	}

	if ((dwMinHookErr = MH_CreateHookApi(L"ntdll", "NtProtectVirtualMemory", &hooked_NtProtectVirtualMemory, (LPVOID*)&g_NtProtectVirtualMemory)) != MH_OK) {
		LogMessage("[!] MH_CreateHook Failed ");
		return FALSE;
	}

	if ((dwMinHookErr = MH_CreateHookApi(L"ntdll", "NtAllocateVirtualMemory", &hooked_NtAllocateVirtualMemory, (LPVOID*)&g_NtAllocateVirtualMemory)) != MH_OK) {
		LogMessage("[!] MH_CreateHook Failed");
		return FALSE;
	}

	if ((dwMinHookErr = MH_EnableHook(MH_ALL_HOOKS)) != MH_OK) {
		LogMessage("[!] MH_EnableHook Failed");
		return -1;
	}
	g_disableDetection = FALSE;
	
	return TRUE;
}






VOID ProcessDetachRoutine() {
	DWORD 	dwMinHookErr = NULL;

	if ((dwMinHookErr = MH_DisableHook(MH_ALL_HOOKS)) != MH_OK) {
		LogMessage("[!] MH_DisableHook Failed ");
		return FALSE;
	}

	if ((dwMinHookErr = MH_Uninitialize()) != MH_OK) {
		LogMessage("[!] MH_Uninitialize Failed");
		return FALSE;
	}

	return TRUE;

}



