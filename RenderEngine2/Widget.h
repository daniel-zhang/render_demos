#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <vector>
#include "IRenderable2D.h"
#include "GUIEvent.h"

enum LayoutType
{
    WIDGET_LAYOUT_CENTER = 0,
    WIDGET_LAYOUT_STATIC,
    WIDGET_LAYOUT_STREAM,
    WIDGET_LAYOUT_TEXT_STREAM,
    WIDGET_LAYOUT_VERTICAL,
    WIDGET_LAYOUT_HORIZENAL,
    WIDGET_LAYOUT_DOCK_LEFT,
    WIDGET_LAYOUT_NOT_SPECIFIED
};

class WidgetMgr;
class Widget : public IRenderable2D
{
public:
    Widget(Widget* parent, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType); 
    virtual ~Widget();

    // UI logic
    WidgetMgr* getWidgetMgr();
    void setWidgetMgr(WidgetMgr* w);

    void dispatch(GUIEvent& evt);
    void addChild(Widget* child);

    // 
    virtual void updateRenderable(Point2D& parentAbsPos, Point2D& myAbsPos) = 0;

    // Event handlers
    //virtual void onLayoutChanged(GUIEvent& evt) = 0;
    virtual void onResize(GUIEvent& e) = 0;
    virtual void onLBtnDown(GUIEvent& e) = 0;
    virtual void onLBtnUp(GUIEvent& e) = 0;
    virtual void onMouseMove(GUIEvent& e) = 0;
    virtual void onMouseWheelUp(GUIEvent& e) = 0;
    virtual void onMouseWheelDown(GUIEvent& e) = 0;
    virtual void onMouseEnter(GUIEvent& e) = 0;
    virtual void onMouseLeave(GUIEvent& e) = 0;

    // Helpers
    virtual void getPaddedRect(Box2D& box);
    virtual void getPaddedRectLocal(Box2D& box);
    virtual void getMarginRect(Box2D& box);

    // Actions
    virtual void move(Vector2D& movement) = 0;
    virtual void moveTo(Point2D& pos) = 0;
    virtual void resize(Area2D& newSize) = 0;
    virtual void solveLayout() = 0;

    enum WidgetState
    {
        NORMAL = 0,
        ON_HOVER,
        PRESSED_DOWN,
        DISABLED,
        STATE_NUMBER
    };

    WidgetState mState;
    LayoutType mLayoutType;

    Box2D mLocalRect;
    PixelPadding mPadding;
    PixelMargin mMargin;
    
    WidgetMgr* mWidgetMgr;
    Widget* mParent;
    std::vector<Widget*> mChildren;
};


#endif
