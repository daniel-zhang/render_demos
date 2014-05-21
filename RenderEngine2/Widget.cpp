#include "Widget.h"
#include <algorithm>

Widget::Widget(D3DEnv* env, Point2D& pos, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color,  LayoutType layoutType  )
{
    mType = WIDGET_TYPE_INVALID;
    mVisible = true;
    mState = UN_INITIALIIZED;

    mEnv = env;

    mLogicalBox = Box2D(pos, size);
    mPadding = padding;
    mMargin = margin;
    mColor = color;
    mLayoutType = layoutType;
    mParent = 0;
    mActiveChild = 0;
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
            // Child receives mouse button down event in front-to-back order
            // And a child can deliberately stop the event propagation if click-through
            // behavior is not wanted.
            if (evt.mPropagate)
            {
                std::copy(mDepthQueue.begin(), mDepthQueue.end(), mDispatchQueue.begin());

                std::vector<Widget*>::reverse_iterator riter = mDispatchQueue.rbegin()  ;
                for (riter; riter != mDispatchQueue.rend(); ++riter)
                {
                    if(evt.mPropagate)
                        (*riter)->dispatch(evt);
                    else
                        break;
                }
                // Sync the dispatch queue with depthQueue
                std::copy(mDepthQueue.begin(), mDepthQueue.end(), mDispatchQueue.begin());
            }
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
    child->mParent = this;

    mChildren.push_back(child);
    mDepthQueue.push_back(child);
    mDispatchQueue.push_back(child);

    mActiveChild = child;
}

void Widget::getPaddedRect( Box2D& box )
{
    Point2D pos = mLogicalBox.point[0] + Vector2D(mPadding.left, mPadding.top);

    Area2D clientSize;
    clientSize.width = mLogicalBox.getWidth() - (mPadding.left+ mPadding.right);
    clientSize.height = mLogicalBox.getHeight() - (mPadding.top + mPadding.bottom);

    box.resize(clientSize);
    box.moveTo(pos);
}

void Widget::getMarginedArea( Area2D& area )
{
    area.width = mLogicalBox.getWidth() + (mMargin.left + mMargin.right);
    area.height = mLogicalBox.getHeight() + (mMargin.top + mMargin.bottom);
}

void Widget::move( Vector2D& movement )
{
    mLogicalBox.move(movement);
    mSprite.move(movement);

    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        mChildren[i]->move(movement);
    }
}

void Widget::moveTo( Point2D& pos )
{
    Vector2D movement = pos - mSprite.getDstBox().point[0];
    move(movement);
}

void Widget::setActive()
{
    mIsActive = true;
    if (mParent)
    {
        mParent->setChildActive(this);
    }
}

void Widget::setInActive()
{
    mIsActive = false;
}

void Widget::setChildActive( Widget* child )
{
    if (child != mActiveChild)
    {
        std::vector<Widget*>::iterator it = std::find(mDepthQueue.begin(), mDepthQueue.end(), child);
        if (it != mDepthQueue.end())
        {
            mActiveChild->setInActive();
            mActiveChild = *it;
            mDepthQueue.erase(it);
            mDepthQueue.push_back(mActiveChild);
        }
    }
}

void Widget::clear()
{
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        if (mChildren[i])
        {
            delete mChildren[i];
            mChildren[i] = 0;
        }
    }
    mChildren.clear();
    mDepthQueue.clear();
    mDispatchQueue.clear();
}











