#ifndef WIDGET_MGR_H
#define WIDGET_MGR_H

#include "Color.h"
#include "Math2D.h"
#include <vector> 
#include "Input.h"
#include "GUIRenderer.h"
#include "FontEngine.h"
#include "Widget.h"
#include "Root.h"

/*
void someUserCode()
{
    widgetMgr.init(...)

    Container* container = new Container(WidgetMgr.getRoot(), ...);
    Button* b = new Button(container, ...);

    widgetMgr.draw();
}
*/

class WidgetMgr
{
public:
    typedef std::vector<Widget*> WidgetArray;

    WidgetMgr();
    ~WidgetMgr();

    bool init(Input* input, ID3D11Device* device, ID3D11DeviceContext* context );

    Widget* getRoot();

    FontSheet* getFontSheet();

    void draw();

    // Widgets use this method to notify their mgr that they needs to be re-batched to renderer
    void setSubmit();

private:

    void submit();

    void _submit(Widget* w, Point2D& parentAbsPos);

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
    GUIRenderer* mRenderer;
    Point2D mLastMousePos;
    Input* mInput;

    Widget* mRoot;
    bool mSubmit;

    FontEngine mFontEngine;
    FontSheet mFontSheet;
};

#endif