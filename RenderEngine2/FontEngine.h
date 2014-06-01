#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H
#include "Util.h"
#include <map>
#include <Gdiplus.h>
#include "Math2D.h"

namespace boost { namespace serialization {
    class access; 
} }

class FontSheet2
{
public:
    FontSheet2()
        :mFontName(L""), mFontPixelSize(0), mCharHeight(0){}
    FontSheet2(std::wstring fontName, int fontPixelSize)
        :mFontName(fontName), mFontPixelSize(fontPixelSize), mCharHeight(0) {}
    Box2D* getCharRect(wchar_t c);

    std::wstring mFontName;
    int mFontPixelSize;
    int mCharHeight;
    std::vector<Box2D> mCharRects;

    // Serialization
    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & mFontName & mFontPixelSize & mCharHeight & mCharRects;
    }
};

class FontMgr
{
public:
    FontMgr(ID3D11Device* device, ID3D11DeviceContext* context);
    ~FontMgr();

    void init();
    FontSheet2* getFont(std::wstring& fontName, UINT fontSize);
    ID3D11ShaderResourceView* getSRV();

private:
    void createFonts();
    // [in] Gdiplus::Bitmap&            bitmap 
    // [out]ID3D11Texture2D**           texture 
    // [out]ID3D11ShaderResourceView**  srv
    void createTextureFromBitmap(Gdiplus::Bitmap& bmp, Gdiplus::Rect& bmpRect, 
        ID3D11Texture2D** ppTexture, ID3D11ShaderResourceView** ppSRV);
    int calcTextureHeight(int width);

    bool validateCache();
    void createFromScratch();

    // Gdiplus::Graphics always draw character with padding
    // Have to iterate through pixels to find out the exact size of character
    int getCharMinX(Gdiplus::Bitmap& bitmap);
    int getCharMaxX(Gdiplus::Bitmap& bitmap);

    std::vector<FontSheet2> mFonts;
    std::wstring mFilePath;
    std::wstring mCacheFileName;
    std::wstring mFontTextureFileName;

    UINT mTextureWidth, mTextureHeight;
    ID3D11ShaderResourceView* mSRV;
    ID3D11Texture2D* mTexture;

    std::vector<std::wstring> mFontNames;
    std::vector<UINT> mFontSizes;

    wchar_t* mCharSet;
    UINT mCharSetNum;

    ID3D11Device* mDevice;
    ID3D11DeviceContext* mCtx;
};

class FontSheet
{
public:
    FontSheet(
        std::wstring fontName = L"Consolas", 
        float fontSizeInPixel = 30.f, 
        int textureWidth = 1024,

        Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleBold, 
        Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias,
        Gdiplus::CompositingMode compositeMode = Gdiplus::CompositingModeSourceCopy,
        Gdiplus::Color bgc = Gdiplus::Color(0, 0, 0, 0),
        Gdiplus::Color fgc = Gdiplus::Color(255, 255, 255, 255)
        );
    ~FontSheet();

    Box2D* getSrcRect(WCHAR c);

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
    std::vector<Box2D> mCharRects;

    int mCharHeight;
    int mSpaceWidth;
    int mTextureHeight;
};

class FontEngine
{
public:
    FontEngine();
    ~FontEngine();

    void createFontSheet(ID3D11DeviceContext* ctx, ID3D11Device* dvc, FontSheet& fs, std::wstring debugFilename = L"");

protected:
    int getMinX(Gdiplus::Bitmap& bitmap);
    int getMaxX(Gdiplus::Bitmap& bitmap);
};

#endif