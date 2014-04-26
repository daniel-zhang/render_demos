#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H
#include "Util.h"
#include <map>

namespace Gdiplus
{
    class Bitmap;
}

class FontEngine
{
public:
    FontEngine();
    ~FontEngine();
    void init();

protected:
    int getMinX(Gdiplus::Bitmap& bitmap);
    int getMaxX(Gdiplus::Bitmap& bitmap);

private:
    std::vector<CD3D11_RECT> mCharRects;

    int mSpaceWidth;
    int mCharHeight;

    int mTextureHeight;
    int mTextureWidth;
    std::wstring mFontName;
};

class FontSheet
{
public:
    FontSheet();
    ~FontSheet();
    void save2Bmp(std::wstring filename);

    WCHAR mStartChar, mEndChar;
    Gdiplus::Bitmap& mFontBmp;
    std::vector<CD3D11_RECT> mCharRects;
    ID3D11ShaderResourceView* mFontSRV;
};
#endif