#include "FontEngine.h"
#include <cassert>

/*
1. Specify the font and the range of chars
2. Create a 'bounding' bitmap for a single char
3. Create fontsheet bitmap onto which all the char bitmaps are printed
3. Create a texture from the fontsheet bitmaps
*/


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if(size == 0)
        return -1;  // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL)
        return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j)
    {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }    
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

FontEngine::FontEngine()
{
}

FontEngine::~FontEngine()
{

}

void FontEngine::createFontSheet( FontSheet& fs, std::wstring debugFilename )
{
    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput startupInput(NULL, TRUE, TRUE);
    Gdiplus::GdiplusStartupOutput startupOutput;
    Gdiplus::GdiplusStartup(&token, &startupInput, &startupOutput);
    {// Scope begins

        int charSize = static_cast<int>(2 * fs.mFontSizeInPixel); // Why mult by 2?
        Gdiplus::Bitmap charBitmap(charSize, charSize, PixelFormat32bppARGB);

        Gdiplus::Graphics charGraphics(&charBitmap);
        charGraphics.SetPageUnit(Gdiplus::UnitPixel);
        charGraphics.SetTextRenderingHint(fs.mHint);

        Gdiplus::Font font(fs.mFontName.c_str(), fs.mFontSizeInPixel, fs.mFontStyle, Gdiplus::UnitPixel);

        Gdiplus::RectF boundingBox;
        charGraphics.MeasureString(fs.mCharSet, fs.mCharSetNum, &font, Gdiplus::PointF(0.f, 0.f), &boundingBox);
        fs.mCharHeight = static_cast<int>(boundingBox.Height+.5f);
          
        //
        // Given texture width, font, and char set, find out texture height
        //
        int rows = static_cast<int>(boundingBox.Width / fs.mTextureWidth) + 1;
        fs.mTextureHeight= rows * fs.mCharHeight + 1;

        // Find out spaceWidth
        WCHAR space[2] = {' ', 0}; 
        charGraphics.MeasureString(space, 1, &font, Gdiplus::PointF(0.f, 0.f), &boundingBox);
        fs.mSpaceWidth = static_cast<int>(boundingBox.Width + .5f);

        //
        // GDI+ does not print space, this is a workaround:
        // Insert space at the beginning
        //
        int sheetPosX = 0;
        int sheetPosY = 0;
        fs.mCharRects.push_back(CD3D11_RECT( sheetPosX, sheetPosY, sheetPosX + fs.mSpaceWidth, sheetPosY + fs.mCharHeight));
        sheetPosX += fs.mSpaceWidth;

        fs.mFontSheetBmp = new Gdiplus::Bitmap(fs.mTextureWidth, fs.mTextureHeight, PixelFormat32bppARGB);

        Gdiplus::Graphics fontSheetGraphics(fs.mFontSheetBmp);
        fontSheetGraphics.SetCompositingMode(fs.mCompositeMode);
        fontSheetGraphics.Clear(fs.mBackgroundColor);
        Gdiplus::SolidBrush whiteBrush(fs.mForegroundColor);

        WCHAR str[2] = {' ', 0};
        for (UINT i = 1; i < fs.mCharSetNum; ++i)
        {
            str[0] = fs.mCharSet[i];
            charGraphics.Clear(fs.mBackgroundColor);
            // draw str onto charBitmap
            charGraphics.DrawString(str, 1, &font, Gdiplus::PointF(0.f, 0.f), &whiteBrush);
            int minX = getMinX(charBitmap);
            int maxX = getMaxX(charBitmap);
            int charWidth = maxX - minX;

            // new line in font sheet
            if (sheetPosX + charWidth > fs.mTextureWidth)
            {
                sheetPosX = 0;
                sheetPosY += fs.mCharHeight;
            }

            fs.mCharRects.push_back(CD3D11_RECT( sheetPosX, sheetPosY, sheetPosX + charWidth, sheetPosY + fs.mCharHeight));

            fontSheetGraphics.DrawImage(&charBitmap, sheetPosX, sheetPosY,
                minX, 0, charWidth, fs.mCharHeight, Gdiplus::UnitPixel);

            sheetPosX += charWidth + 1;
        }

        CLSID clsid;
        GetEncoderClsid(L"image/bmp", &clsid);
        fs.mFontSheetBmpFileName = fs.mFontName + L"_" + debugFilename +  L".bmp"; 
        fs.mFontSheetBmp->Save((fs.mFontSheetBmpFilePath + fs.mFontSheetBmpFileName).c_str(), &clsid, NULL);

        // Font sheet bmp must be freed here... 
        // Basically it does not make sense to let FontSheet hold a pointer to the Bitmap object...
        // Let me fix it later.
        if (fs.mFontSheetBmp)
        {
            delete fs.mFontSheetBmp;
            fs.mFontSheetBmp = 0;
        }

    }// Scope ends

    Gdiplus::GdiplusShutdown(token);

}

int FontEngine::getMaxX( Gdiplus::Bitmap& bitmap )
{
    int width  = bitmap.GetWidth();
    int height = bitmap.GetHeight();

    for(int x = width -1 ; x >= 0; --x)
    {
        for(int y = 0; y < height; ++y)
        {
            Gdiplus::Color color;
            bitmap.GetPixel(x, y, &color);
            if(color.GetAlpha() > 0)
            {
                return x;
            }
        }
    }
    return 0;
}

int FontEngine::getMinX( Gdiplus::Bitmap& bitmap )
{
    int width  = bitmap.GetWidth();
    int height = bitmap.GetHeight();

    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            Gdiplus::Color color;
            bitmap.GetPixel(x, y, &color);
            if(color.GetAlpha() > 0)
            {
                return x;
            }
        }
    }
    return 0;
}

FontSheet::FontSheet( std::wstring fontName /*= L"Consolas"*/, float fontSizeInPixel /*= 96.f*/, int textureWidth /*= 1024*/, WCHAR startChar /*= 33*/, WCHAR endChar /*= 126*/, Gdiplus::FontStyle fontStyle /*= Gdiplus::FontStyleRegular*/, Gdiplus::TextRenderingHint hint /*= Gdiplus::TextRenderingHintAntiAlias*/, Gdiplus::CompositingMode compositeMode /*= Gdiplus::CompositingModeSourceCopy*/, Gdiplus::Color bgc /*= Gdiplus::Color(0, 0, 0, 0)*/, Gdiplus::Color fgc /*= Gdiplus::Color(255, 255, 255, 255) */ )
    :
    mFontName(fontName),
    mFontSizeInPixel(fontSizeInPixel),
    mTextureWidth(textureWidth),
    mStartChar(startChar),
    mEndChar(endChar),
    mFontStyle(fontStyle),
    mHint(hint),
    mCompositeMode(compositeMode),
    mBackgroundColor(bgc),
    mForegroundColor(fgc),
    mCharSet(0),
    mCharSetNum(0),
    mFontSheetBmp(0),
    mFontSheetBmpFilePath(L"../Textures/")
{
        // Construct the charset
        mCharSetNum = mEndChar - mStartChar + 1;
        mCharSet = new WCHAR[mCharSetNum + 1];

        for(UINT i = 0; i < mCharSetNum; ++i)
        {
            mCharSet[i] = mStartChar + i;
        }

        // Append the trailing zero
        mCharSet[mCharSetNum] = 0;
}

FontSheet::~FontSheet()
{
    if(mCharSet)
    {
        delete[] mCharSet;
        mCharSet = 0;
    }

    if (mFontSheetBmp)
    {
        delete mFontSheetBmp;
        mFontSheetBmp = 0;
    }
}

CD3D11_RECT* FontSheet::getSrcRect( WCHAR c )
{
    if (c < mStartChar || c > mEndChar)
    {
        return 0;
    }
    return &(mCharRects[c - mStartChar]);
}


