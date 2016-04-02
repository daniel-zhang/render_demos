#ifndef DUMB_DLL_H
#define DUMB_DLL_H

#ifdef DUMBDLL_EXPORTS
#define DUMBDLL_API __declspec(dllexport)
#else
#define DUMBDLL_API __declspec(dllimport)
#endif

#include <string>

namespace dumb
{
    class LonelyDLL
    {
    public:
        DUMBDLL_API LonelyDLL();
        DUMBDLL_API ~LonelyDLL();

        DUMBDLL_API std::string whoAmI();
    private:
        bool mInnerPeaceful;
    };
}
#endif