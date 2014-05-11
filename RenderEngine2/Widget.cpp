#include "Widget.h"
#include "LayoutSolver.h"

Widget::Widget( std::string name, Area2D& size, const RGBA& color, LayoutSolver* solver)
{
    mSize = size;
    mColor = color;
    mLayerDepth = 0;

    mState = NORMAL;
    mName = name;

    mMoveable = false;

    mParent = NULL;
    mLayoutType = WIDGET_LAYOUT_NOT_SPECIFIED;
    mSolver = solver;
}

Widget::Widget( std::string name, Widget* parent, Area2D& size, PixelPadding& padding, const RGBA& color, LayoutSolver* solver, LayoutType layoutType /*= WIDGET_LAYOUT_STREAM*/ )
{
    mSize = size;
    mPadding = padding;
    mLayerDepth = 0;
    mColor = color;

    mState = NORMAL;
    mName = name;

    mMoveable = true;

    mParent = parent;
    mParent->addChild(this);
    mLayoutType = layoutType;
    mSolver = solver;
}

Widget::~Widget() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void Widget::dispatch( GUIEvent& evt )
{
    switch(evt.mEventType)
    {
    case GUIEvent::WidgetResize:
        {
            this->onResize(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    case GUIEvent::MouseLBtnDown:
        {
            this->onLBtnDown(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    case GUIEvent::MouseLBtnUp:
        {
            this->onLBtnUp(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    case GUIEvent::MouseMove:
        {
            this->onMouseMove(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    case GUIEvent::MouseEnter:
        {
            this->onMouseEnter(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    case GUIEvent::MouseLeave:
        {
            this->onMouseLeave(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    case GUIEvent::MouseWheelUp:
        {
            this->onMouseWheelUp(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);

        }
        break;

    case GUIEvent::MouseWheelDown:
        {
            this->onMouseWheelDown(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void Widget::onResize( GUIEvent& e )
{
    WidgetResizeEvent& evt = reinterpret_cast<WidgetResizeEvent&>(e);
    resize(evt.mSize);

    evt.mPropagate = false;
}

void Widget::onLBtnDown( GUIEvent& e )
{
    MouseLBtnDownEvent& evt = reinterpret_cast<MouseLBtnDownEvent&>(e);

    if (isPointInside(evt.mMousePos))
    {
        bool handledByChild = false;
        for (UINT i = 0; i < mChildren.size(); ++i)
        {
            if (mChildren[i]->isPointInside(evt.mMousePos))
            {
                handledByChild = true;
            }
        }
        if (handledByChild == false)
        {
            mState = PRESSED_DOWN;
            evt.mPropagate = false;
        }
    }
}

void Widget::onLBtnUp( GUIEvent& e )
{
    MouseLBtnUpEvent& evt = reinterpret_cast<MouseLBtnUpEvent&>(e);
    if (mState == PRESSED_DOWN)
    {
        mState = NORMAL;
        evt.mPropagate = false;
    }
}

void Widget::onMouseMove( GUIEvent& e )
{
    MouseMoveEvent& evt = reinterpret_cast<MouseMoveEvent&>(e);
    if (mState == PRESSED_DOWN && mMoveable)
    {
        move(static_cast<Vector2D>(evt.mMousePos - evt.mLastMousePos));
        evt.mPropagate = false;
    }
}

void Widget::onMouseWheelUp( GUIEvent& e )
{
    MouseWheelUpEvent& evt = reinterpret_cast<MouseWheelUpEvent&>(e);
    if (isPointInside(evt.mMousePos) && mMoveable)
    {
        move(Vector2D(0, 30));
        //evt.mPropagate = false;
    }
}

void Widget::onMouseWheelDown( GUIEvent& e )
{
    MouseWheelDownEvent& evt = reinterpret_cast<MouseWheelDownEvent&>(e);
    if (isPointInside(evt.mMousePos) && mMoveable)
    {
        move(Vector2D(0, -30));
        //evt.mPropagate = false;
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void Widget::getRenderInfo( Box2D& box, int& layoutDepth, RGBA& color )
{
    box = Box2D(mAbsolutePos, mSize);
    layoutDepth = mLayerDepth;
    color = mColor;
}

bool Widget::isPointInside( Point2D& point )
{
    return (Box2D(mAbsolutePos, mSize).isPointInside(point));
}

void Widget::resize( Area2D& newArea )
{
    mSize = newArea;

    mSolver->solve(this);
}

void Widget::move( Vector2D& movement )
{
    mPos += movement;
    mAbsolutePos += movement;

    mSolver->solve(this);
}

// Compute client area based on widget size and padding
// Note: the returned box is relative to widget/local space.
Box2D Widget::getClientArea()
{
    Point2D clientPos = Vector2D(mPadding.left, mPadding.top);

    Area2D clientSize;
    clientSize.width = mSize.width - (mPadding.left+ mPadding.right);
    clientSize.height = mSize.height - (mPadding.top + mPadding.bottom);

    return Box2D(clientPos, clientSize);
}



