#ifndef WIDGET_H
#define WIDGET_H
#include <string>
#include <vector>
#include "GUIEvent.h"
#include "Math2D.h"
#include "Sprite2D.h"

enum LayoutType
{
    WIDGET_LAYOUT_CENTER = 0,
    WIDGET_LAYOUT_STATIC,
    WIDGET_LAYOUT_STREAM,
    WIDGET_LAYOUT_TEXT_STREAM,
    WIDGET_LAYOUT_TEXT_STREAM_CENTERED,
    WIDGET_LAYOUT_VERTICAL,
    WIDGET_LAYOUT_HORIZENAL,
    WIDGET_LAYOUT_DOCK_LEFT,
    WIDGET_LAYOUT_NOT_SPECIFIED
};

enum WidgetType
{
    WIDGET_TYPE_INVALID = 0,
    WIDGET_TYPE_ROOT, 
    WIDGET_TYPE_TEXTELEMENT,
    WIDGET_TYPE_TEXT,
    TOTAL_NUMBER
};

class Widget 
{
public:
    Widget(D3DEnv* env, Point2D& pos, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType); 

    bool operator==(const Widget& rhs){return this == &rhs;}
    bool operator!=(const Widget& rhs){return !(*this == rhs);}

    virtual ~Widget();

    virtual bool init() = 0;
    void clear();
    void addChild(Widget* child);
    void dispatch(GUIEvent& evt);

    void setActive();
    void setInActive();
    bool isActive(){return mIsActive;}
    void setChildActive(Widget* child);

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

    // Actions
    virtual void solveLayout() = 0;
    virtual void resize(Area2D& newSize) = 0;
    virtual void move(Vector2D& movement);
    virtual void moveTo(Point2D& pos);

    virtual void beforeDrawChildren(){}
    virtual void afterDrawChildren(){}

    // Helpers
    void getPaddedRect(Box2D& box);
    void getMarginedArea(Area2D& area);
    enum WidgetState
    {
        UN_INITIALIIZED = 0,
        NORMAL,
        ON_HOVER,
        PRESSED_DOWN,
        DISABLED,
        STATE_NUMBER
    };

    WidgetState mState;
    LayoutType mLayoutType;

    Box2D mLogicalBox;
    RGBA mColor;
    PixelPadding mPadding;
    PixelMargin mMargin;
    bool mVisible;

    D3DEnv* mEnv;
    Sprite2D mSprite;
    
    WidgetType mType;
    Widget* mParent;
    Widget* mActiveChild;

    std::vector<Widget*> mChildren;
    std::vector<Widget*> mDepthQueue;
    std::vector<Widget*> mDispatchQueue;

    bool mIsActive;
};


#endif
