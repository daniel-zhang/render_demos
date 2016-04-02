#ifndef RT_DLL_H
#define RT_DLL_H

#include <windows.h>

#ifdef RTDLL_EXPORTS
#define RTDLL_API __declspec(dllexport)
#else
#define RTDLL_API __declspec(dllimport)
#endif

/**
@param handle    
    - handle to this dll module
@param reason     
    - process attach
        load time(proc starting up)
        run time(calling LoadLibrary)
        setup TLS
    - process detach
        process exiting
        DLL failed to load
        last call of FreeLibrary
        tear down TLS
    - thread attach 
        only when creating a new (child) thread, the process/system will call all existing/loaded DLL's DllMain with this reason
        setup TLS
    - thread detach 
        thread exiting
        tear down TLS
@param reserved  
    reason == proc attach && reserver == NULL : dll is loaded at runtime
    reason == proc attach && reserver != NULL : dll is loaded at loadtime 
    reason == proc detach && reserver == NULL : triggered by FreeLibrary
    reason == proc detach && reserver != NULL : triggered by proc terminating
*/   
BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved)
{

}

class RTDLL
{
public:


};
#endif