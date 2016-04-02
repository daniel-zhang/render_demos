#ifndef FONT_FACTORY_H
#define FONT_FACTORY_H

#include <string>
#include <vector>

namespace FontFactory 
{
    class FontGenerator
    {
    public:
        typedef std::vector<std::string> NameArray;
        FontGenerator();
        ~FontGenerator();

        void getFontNames(NameArray& nameArray);
        const NameArray& getFontNames();

        static int getFontNum();
    private:
        int mFontNum;
        NameArray mFontNames;
    };

}
#endif
