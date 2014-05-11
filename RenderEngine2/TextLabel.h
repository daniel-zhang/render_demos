#ifndef TEXT_LABEL_H
#define TEXT_LABEL_H

#include "IRenderable2D.h"


class TextElement: public IRenderable2D
{
public:
    std::wstring mCharacter;
    UINT mFontSize;
    std::string mFontName;
};

class TextBlock
{
public:


};

#endif