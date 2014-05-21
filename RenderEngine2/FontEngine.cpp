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

void FontEngine::createFontSheet(ID3D11Device* dvc,  FontSheet& fs, std::wstring debugFilename)
{
    ULONG_PTR token = NULL;
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

        Gdiplus::Bitmap fontSheetBitmap(fs.mTextureWidth, fs.mTextureHeight, PixelFormat32bppARGB);

        Gdiplus::Graphics fontSheetGraphics(&fontSheetBitmap);
        fontSheetGraphics.SetCompositingMode(fs.mCompositeMode);
        fontSheetGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));

        WCHAR str[2] = {' ', 0};
        for (UINT i = 1; i < fs.mCharSetNum; ++i)
        {
            str[0] = fs.mCharSet[i];
            charGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
            // draw st..r onto charBitmap
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
        fontSheetBitmap.Save((fs.mFontSheetBmpFilePath + fs.mFontSheetBmpFileName).c_str(), &clsid, NULL);

        Gdiplus::BitmapData bmData;
        fontSheetBitmap.LockBits(&Gdiplus::Rect(0, 0, fs.mTextureWidth, fs.mTextureHeight), 
            Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);  

        // Copy into a texture.
        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Width  = fs.mTextureWidth;
        texDesc.Height = fs.mTextureHeight;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_IMMUTABLE;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;        
        data.pSysMem = bmData.Scan0;
        data.SysMemPitch = fs.mTextureWidth* 4;
        data.SysMemSlicePitch = 0;

        dvc->CreateTexture2D(&texDesc, &data, &fs.mFontSheetTex);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        dvc->CreateShaderResourceView(fs.mFontSheetTex, &srvDesc, &fs.mFontSRV);

        fontSheetBitmap.UnlockBits(&bmData);  

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

FontSheet::FontSheet( std::wstring fontName /*= L"Consolas"*/, float fontSizeInPixel /*= 96.f*/, int textureWidth /*= 1024*/,  Gdiplus::FontStyle fontStyle /*= Gdiplus::FontStyleRegular*/, Gdiplus::TextRenderingHint hint /*= Gdiplus::TextRenderingHintAntiAlias*/, Gdiplus::CompositingMode compositeMode /*= Gdiplus::CompositingModeSourceCopy*/, Gdiplus::Color bgc /*= Gdiplus::Color(0, 0, 0, 0)*/, Gdiplus::Color fgc /*= Gdiplus::Color(255, 255, 255, 255) */ )
    :
    mFontName(fontName),
    mFontSizeInPixel(128.f),
    mTextureWidth(textureWidth),
    mFontStyle(fontStyle),
    mHint(hint),
    mCompositeMode(compositeMode),
    mBackgroundColor(bgc),
    mForegroundColor(fgc),
    mCharSet(0),
    mCharSetNum(0),
    mFontSheetBmpFilePath(L"../Textures/"),
    mFontSheetTex(0),
    mFontSRV(0)
{
    // ASCII
    mStartChar = 32;
    mEndChar = 126;
    WCHAR newline = 10;
        
    // num: [32, 126] + 10
    mCharSetNum = mEndChar - mStartChar + 1 + 1;
    // +1 for to prepare space for the trailing zero
    mCharSet = new WCHAR[mCharSetNum + 1];

    //
    for(UINT i = 0; i < mCharSetNum - 1; ++i)
    {
        mCharSet[i] = mStartChar + i;
    }

    // Append newline(\n)
    mCharSet[mCharSetNum - 1] = newline;

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
}

CD3D11_RECT* FontSheet::getSrcRect( WCHAR c )
{
    if (c >= mStartChar && c <= mEndChar)
    {
        return &(mCharRects[c - mStartChar]);
    }
    else if (c == 10)
    {
        return &(mCharRects[mCharSetNum-1]);
    }
    else
        return 0;
}


