#include <Windows.h>
#include <stdio.h>



HANDLE hpipe = INVALID_HANDLE_VALUE;
DWORD processId = 0;
char processName[MAX_PATH];




void GetProcessInfo() {
	processId = GetCurrentProcessId();
	char processPath[MAX_PATH];
	if (GetModuleFileNameA(NULL, processPath, MAX_PATH)) {
		char* baseName = strrchr(processPath, '\\');
		baseName = (baseName + 1);
		if (baseName)
		{
			strcpy_s(processName, MAX_PATH, baseName);

		}
	}
}



BOOL OpenPipeConnection() {
	if (hpipe == INVALID_HANDLE_VALUE) {
		hpipe = CreateFile(L"\\\\.\\pipe\\hookPipe", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hpipe == INVALID_HANDLE_VALUE) {
			return FALSE;
		}
	}
	return TRUE;
}


void LogMessage(char* message) {
	
	if (processId == 0)
	{
		GetProcessInfo();
	}

	if (!OpenPipeConnection()) {
		
		return;
	}

	char str[256];
	sprintf_s(str, sizeof(str), "%s | PROCESS ID | %u | PROCESS NAME | %s", message, processId, processName);
	DWORD byteWritten = 0;
	if (!WriteFile(hpipe, str, 256, &byteWritten, NULL))
	{
		CloseHandle(hpipe);
		hpipe = INVALID_HANDLE_VALUE;
	}
	
	return; 
}




void CleanUp() {
	if (hpipe != INVALID_HANDLE_VALUE) {
		CloseHandle(hpipe);
	}
	return;
}
