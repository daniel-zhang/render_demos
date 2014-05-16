#ifndef ROOT_H
#define ROOT_H

#include "Widget.h"

class Root : public Widget
{
public:
    Root(Area2D& vpSize); 

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
    virtual void move(Vector2D& movement);
    virtual void moveTo(Point2D& pos);
    virtual void resize(Area2D& newSize);
    virtual void solveLayout();


    virtual void getPaddedRect(Box2D& box);
    virtual void getMarginRect(Box2D& box);
};

#endif