#include "Widget.h"
#include "LayoutSolver.h"
#include "WidgetMgr.h"

Widget::Widget( Widget* parent, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType  )
{
    mLocalRect.resize(size); 
    mPadding = padding;
    mMargin = margin;
    mColor = color;
    mLayerDepth = 0;
    mLayoutType = layoutType;

    mState = NORMAL;

    // Children gets ref to widgetMgr via their parent
    // Root's widgetMgr ref is set externally by widgetMgr
    if(parent)
    {
        mParent = parent;
        mParent->addChild(this);
        mWidgetMgr = mParent->getWidgetMgr();
    }
    else
    {
        mWidgetMgr = 0;
    }
}


Widget::~Widget() 
{
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        delete mChildren[i];
    }
}

void Widget::dispatch( GUIEvent& evt )
{
    switch(evt.mEventType)
    {
        /*
    case GUIEvent::LayoutChanged:
        {
            this->onLayoutChanged(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;
        */

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

void Widget::addChild( Widget* child )
{
    mChildren.push_back(child);
}

WidgetMgr* Widget::getWidgetMgr()
{
    return mWidgetMgr;
}

void Widget::setWidgetMgr( WidgetMgr* w )
{
    mWidgetMgr = w;
}

void Widget::getPaddedRect( Box2D& box )
{
    Point2D pos = this->mLocalRect.point[0] + Vector2D(mPadding.left, mPadding.top);

    Area2D clientSize;
    clientSize.width = mLocalRect.getWidth() - (mPadding.left+ mPadding.right);
    clientSize.height = mLocalRect.getHeight() - (mPadding.top + mPadding.bottom);

    box.resize(clientSize);
    box.moveTo(pos);
}
void Widget::getPaddedRectLocal( Box2D& box )
{
    Point2D pos = Point2D(mPadding.left, mPadding.top);

    Area2D clientSize;
    clientSize.width = mLocalRect.getWidth() - (mPadding.left+ mPadding.right);
    clientSize.height = mLocalRect.getHeight() - (mPadding.top + mPadding.bottom);

    box.resize(clientSize);
    box.moveTo(pos);
}

void Widget::getMarginRect( Box2D& box )
{
    Point2D pos = this->mLocalRect.point[0] - Vector2D(mMargin.left, mMargin.top);
    Area2D size(
        mLocalRect.getWidth() + (mMargin.left + mMargin.right),
        mLocalRect.getHeight() + (mMargin.top + mMargin.bottom) );

    box.resize(size);
    box.moveTo(pos);
}








