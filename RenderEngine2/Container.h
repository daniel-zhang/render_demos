#ifndef CONTAINER_H
#define CONTAINER_H

#include "Widget.h"

class Container : public Widget
{
public:
    Container(
        D3DEnv* env, 
        Point2D& pos, Area2D& size, const RGBA& color,
        PixelPadding& padding, PixelMargin& margin, 
        LayoutType layoutType = WIDGET_LAYOUT_STATIC);

    virtual bool init();
    virtual void onMouseEnter(GUIEvent& e);
    virtual void onMouseLeave(GUIEvent& e);

    Sprite2D* mBkgSprite;
};
#endif