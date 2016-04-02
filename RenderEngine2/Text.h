#ifndef TEXT_H
#define TEXT_H
#include "Widget.h"
#include "TextElement.h"
#include <string>

class FontSheet2;

class Text : public Widget
{
public:
    struct Attributes 
    {
        Point2D position;
        Area2D size;
        PixelPadding padding;
        PixelMargin margin;
        std::wstring text_string;
        std::wstring font_name;
        int font_size;
        FontSheet* font_sheet;
        bool center_text;
        RGBA font_color;
        RGBA active_font_color;
        LayoutType layout_type;
        bool visible;
        bool clipped_by_parent;
        bool linked_to_parent;

        Attributes()
        {
            position          = Point2D();
            size              = Area2D(80, 40);
            padding           = PixelPadding();
            margin            = PixelMargin();
            text_string       = L"Text";
            font_name         = L"Arial";
            font_size         = 20;
            font_sheet        = NULL;
            center_text       = true;
            font_color        = RGBAColor::White;
            active_font_color = RGBAColor::Blue;
            layout_type       = WIDGET_LAYOUT_STATIC;
            visible           = true;
            clipped_by_parent = true;
            linked_to_parent  = true;
        }
    };

    Text(D3DEnv* env, Attributes& attr);

    Text( D3DEnv* env, 
        Point2D& pos, Area2D& size, 
        int fontSize,  FontSheet* fontSheet, bool centerText = true,
        const RGBA& fontColor = RGBAColor::White, 
        const RGBA& activeFontColor = RGBAColor::Blue,
        LayoutType layoutType = WIDGET_LAYOUT_STATIC
        );

    virtual bool init();

    void setText(std::wstring& s);
    void setTextSize(int newSize);
    void setTextColor(RGBA& color);

    virtual void beforeDrawSelf();
    virtual void afterDrawSelf();

    virtual void onMouseEnter(GUIEvent& e);
    virtual void onMouseLeave(GUIEvent& e);

    virtual void resize(Area2D& newSize);

protected:
    void generateTextSprites();
    void getCharactorRects(wchar_t c, Area2D& dstSize, FBox2D& srcRect);
    virtual UINT getValidSpritesNumber();
    void layoutText_bak(bool centered);
    void layoutText(bool centered);

    std::wstring mFontName;
    int mFontSize;
    FontSheet* mFontSheet;
    std::wstring mString;
    bool mCenterText;

    ID3D11ShaderResourceView* mSRV; 

    RGBA mFontColor;
    RGBA mActiveFontColor;
};

#endif