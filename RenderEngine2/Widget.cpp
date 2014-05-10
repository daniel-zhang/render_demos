#include "Widget.h"
#include "LayoutSolver.h"

Widget::Widget( std::string name, Area2D& size, RGBA& color, LayoutSolver* solver)
{
    mSize = size;
    mColor = color;
    mLayerDepth = 0;

    mState = NORMAL;
    mName = name;

    mParent = NULL;
    mLayoutType = WIDGET_LAYOUT_NOT_SPECIFIED;
    mSolver = solver;
}

Widget::Widget( std::string name, Widget* parent, Area2D& size, PixelPadding& padding, RGBA& color, LayoutSolver* solver, LayoutType layoutType /*= WIDGET_LAYOUT_STREAM*/ )
{
    mSize = size;
    mPadding = padding;
    mLayerDepth = 0;
    mColor = color;

    mState = NORMAL;
    mName = name;

    mParent = parent;
    mParent->addChild(this);
    mLayoutType = layoutType;
    mSolver = solver;
}

Widget::~Widget() {}

void Widget::onLBtnDown( Point2D& clickPos)
{
    bool isMyJob = true;
    for(UINT i = 0; i < mChildren.size(); ++i)
    {
        if (mChildren[i]->isPointInside(clickPos))
        {
            isMyJob = false;
            mChildren[i]->onLBtnDown(clickPos);
        }
    }

    if (isMyJob && isPointInside(clickPos))
    {
        mState = PRESSED_DOWN;
    }
}

void Widget::onLBtnUp( Point2D& clickPos)
{
    switch(mState)
    {
    case PRESSED_DOWN:
        mState = NORMAL;
        break;
        
    default:
        for (UINT i = 0; i < mChildren.size(); ++i)
        {
            mChildren[i]->onLBtnUp(clickPos);
        }
        break;
    }
}

void Widget::onMouseMove( Vector2D& movement )
{
    if(mState == PRESSED_DOWN)
    {
        move(movement);
    }
    else
    {
        for (UINT i = 0; i < mChildren.size(); ++i)
        {
            mChildren[i]->onMouseMove(movement);
        }
    }
}

void Widget::onResize( Area2D& newArea )
{
    resize(newArea);
}

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


