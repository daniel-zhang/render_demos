#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H
#include "Util.h"
#include <map>
#include <Gdiplus.h>

class FontSheet
{
public:
    FontSheet(
        std::wstring fontName = L"Consolas", 
        float fontSizeInPixel = 30.f, 
        int textureWidth = 1024,

        Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular, 
        Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias,
        Gdiplus::CompositingMode compositeMode = Gdiplus::CompositingModeSourceCopy,
        Gdiplus::Color bgc = Gdiplus::Color(0, 0, 0, 0),
        Gdiplus::Color fgc = Gdiplus::Color(255, 255, 255, 255)
        );
    ~FontSheet();

    CD3D11_RECT* getSrcRect(WCHAR c);

    //
    // Input properties
    //
    std::wstring                mFontName;
    Gdiplus::FontStyle          mFontStyle; 
    float                       mFontSizeInPixel;
    Gdiplus::TextRenderingHint  mHint; 

    Gdiplus::Color              mBackgroundColor;
    Gdiplus::Color              mForegroundColor;
    int                         mTextureWidth;
    Gdiplus::CompositingMode    mCompositeMode;

    WCHAR                       mStartChar, mEndChar;
    WCHAR*                      mCharSet;
    UINT                        mCharSetNum; // Without the trailing zero

    //
    // Output properties
    //
    std::wstring mFontSheetBmpFileName;
    std::wstring mFontSheetBmpFilePath;
    ID3D11Texture2D* mFontSheetTex;
    ID3D11ShaderResourceView* mFontSRV;
    std::vector<CD3D11_RECT> mCharRects;

    int mCharHeight;
    int mSpaceWidth;
    int mTextureHeight;
};

class FontEngine
{
public:
    FontEngine();
    ~FontEngine();

    void createFontSheet(ID3D11Device* dvc, FontSheet& fs, std::wstring debugFilename = L"");

protected:
    int getMinX(Gdiplus::Bitmap& bitmap);
    int getMaxX(Gdiplus::Bitmap& bitmap);
};

#endif