#ifndef ROOT_H
#define ROOT_H

#include "Widget.h"

class Root : public Widget
{
public:
    Root(D3DEnv* env, Area2D& vpSize); 

    virtual bool init();

    //virtual void onLayoutChanged(GUIEvent& evt);
    virtual void onResize(GUIEvent& e);
    virtual void onLBtnDown(GUIEvent& e);
    virtual void onLBtnUp(GUIEvent& e);
    virtual void onMouseMove(GUIEvent& e);
    virtual void onMouseWheelUp(GUIEvent& e);
    virtual void onMouseWheelDown(GUIEvent& e);
    virtual void onMouseEnter(GUIEvent& e);
    virtual void onMouseLeave(GUIEvent& e);

    virtual void resize(Area2D& newSize);
    virtual void solveLayout();
};

#endif