#ifndef LIST_H
#define LIST_H

#include "Button.h"
#include "Widget.h"
#include "FontEngine.h"

class List : public Widget
{
public:
    struct ItemDesc
    {
        ItemDesc(){}
        ItemDesc(std::wstring label_, std::wstring info_)
        {
            label = label_;
            info = info_;
        }
        std::wstring label;
        std::wstring info;
    };

    struct Attributes
    {
        bool visible;
        Point2D position;
        Area2D size;
        PixelPadding padding;
        PixelMargin margin;
        LayoutType layout_type;
        bool clipped_by_parent;
        bool linked_to_parent;

        RGBA background_color;
        RGBA active_background_color;
        int font_size;
        FontSheet* font_sheet;


        std::vector<ItemDesc> item_desc;

        Attributes();
        Attributes& operator=(Attributes& rhs);
    };

    List(D3DEnv* env, Attributes& attr);

    virtual bool init();
    
    virtual void onMouseWheelUp(GUIEvent& e);
    virtual void onMouseWheelDown(GUIEvent& e);
    virtual void onMouseEnter(GUIEvent& e);
    virtual void onMouseLeave(GUIEvent& e);

    void solveLayout();

    Sprite2D* mBkgSprite;
    FontSheet* mFontSheet;
    Attributes mAttr;
};



#endif