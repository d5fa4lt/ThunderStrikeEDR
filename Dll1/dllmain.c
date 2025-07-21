#include <Windows.h>
#include "Common.h"
#include <stdio.h>





BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call, LPVOID lpReserved)
{

    HANDLE hThread = NULL;


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        
        install_hooks();
        break;
    }
    

    case DLL_PROCESS_DETACH: {
            ProcessDetachRoutine();
            break;
        }
    }
    return TRUE;
}

