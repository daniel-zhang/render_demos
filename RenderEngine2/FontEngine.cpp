#include "FontEngine.h"
#include <cassert>
#include "TextureMgr.h"

#include <sys/stat.h>

// Start: serialization specific
#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
// End: serialization specific

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

void FontEngine::createFontSheet(ID3D11DeviceContext* ctx, ID3D11Device* dvc,  FontSheet& fs, std::wstring debugFilename)
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
        fs.mCharRects.push_back(Box2D(sheetPosX, sheetPosY, sheetPosX + fs.mSpaceWidth, sheetPosY + fs.mCharHeight));
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

            fs.mCharRects.push_back(Box2D(sheetPosX, sheetPosY, sheetPosX + charWidth, sheetPosY + fs.mCharHeight));

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

        // Temp start
        // Save texture2D to file
        HRESULT h = D3DX11SaveTextureToFile(
            ctx, fs.mFontSheetTex, D3DX11_IFF_DDS, 
            (L"../Textures/" + fs.mFontName + L"_" + debugFilename +  L".dds").c_str());

        ID3D11ShaderResourceView* srv = TextureMgr::getTexture(fs.mFontName + L"_" + debugFilename +  L".dds");
        //fs.mFontSRV = srv;
        // Temp end 

    }// Scope ends

    // Temp start
    std::string filename = "good";
    struct stat buff;
    bool flag1 = (stat(filename.c_str(), &buff) != -1);
    bool fileExist = (INVALID_FILE_ATTRIBUTES != GetFileAttributes(L"good"));
    // Temp end 

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

Box2D* FontSheet::getSrcRect( WCHAR c )
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



FontMgr::FontMgr( ID3D11Device* device, ID3D11DeviceContext* context ) 
    :mTexture(0), mSRV(0), mDevice(device), mCtx(context),
    mCacheFileName(L"FontCache"), mFontTextureFileName(L"FontAtlas"), mFilePath(L"../Textures/"), 
    mTextureWidth(1400), mTextureHeight(0)
{
    mFontNames.push_back(L"Consolas");
    mFontNames.push_back(L"Arial");
    mFontNames.push_back(L"Arial Unicode MS");
    mFontNames.push_back(L"Times New Roman");

    mFontSizes.push_back(20);
    mFontSizes.push_back(90);
    mFontSizes.push_back(160);

    // Populate char set: ASCII [32, 126]
    wchar_t charSetStart = L' '; 
    wchar_t charSetEnd   = L'~';
    mCharSetNum          = charSetEnd - charSetStart + 1;
    mCharSet             = new WCHAR[mCharSetNum + 1];

    for(UINT i = 0; i < mCharSetNum; ++i)
    {
        mCharSet[i] = charSetStart + i;
    }
    // Append the trailing zero
    mCharSet[mCharSetNum] = 0;
}

FontMgr::~FontMgr()
{
    if(mCharSet)
    {
        delete[] mCharSet;
        mCharSet = NULL;
        mCharSetNum = 0;
    }
}

void FontMgr::init()
{
    /*
    pseudo:
    if serialization or texture file does not exist:
        create fonts
        serialize fonts
        save SRV to dds file
    else
        de_serialize(serilazationFile)
        d3dx11createSRVFromFile(textureFile)
    */
    if (GetFileAttributes((mFilePath + mCacheFileName).c_str()) != INVALID_FILE_ATTRIBUTES && 
        GetFileAttributes((mFilePath + mFontTextureFileName + L".dds").c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        // Load from cache on disc
        {
            std::ifstream ifs(mFilePath + mCacheFileName); 
            boost::archive::text_iarchive ar(ifs);
            ar & mFonts;
        }
        if (validateCache())
        {
            HR( D3DX11CreateShaderResourceViewFromFile( mDevice, (mFilePath + mFontTextureFileName + L".dds").c_str(), 0, 0, &mSRV, 0 ) );
        }
        else
        {
            createFromScratch();
        }
    }
    else
    {
        createFromScratch();
    }
}

void FontMgr::createFonts()
{
    mTextureHeight = calcTextureHeight(mTextureWidth);

    ULONG_PTR token = NULL;
    Gdiplus::GdiplusStartupInput startupInput(NULL, TRUE, TRUE);
    Gdiplus::GdiplusStartupOutput startupOutput;
    Gdiplus::GdiplusStartup(&token, &startupInput, &startupOutput);
    {// Scope begins
        // Create a single bitmap for font drawing
        Gdiplus::Bitmap fontBmp(mTextureWidth, mTextureHeight, PixelFormat32bppARGB);
        Gdiplus::Graphics fontGraphics(&fontBmp);
        fontGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        fontGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
        Point2D insertPos;

        // Iterate over all possible combinations of font families and sizes
        // and draw each combination onto the font bitmap
        for (UINT i = 0; i < mFontSizes.size(); ++i)
        {
            for (UINT j = 0; j < mFontNames.size(); ++j)
            {
                int fontSize = mFontSizes[i];
                std::wstring fontName = mFontNames[j];
                FontSheet2 fontsheet(fontName, fontSize);
                Gdiplus::Font font(fontName.c_str(), static_cast<float>(fontSize), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

                Gdiplus::Bitmap charBitmap(fontSize*2, fontSize*2, PixelFormat32bppARGB);
                Gdiplus::Graphics charGraphics(&charBitmap);
                charGraphics.SetPageUnit(Gdiplus::UnitPixel);
                charGraphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

                // Measure char height
                Gdiplus::RectF boundingBox;
                charGraphics.MeasureString(mCharSet, mCharSetNum, &font, Gdiplus::PointF(0.f, 0.f), &boundingBox);
                fontsheet.mCharHeight = static_cast<int>(boundingBox.Height+.5f); 

                // Draw charset onto fontBmp using specified font
                wchar_t str[2] = {L' ', 0};
                for (UINT i = 0; i < mCharSetNum; ++i)
                {
                    str[0] = mCharSet[i];
                    charGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
                    Gdiplus::Status status = 
                    charGraphics.DrawString(str, 1, &font, Gdiplus::PointF(0.f, 0.f), &whiteBrush);
                    int minX = getCharMinX(charBitmap);
                    int maxX = getCharMaxX(charBitmap);
                    int charWidth = maxX - minX;

                    // Start a new line if the char to be drawn exceeds the right boundary
                    if (insertPos.x + charWidth > static_cast<int>(mTextureWidth))
                    {
                        insertPos.x = 0;
                        insertPos.y += fontsheet.mCharHeight;
                    }
                    fontsheet.mCharRects.push_back(
                        Box2D(insertPos, Area2D(charWidth, fontsheet.mCharHeight)) );

                    status = 
                        fontGraphics.DrawImage(&charBitmap, insertPos.x, insertPos.y,
                        minX, 0, charWidth, fontsheet.mCharHeight, Gdiplus::UnitPixel);

                    // Move insertPos to the next position
                    insertPos.x += charWidth + 1;
                }
                // Instead of exhausted packing algorithm, just start a new line 
                // after the current font drawing is done
                insertPos.x = 0;
                insertPos.y += fontsheet.mCharHeight;

                mFonts.push_back(fontsheet); 
            }
        }// End of for-for

        // Save font bmp to .bmp file for debug purpose
        CLSID clsid;
        GetEncoderClsid(L"image/bmp", &clsid);
        Gdiplus::Status status = 
        fontBmp.Save((mFilePath+ mFontTextureFileName + L".bmp").c_str(), &clsid, NULL);

        // Create d3d texture from font bmp
        Gdiplus::Rect fontRect(0, 0, mTextureWidth, mTextureHeight);
        createTextureFromBitmap(fontBmp, fontRect, &mTexture, &mSRV);

    }// Scope ends
    Gdiplus::GdiplusShutdown(token);
}

void FontMgr::createTextureFromBitmap( Gdiplus::Bitmap& bmp, Gdiplus::Rect& bmpRect, ID3D11Texture2D** ppTexture, ID3D11ShaderResourceView** ppSRV )
{
    Gdiplus::BitmapData bmData;
    bmp.LockBits(&bmpRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);  

    // Copy into a texture.
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width  = bmpRect.Width;
    texDesc.Height = bmpRect.Height;
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
    data.SysMemPitch = bmpRect.Width* 4;
    data.SysMemSlicePitch = 0;

    mDevice->CreateTexture2D(&texDesc, &data, &(*ppTexture));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    mDevice->CreateShaderResourceView(*ppTexture, &srvDesc, &(*ppSRV));

    bmp.UnlockBits(&bmData);  
}

int FontMgr::calcTextureHeight(int width)
{
    int height = 0;

    ULONG_PTR token = NULL;
    Gdiplus::GdiplusStartupInput startupInput(NULL, TRUE, TRUE);
    Gdiplus::GdiplusStartupOutput startupOutput;
    Gdiplus::GdiplusStartup(&token, &startupInput, &startupOutput);
    {
        Gdiplus::Bitmap bmp(1, 1, PixelFormat32bppARGB);
        Gdiplus::Graphics charGraphics(&bmp);
        charGraphics.SetPageUnit(Gdiplus::UnitPixel);
        charGraphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        for (UINT i = 0; i < mFontSizes.size(); ++i)
        {
            for (UINT j = 0; j < mFontNames.size(); ++j)
            {
                // Create font according to specified font family and size
                int fontSize = mFontSizes[i];
                std::wstring fontName = mFontNames[j];
                Gdiplus::Font font(fontName.c_str(), static_cast<float>(fontSize), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

                // Measure char height
                Gdiplus::RectF boundingBox;
                charGraphics.MeasureString(mCharSet, mCharSetNum, &font, Gdiplus::PointF(0.f, 0.f), Gdiplus::StringFormat::GenericTypographic(), &boundingBox);
                int charHeight = static_cast<int>(boundingBox.Height+.5f);

                // Calculate texture height
                int rows = static_cast<int>(boundingBox.Width / width) + 1;
                //height += rows * charHeight + 1;
                height += rows * charHeight;
            }
        }
    }
    Gdiplus::GdiplusShutdown(token);

    return height;
}

int FontMgr::getCharMinX( Gdiplus::Bitmap& bitmap )
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
                return x;
        }
    }
    return 0;
}

int FontMgr::getCharMaxX( Gdiplus::Bitmap& bitmap )
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
                return x;
        }
    }
    return 0;
}

bool FontMgr::validateCache()
{
    if (mFonts.size() != mFontSizes.size() * mFontNames.size())
        return false;

    bool identical = true;
    for (UINT i = 0; i < mFonts.size(); ++i)
    {
        FontSheet2& font = mFonts[i];
        bool matched = false;
        for (UINT j = 0; j < mFontSizes.size(); ++j)
            for (UINT k = 0; k < mFontNames.size(); ++k)
                if (font.mFontName.compare(mFontNames[k]) == 0 && font.mFontPixelSize == mFontSizes[j] )
                    matched = true;
        if (matched == false)
        {
            identical = false;
            break;
        }
    }
    return identical;
}

void FontMgr::createFromScratch()
{
    mFonts.clear();
    createFonts();
    {
        std::ofstream ofs(mFilePath + mCacheFileName, std::ios_base::trunc); 
        boost::archive::text_oarchive ar(ofs);
        ar & mFonts;
    }
    HR(D3DX11SaveTextureToFile( mCtx, mTexture, D3DX11_IFF_DDS, 
        (mFilePath + mFontTextureFileName+  L".dds").c_str() ));
}


