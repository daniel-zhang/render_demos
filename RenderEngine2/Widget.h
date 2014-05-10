#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <vector>
#include "IRenderable2D.h"
#include "LayoutSolver.h"

struct GUIEvent;

class Widget : public IRenderable2D
{
public:
    friend LayoutSolver;

    // Ctor for root widget
    Widget(std::string name, Area2D& size, RGBA& color, LayoutSolver* solver);

    // Ctor for non-root widget
    Widget(std::string name,  Widget* parent, Area2D& size, PixelPadding& padding, RGBA& color,
        LayoutSolver* solver, LayoutType layoutType = WIDGET_LAYOUT_STREAM);

    ~Widget();

    // UI logic
    void dispatch(GUIEvent& evt);
    virtual void onLBtnDown(Point2D& clickPos);
    virtual void onLBtnUp(Point2D& clickPos);
    // TODO: we will need the mouse pos info too
    // like this: 
    // onMouseMove(Point2D& mousePos, Vector2D& movement);
    virtual void onMouseMove(Vector2D& movement);
    virtual void onResize(Area2D& newArea);

    void addChild(Widget* child){mChildren.push_back(child);}

    //
<<<<<<< HEAD
    virtual void onMouseEnter();
    virtual void onMouseLeave();
    virtual void onLBtnDown(){}
    virtual void onLBtnUp(){}
    virtual void onResize(Viewport& vp);
=======
    // getters
    //
    std::vector<Widget*>& getChildren(){return this->mChildren;}
    virtual Box2D getClientArea();

    // TODO: by making render data public, these getters are to be removed.
    virtual void getRenderInfo(Box2D& box, int& layoutDepth, RGBA& color);
    UINT getLayerDepth(){return mLayerDepth;}
>>>>>>> a8815b47191ea80dce029895a4617a5fbd1ce1fe

    //
    // Graphics
    //
    virtual bool isPointInside(Point2D& point);

protected:
    // Actions
    void resize(Area2D& newArea);
    void move(Vector2D& movement);

protected:
    enum WidgetState
    {
        NORMAL = 0,
        ON_HOVER,
        PRESSED_DOWN,
        DISABLED,
        STATE_NUMBER
    };

protected:
    WidgetState mState;
    LayoutType mLayoutType;
    LayoutSolver* mSolver;
    // Widget position relative to its parent
    Point2D mPos;
    PixelPadding mPadding;
    
    std::string mName;
    Widget* mParent;
    std::vector<Widget*> mChildren;
};


struct GUIEvent
{
    enum EventType 
    {
        MouseMove = 0,
        MouseLBtnDown,
        MouseLBtnUp
    };
    EventType mEventId;
    UINT mKeyId;
    Point2D mClickPos;
    Point2D mLastClickPos;
    Vector2D mMouseMovement;
};

class Dispatcher
{
public:
    void init(Widget* widget);
    void dispatch(GUIEvent& evt)
    {
        switch(evt.mEventId)
        {
        case GUIEvent::MouseLBtnDown:
            bool captureFlag = true;
            for (UINT i = 0; i < mWidget->getChildren().size(); ++i)
            {
                Widget*  child = mWidget->getChildren()[i];
                if (child->isPointInside(evt.mClickPos))
                {
                    child->dispatch(evt);
                    captureFlag = false;
                }
            }
            if (captureFlag)
            {
                mWidget->onLBtnDown(evt.mClickPos);
            }
            break;

        default:
            break;
        }

    }

private:
    Widget* mWidget;

};

#endif
