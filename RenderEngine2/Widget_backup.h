#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <vector>
#include "IRenderable2D.h"
#include "LayoutSolver.h"
#include "GUIEvent.h"

#include "TextBlock.h"

class Widget : public IRenderable2D
{
public:
    friend LayoutSolver;

    // Ctor for root widget
    Widget(Area2D& size, const RGBA& color, LayoutSolver* solver);

    // Ctor for non-root widget
    Widget(Widget* parent, Area2D& size, PixelPadding& padding, const RGBA& color,
        LayoutSolver* solver, LayoutType layoutType = WIDGET_LAYOUT_STREAM);

    ~Widget();

    // UI logic
    virtual void dispatch(GUIEvent& evt);

    virtual void onLayoutChanged(GUIEvent& evt);
    virtual void onResize(GUIEvent& e);
    virtual void onLBtnDown(GUIEvent& e);
    virtual void onLBtnUp(GUIEvent& e);
    virtual void onMouseMove(GUIEvent& e);
    virtual void onMouseWheelUp(GUIEvent& e);
    virtual void onMouseWheelDown(GUIEvent& e);
    virtual void onMouseEnter(GUIEvent& e){}
    virtual void onMouseLeave(GUIEvent& e){}

    void addChild(Widget* child){mChildren.push_back(child);}

    //
    // Setters
    //
    void setColor(RGBA& color) { mColor = color; }
    void setVisibility(bool isVisible){mVisible = isVisible;}
    void setMoveable(bool isMoveable){mMoveable = isMoveable;}
    void setText(std::wstring& text, const UINT fontSize = 20)
    {
        mTextBlock.setText(text, fontSize);
    }

    //
    // getters
    //
    virtual Box2D getClientArea();
    bool hasText()
    {
        return mTextBlock.hasText();
    }

    //
    // Graphics
    //
    virtual bool isPointInside(Point2D& point);

protected:
    // Helpers
    void forwardEvent(GUIEvent& evt)
    {
        if (evt.mPropagate)
        {
            for (UINT i = 0; i < mChildren.size(); ++i)
                mChildren[i]->dispatch(evt);
        }
    }

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

    bool mMoveable;

    Widget* mParent;
    std::vector<Widget*> mChildren;

public:
    TextBlock mTextBlock;
};


#endif
