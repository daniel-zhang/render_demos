#ifndef WIDGET_MGR_2_H
#define WIDGET_MGR_2_H

#include "Color.h"
#include "Math2D.h"
#include <vector> 
#include "Input.h"
#include "LayoutSolver.h"
#include "Widget.h"
#include "GUIRenderer.h"
class WidgetMgr2
{
public:
    friend GUIRenderer;
    typedef std::vector<Widget*> WidgetArray;

    WidgetMgr2();
    ~WidgetMgr2();

    bool init(Input* input, GUIRenderer* renderer);
    Widget* createRootWidget(std::string name, Area2D& viewportSize, RGBA& color);
    Widget* createChildWidget(
        std::string name, 
        Widget* parent, 
        Area2D& size, PixelPadding& padding, RGBA& color,
        LayoutType layoutType = WIDGET_LAYOUT_NOT_SPECIFIED);

    void createBegin(){}
    void createEnd()
    {
        mSolver.solve(mRoot);
    }

    //
    // Getters
    //
    WidgetArray& getWidgets(){return mWidgets;}

private:
    //
    // Event handlers
    //
    bool hookEventHandlers();

    // WidgetMgr just wraps the incoming event and forwards it to the root widget
    // The specific event dispatch is implemented in Widget
    void onViewportResize(int width, int height);
    
    void onMouseMove(int x, int y);
    void onMouseLBtnDown(int x, int y);
    void onMouseLBtnUp(int x, int y);
    /*
    TODO:
    void onMouseRBtnDown(int x, int y);
    void onMouseRBtnUp(int x, int y);
    void onMouseWheelUp(int x, int y);
    void onMouseWheelDown(int x, int y);
    */

private:
    GUIRenderer* mRenderer;
    Point2D mLastMousePos;
    Input* mInput;
    Widget* mRoot;
    WidgetArray mWidgets;
    LayoutSolver mSolver;
};

#endif