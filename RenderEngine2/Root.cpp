#include "Root.h"

Root::Root( Area2D& vpSize )
    :Widget(NULL, vpSize, PixelPadding(), PixelMargin(), RGBA(), WIDGET_LAYOUT_NOT_SPECIFIED)
{
    mVisible = false;

}

void Root::onLayoutChanged( GUIEvent& evt )
{
}

void Root::onResize( GUIEvent& e )
{
    WidgetResizeEvent& evt = reinterpret_cast<WidgetResizeEvent&>(e);
    this->resize(evt.mSize);
    evt.mPropagate = false;
}

void Root::onLBtnDown( GUIEvent& e )
{

}

void Root::onLBtnUp( GUIEvent& e )
{

}

void Root::onMouseMove( GUIEvent& e )
{

}

void Root::onMouseWheelUp( GUIEvent& e )
{

}

void Root::onMouseWheelDown( GUIEvent& e )
{

}

void Root::onMouseEnter( GUIEvent& e )
{

}

void Root::onMouseLeave( GUIEvent& e )
{

}

void Root::getPaddedRect( Box2D& box )
{
    Point2D pos = this->mLocalRect.point[0] + Vector2D(mPadding.left, mPadding.top);

    Area2D clientSize;
    clientSize.width = mLocalRect.getWidth() - (mPadding.left+ mPadding.right);
    clientSize.height = mLocalRect.getHeight() - (mPadding.top + mPadding.bottom);

    box.resize(clientSize);
    box.moveTo(pos);
}

void Root::getMarginRect( Box2D& box )
{
    Point2D pos = this->mLocalRect.point[0] - Vector2D(mMargin.left, mMargin.top);
    Area2D size(
        mLocalRect.getWidth() + (mMargin.left + mMargin.right),
        mLocalRect.getHeight() + (mMargin.top + mMargin.bottom) );

    box.resize(size);
    box.moveTo(pos);
}

void Root::move( Vector2D& movement )
{
    // root widget not movealbe
}


void Root::resize( Area2D& newSize )
{
    // Maintain static widgets' position when size changed
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        Widget* child = mChildren[i];
        if (child->mLayoutType == WIDGET_LAYOUT_STATIC)
        {
            FPoint2D intermediatePos(
                static_cast<float>(child->mLocalRect.getLeft())/this->mLocalRect.getWidth(),
                static_cast<float>(child->mLocalRect.getTop())/this->mLocalRect.getHeight()
                );
            child->mLocalRect.moveTo( Point2D(
                static_cast<int>(newSize.width * intermediatePos.x),
                static_cast<int>(newSize.height * intermediatePos.y)
                ));
        }
    }

    mLocalRect.resize(newSize);
    solveLayout();
}

void Root::solveLayout()
{
    Box2D clientArea;
    getPaddedRect(clientArea);

    Point2D dockLeftInsertPos(clientArea.point[0]);

    UINT childNum = this->mChildren.size();
    for (UINT i = 0; i < childNum; ++i )
    {
        Widget* child = mChildren[i];
        switch(child->mLayoutType)
        {
        case WIDGET_LAYOUT_DOCK_LEFT:
            {
                Box2D paddedArea;
                child->getPaddedRect(paddedArea);
                child->moveTo(dockLeftInsertPos);

                dockLeftInsertPos.y += paddedArea.getHeight();
            }
            break;

        default:
            break;
        }
    }
}

void Root::moveTo( Point2D& pos )
{

}



