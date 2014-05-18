#include "Widget.h"
#include "LayoutSolver.h"
#include "WidgetMgr.h"

Widget::Widget( Widget* parent, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType  )
{
    mRect.resize(size); 
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
    Point2D pos = this->mRect.point[0] + Vector2D(mPadding.left, mPadding.top);

    Area2D clientSize;
    clientSize.width = mRect.getWidth() - (mPadding.left+ mPadding.right);
    clientSize.height = mRect.getHeight() - (mPadding.top + mPadding.bottom);

    box.resize(clientSize);
    box.moveTo(pos);
}

void Widget::getMarginedArea( Area2D& area )
{
    area.width = mRect.getWidth() + (mMargin.left + mMargin.right);
    area.height = mRect.getHeight() + (mMargin.top + mMargin.bottom);
}

void Widget::updateRenderable()
{
    mVisibleRect = mRect;
}

void Widget::move( Vector2D& movement )
{
    mRect.move(movement);
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        mChildren[i]->move(movement);
    }

    mWidgetMgr->setSubmit();
}

void Widget::moveTo( Point2D& pos )
{
    Vector2D movement = pos - mRect.point[0];
    move(movement);

    mWidgetMgr->setSubmit();
}










