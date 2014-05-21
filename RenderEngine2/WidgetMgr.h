#ifndef WIDGET_MGR_H
#define WIDGET_MGR_H

#include "Color.h"
#include "Math2D.h"
#include <vector> 
#include "Input.h"
#include "SpriteRenderer.h"
#include "Root.h"

/*
void someUserCode()
{
    widgetMgr.init(...)

    Container* container = new Container( ...);
    widgetMgr.addWidget(container);

    Button* b = new Button(...);
    widgetMgr.addWidget(b, container);

    widgetMgr.draw();
}
*/

class WidgetMgr
{
public:
    WidgetMgr();
    ~WidgetMgr();

    bool init(Input* input, ID3D11Device* device, ID3D11DeviceContext* context );
    void addWidget(Widget* widget, Widget* parent = NULL);

    void draw();

    D3DEnv* getRenderEnv();

private:
    void submitToRenderer(Widget* widget);

    //
    // Event handlers
    //
    bool hookEventHandlers();

    // WidgetMgr translates from the incoming boost::signals2 into the root widget's dispatcher
    // The specific event dispatch is implemented in each widget
    void onViewportResize(int width, int height);
    void onMouseMove(int x, int y);
    void onMouseLBtnDown(int x, int y);
    void onMouseLBtnUp(int x, int y);
    void onMouseWheelUp(int x, int y);
    void onMouseWheelDown(int x, int y);
    /*
    TODO:
    void onMouseRBtnDown(int x, int y);
    void onMouseRBtnUp(int x, int y);
    */

private:
    SpriteRenderer mSpriteRenderer;
    Widget* mRoot;

    Point2D mLastMousePos;
    Input* mInput;
};

#endif