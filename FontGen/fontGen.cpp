#include "FontFactory.h"
#include "DumbDll.h"
#include "LoadTimeDll.h"

#include <iostream>


int main(int argc, char** argv)
{
    std::cout << "Font Number: " << FontFactory::FontGenerator::getFontNum() << std::endl;

    FontFactory::FontGenerator fontGen;

    FontFactory::FontGenerator::NameArray names;
    fontGen.getFontNames(names);

    const FontFactory::FontGenerator::NameArray& namesRef = fontGen.getFontNames();

    for (unsigned int i = 0; i < names.size(); ++i)
    {
        std::cout << "Font name: " << names[i] << std::endl;
    }

    std::cout << std::endl;

    for (unsigned int i = 0; i < namesRef.size(); ++i)
    {
        std::cout << "Ref font name: " << namesRef[i] << std::endl;
    }

    dumb::LonelyDLL dll;
    std::cout << std::endl << "The lonely dll says: I am " << dll.whoAmI() << std::endl;

    std::cout << "Load time dll: " << dumb2::LoadTimeDll::aLoadTimeDll() << std::endl;

    system("pause");
    return 0;
}