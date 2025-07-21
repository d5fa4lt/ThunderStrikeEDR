#include <Windows.h>
#include <stdio.h>
#pragma warning(disable : 4996)



BOOL LoadService(LPCSTR    lpServiceName, LPCSTR    lpDisplayName, LPCSTR    lpBinaryPathName) {
	SC_HANDLE hSCManeger;
	SC_HANDLE hService = NULL;

	hSCManeger = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!hSCManeger)
	{
		printf("[-] OpenSCManagerA Failed with Error: %d\n", GetLastError());
		CloseServiceHandle(hSCManeger);
		return FALSE;
	}


	hService = CreateServiceA(hSCManeger, lpServiceName, lpDisplayName, SC_MANAGER_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, lpBinaryPathName, 
		NULL, NULL, NULL, NULL, NULL);
	if (!hService)
	{
		printf("[-] CreateServiceA Failed with error: %d\n", GetLastError());
		CloseServiceHandle(hService); CloseServiceHandle(hSCManeger);
		return FALSE;
	}

	return TRUE;

}

BOOL StartKernelService(LPCSTR  lpServiceName) {
	SC_HANDLE hSCManeger;
	SC_HANDLE hService = NULL;

	hSCManeger = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!hSCManeger)
	{
		printf("[-] OpenSCManagerA Failed with Error: %d\n", GetLastError());
		CloseServiceHandle(hSCManeger);
		return FALSE;
	}


	hService = OpenServiceA(hSCManeger, lpServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
	if (!hService)
	{
		printf("[-] OpenServiceA Failed with Error: %d\n", GetLastError());
		CloseServiceHandle(hSCManeger);
		CloseServiceHandle(hService);
		return FALSE;
	}


	if (StartServiceA(hService, 0 , NULL) == FALSE)
	{
		printf("[-] StartServiceA Failed with Error: %d\n", GetLastError());
		CloseServiceHandle(hSCManeger);
		CloseServiceHandle(hService);
		return FALSE;
	}




	return TRUE;


}



void WINAPI HandleClientConnection(HANDLE hpipe) {
	char buffer[1024];
	DWORD bytesREAD; 

	while (TRUE) {
		BOOL result = ReadFile(hpipe, buffer, sizeof(buffer) -1, &bytesREAD, NULL);
		if (!result || bytesREAD == 0)
		{
			break;
		}
		buffer[bytesREAD] = '\0';  // Null-terminate
		printf("%s\n", buffer);
	}
	
	
	return;
}



void StarteNamedPipeServer() {

	HANDLE hThread;
	DWORD dwThreadId;
	while (TRUE) {
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = (PSECURITY_ATTRIBUTES)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
		sa.bInheritHandle = TRUE;

		if (!InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
		{
			printf("[!] InitializeSecurityDescriptor Failed: %d \n", GetLastError());
			return;

		}

		if (!SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE))
		{
			printf("[!] SetSecurityDescriptorDacl Failed: %d \n", GetLastError());
			return;
		}


		HANDLE hpipe = CreateNamedPipeW(L"\\\\.\\pipe\\hookPipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,1024, 1024, 0, &sa);
		if (hpipe == INVALID_HANDLE_VALUE)
		{
			printf("[!] CreateNamedPipeW Failed: %d \n", GetLastError());
			return; 
		}
		
		
		BOOL isConnect = FALSE;



		if (ConnectNamedPipe(hpipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED)
		{
			isConnect = TRUE;
		}
		

		if (isConnect) {

			
			hThread = CreateThread(NULL, 0, &HandleClientConnection, (LPVOID)hpipe, 0 , &dwThreadId);
			if (!hThread) // hThread == NULL 
			{
				printf("[!] CreateThread Failed \n");
				return;
			}
			else {
				CloseHandle(hThread);
			}
			

		}
		else
		{
			CloseHandle(hpipe);
		}

	}
}


int main() {

	char* ServiceFile = "C:\\vedr\\EDRDriver.sys";
	char* ServiceName = "VERD Kernel";


	FILE* file = fopen("EDRDriver.sys", "r");
	if (!file) {
		printf("File does not exists.\n");
		fclose(file);
		return -1; 
	}

	file = fopen("hook.dll", "r");
	if (!file) {
		printf("File does not exists.\n");
		fclose(file);
		return -1;
	}

	fclose(file);

	printf("[+] Driver : %s\n", ServiceFile);
	printf("[+] Service Name : %s\n", ServiceName);

	if (!LoadService(ServiceName, ServiceName, ServiceFile))
	{
		printf("[!] LoadService Failed with error %d\n", GetLastError());
		return -1;
	}

	if (!StartKernelService(ServiceName))
	{
		printf("[!] StartKernelService Failed with error : %d\n", GetLastError());
		return -1;
	}

	printf("Driver Loaded and running ... \n");
	printf("[#] Starting Named Pipe Server... \n");

	// start named pipe server 
	StarteNamedPipeServer();
	
	return 0;
}