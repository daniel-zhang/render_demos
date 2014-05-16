#include "Widget.h"
#include "LayoutSolver.h"


Widget::Widget( Widget* parent, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType  )
{
    mLocalRect.resize(size); 
    mPadding = padding;
    mMargin = margin;
    mColor = color;
    mLayerDepth = 0;
    mLayoutType = layoutType;

    mState = NORMAL;

    mParent = parent;
    mParent->addChild(this);
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
    case GUIEvent::LayoutChanged:
        {
            this->onLayoutChanged(evt);
            if (evt.mPropagate)
                for (UINT i = 0; i < mChildren.size(); ++i)
                    mChildren[i]->dispatch(evt);
        }
        break;

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







