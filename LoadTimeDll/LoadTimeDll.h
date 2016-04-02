#ifndef LOAD_TIME_DLL_H
#define LOAD_TIME_DLL_H

#ifdef LOADTIMEDLL_EXPORTS 
#define LTDLL_API __declspec(dllimport)
#else
#define LTDLL_API __declspec(dllimport)
#endif

namespace dumb2
{
    class LoadTimeDll
    {
    public:
        static LTDLL_API int aLoadTimeDll();
    };
}

#endif