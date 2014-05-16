#ifndef CONTAINER_H
#define CONTAINER_H

#include "Widget.h"

class Container : public Widget
{
public:
    Container(Widget* parent, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType); 

    virtual void onLayoutChanged(GUIEvent& evt);
    virtual void onResize(GUIEvent& e);

    virtual void onLBtnDown(GUIEvent& e);
    virtual void onLBtnUp(GUIEvent& e);

    virtual void onMouseMove(GUIEvent& e);

    virtual void onMouseWheelUp(GUIEvent& e);
    virtual void onMouseWheelDown(GUIEvent& e);

    virtual void onMouseEnter(GUIEvent& e);
    virtual void onMouseLeave(GUIEvent& e);

protected:
    bool mPosFixed;
};
#endif