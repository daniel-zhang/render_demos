#ifndef BUTTON_H
#define BUTTON_H

#include "Widget.h"
#include "Text.h"
#include "FontEngine.h"

class Button: public Widget
{
public:
    struct Attributes
    {
        Point2D position;
        Area2D size;
        PixelPadding padding;
        PixelMargin margin;
        LayoutType layout_type;
        bool clipped_by_parent;
        bool linked_to_parent;

        bool label_visible;
        std::wstring label_string;
        FontSheet* label_font_sheet;
        int label_font_size;
        bool center_label_string;
        RGBA label_string_color;
        RGBA active_label_string_color;
        RGBA label_background_color;
        RGBA active_label_background_color;

        bool info_visible;
        Area2D info_frame_size;
        std::wstring info_string;
        FontSheet* info_font_sheet;
        int info_font_size;
        bool center_info_string;
        RGBA info_string_color;
        RGBA active_info_string_color;
        RGBA info_background_color;
        RGBA active_info_background_color;

        Attributes& operator=(const Attributes& rhs);
        Attributes();
    };

    Button(D3DEnv* env, Attributes& attr);

    virtual bool init();
    virtual void onLBtnDown( GUIEvent& e );
    virtual void onMouseEnter(GUIEvent& e); 
    virtual void onMouseLeave(GUIEvent& e);
    virtual UINT getValidSpritesNumber();

protected:
    Sprite2D* mLabelBkgSprite;
    Sprite2D* mInfoBkgSprite;
    Text* mLabelTextWidget;
    Text* mInfoTextWidget;

    Attributes mAttr;
};

#endif