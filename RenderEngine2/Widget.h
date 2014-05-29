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
    WIDGET_TYPE_CONTAINER,
    WIDGET_TYPE_BUTTON,
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

    bool isActive(){return mIsActive;}
    void setActive();
    void setInActive();
    void setChildActive(Widget* child);

    // Event handlers
    //virtual void onLayoutChanged(GUIEvent& evt) = 0;
    virtual void onResize(GUIEvent& e){} 
    virtual void onLBtnDown(GUIEvent& e); 
    virtual void onLBtnUp(GUIEvent& e);
    virtual void onMouseMove(GUIEvent& e);
    virtual void onMouseWheelUp(GUIEvent& e){}
    virtual void onMouseWheelDown(GUIEvent& e){}
    virtual void onMouseEnter(GUIEvent& e) {}
    virtual void onMouseLeave(GUIEvent& e) {}
    virtual void onKeyboardDown(GUIEvent& e){}
    virtual void onKeyboardUp(GUIEvent& e){}

    // Actions
    virtual void resize(Area2D& newSize){} 
    virtual void move(Vector2D& movement);
    virtual void moveTo(Point2D& pos);
    virtual void synClipAndClickBoxes();

    // Callbacks to WidgetMgr
    virtual void beforeDrawSelf(){}
    virtual void afterDrawSelf(){}
    virtual void beforeDrawChildren(){}
    virtual void afterDrawChildren(){}

    // Misc
    void getPaddedRect(Box2D& box);
    void getMarginedArea(Area2D& area);
    virtual UINT getValidSpritesNumber();

    enum WidgetState
    {
        UN_INITIALIZED = 0,
        NORMAL,
        ON_HOVER,
        PRESSED_DOWN,
        DISABLED,
        STATE_NUMBER
    };

    WidgetState mState;
    LayoutType mLayoutType;

    // Behaviors 
    bool mVisible;
    bool mClippedByParent;      
    bool mLinkedToParent;      

    Box2D mLogicalBox;
    Box2D mClipBox;
    Box2D mClickBox;

    PixelPadding mPadding;
    PixelMargin mMargin;
    RGBA mColor;

    D3DEnv* mEnv;
    std::vector<Sprite2D*> mSprites;

    // Children in their creation order.
    std::vector<Widget*> mChildren;
    // Children in their draw order: the first item is bottom-most, and the last is top-most.
    // Used by WidgetMgr to submit sprites to renderer.
    std::vector<Widget*> mDepthQueue;
    // Children in their dispatch order: the last item has highest priority to handle event.
    // Used by dispatcher to dispatch event to children.
    std::vector<Widget*> mDispatchQueue;

    // Linkage
    WidgetType mType;
    Widget* mParent;
    Widget* mActiveChild;
    bool mIsActive;
};


#endif
