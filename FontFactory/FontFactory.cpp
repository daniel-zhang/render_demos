#include "FontFactory.h"

namespace FontFactory
{
    FontGenerator::FontGenerator()
    {
        mFontNum = 100;
        mFontNames.push_back("arial");
        mFontNames.push_back("consolas");
        mFontNames.push_back("latin");
        mFontNames.push_back("CJK");
    }

    FontGenerator::~FontGenerator() { }

    void FontGenerator::getFontNames( NameArray& nameArray )
    {
        for (unsigned int i = 0; i < mFontNames.size(); ++i)
        {
            nameArray.push_back(mFontNames[i]);
        }
    }

    const FontGenerator::NameArray& FontGenerator::getFontNames()
    {
        return mFontNames;
    }

    int FontGenerator::getFontNum()
    {
        return 100;
    }
}
