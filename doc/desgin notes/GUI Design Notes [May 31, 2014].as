Customized Font Format:
>> char left top right bottom 
>> A    100  0   150   20
>> B    151  0   200   20
>> C    201  0   250   20
>> D    251  0   300   20

texture:
    id3d11Resource
    file_name

save(out_ar)
    d3dx11SaveTextureToFile(file_name, id3d11Resource)
    out_ar << file_name
    

load(in_ar)
    in_ar >> file_name
    d3dx11LoadTextureFromFile(file_name)

Pseudo code TextureAtlas
-----------------------------------------
SrcRect:
    int left, top, right, bottom;

AtlasDesc:
    string  sub_texture_name;
    SrcRect sub_texture_rect;

TextureAtlas:
    string              dds_file_name;
    id3d11Resource      resource;
    id3d11SRV           srv;
    vector<AtlasDesc>   desc;
    save(Archive& ar):
        ar & dds_file_name & desc;
        d3dx11SaveTextureToFile(dds_file_name, resource);

    load(Archive& ar):
        ar & dds_file_name & desc;
        d3dx11LoadTextureFromFile(dds_file_name, resource, srv);

serialize(Archive& ar, vector<TextureAtlas>& objs):
    ar & objs;

TextureMgr:
    vector<TextureAtlas> Textures;
    init():
        ifstream ifs(filename);
        boost::archive::some_archive ar(ifs);
        ar & Textures;
------------------------------------------------------------
 UINT                       Width;
    UINT                       Height;
    UINT                       Depth;
    UINT                       FirstMipLevel;
    UINT                       MipLevels;
    D3D11_USAGE                Usage;
    UINT                       BindFlags;
    UINT                       CpuAccessFlags;
    UINT                       MiscFlags;
    DXGI_FORMAT                Format;
    UINT                       Filter;
    UINT                       MipFilter;
    D3DX11_IMAGE_INFO*         pSrcInfo;
    ----------------------------------------------------
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

    





