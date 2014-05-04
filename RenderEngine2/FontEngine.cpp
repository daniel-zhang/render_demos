#include "FontEngine.h"
#include <Gdiplus.h>
#include <cassert>

/*
1. Specify the font and the range of chars
2. Create a 'bounding' bitmap for a single char
3. Create fontsheet bitmap onto which all the char bitmaps are printed
3. Create a texture from the fontsheet bitmaps
*/

// ASCII characters from 33='!' to 127.  
static const WCHAR StartChar = 33;
static const WCHAR EndChar = 126;
static const UINT NumChars = EndChar - StartChar + 1;

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
    mTextureWidth = 1024;
    //mFontName = L"Consolas";
    mFontName = L"Times New Roman";
}

FontEngine::~FontEngine()
{

}

void FontEngine::init()
{
    // temp parameters
    float fontSizeInPixel = 96.f;
    Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
    Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias;

    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput startupInput(NULL, TRUE, TRUE);
    Gdiplus::GdiplusStartupOutput startupOutput;
    Gdiplus::GdiplusStartup(&token, &startupInput, &startupOutput);

    {
        // Create a bitmap for each char
        // Bitmap(int width, int height, Gdiplus::PixelFormat)
        int charSize = static_cast<int>(2*fontSizeInPixel);
        Gdiplus::Bitmap charBitmap(charSize, charSize, PixelFormat32bppARGB);

        //
        Gdiplus::Graphics charGraphics(&charBitmap);
        charGraphics.SetPageUnit(Gdiplus::UnitPixel);
        charGraphics.SetTextRenderingHint(hint);

        //
        // name, style, unit(pixel) and size(in pixel)
        //
        Gdiplus::Font font(mFontName.c_str(), fontSizeInPixel, fontStyle, Gdiplus::UnitPixel);

        // Construct the charset
        WCHAR charset[NumChars + 1];
        for (UINT i = 0; i < NumChars; ++i)
        {
            charset[i] = StartChar + i;
        }
        // Append the trailing zero
        charset[NumChars] = 0;

        Gdiplus::RectF boundingBox;

        // Find out charHeight -- the tallest char of the char set 
        charGraphics.MeasureString(charset, NumChars, &font, Gdiplus::PointF(0.f, 0.f), &boundingBox);
        mCharHeight = static_cast<int>(boundingBox.Height+.5f);
          
        // Find out textureHeight -- rows * charHeight
        int rows = static_cast<int>(boundingBox.Width / mTextureWidth) + 1;
        mTextureHeight = rows * mCharHeight + 1;

        // Find out spaceWidth
        WCHAR space[2] = {' ', 0}; 
        charGraphics.MeasureString(space, 1, &font, Gdiplus::PointF(0.f, 0.f), &boundingBox);
        // Round width to int
        mSpaceWidth = static_cast<int>(boundingBox.Width + .5f);

        // build Font sheet bitmap
        Gdiplus::Bitmap fontSheetBmp(mTextureWidth, mTextureHeight, PixelFormat32bppARGB);
        Gdiplus::Graphics fontSheetGraphics(&fontSheetBmp);
        fontSheetGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        fontSheetGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));

        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
        UINT sheetPosX = 0;
        UINT sheetPosY = 0;
        WCHAR str[2] = {' ', 0};
        for (UINT i = 0; i < NumChars; ++i)
        {
            str[0] = charset[i];
            charGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
            // draw str onto charBitmap
            charGraphics.DrawString(str, 1, &font, Gdiplus::PointF(0.f, 0.f), &whiteBrush);
            int minX = getMinX(charBitmap);
            int maxX = getMaxX(charBitmap);
            int charWidth = maxX - minX;

            // new line in font sheet
            if (sheetPosX + charWidth > mTextureWidth)
            {
                sheetPosX = 0;
                sheetPosY += mCharHeight;
            }

            mCharRects.push_back(CD3D11_RECT(
                sheetPosX, sheetPosY, 
                sheetPosX + charWidth, sheetPosY + mCharHeight));

            fontSheetGraphics.DrawImage(&charBitmap, sheetPosX, sheetPosY,
                minX, 0, charWidth, mCharHeight, Gdiplus::UnitPixel);

            sheetPosX += charWidth + 1;
        }
        CLSID clsid;
        GetEncoderClsid(L"image/bmp", &clsid);
        std::wstring fname = L"../Textures/" + mFontName + L".bmp"; 
        fontSheetBmp.Save(fname.c_str(), &clsid, NULL);

    }// end of scope
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
