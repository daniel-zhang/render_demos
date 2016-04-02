#ifndef RT_DLL_LOADER_H
#define RT_DLL_LOADER_H

#include <string>
#include <iostream>

class RTDllLoader
{
public:
    static void go()
    {
        std::string dllName = "";
        std::cout << ">>> Loading dll: " << dllName << std::endl;
        std::cout << ">>> Unloading dll: " << dllName << std::endl;
    }
};
#endif