#include "Root.h"
#include "WidgetMgr.h"

Root::Root( D3DEnv* env, Area2D& vpSize ) :
    Widget(env, Point2D(), vpSize, PixelPadding(), PixelMargin(), RGBAColor::Background, WIDGET_LAYOUT_NOT_SPECIFIED)
{
    mVisible = false;
    mType = WIDGET_TYPE_ROOT;
}

bool Root::init()
{
    mState = NORMAL;
    return true;
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

void Root::resize( Area2D& newSize )
{
    // Maintain static widgets' position when size changed
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        Widget* child = mChildren[i];
        if (child->mLayoutType == WIDGET_LAYOUT_STATIC)
        {
            FPoint2D intermediatePos(
                static_cast<float>(child->mLogicalBox.getLeft())/this->mLogicalBox.getWidth(),
                static_cast<float>(child->mLogicalBox.getTop())/this->mLogicalBox.getHeight()
                );
            child->moveTo( Point2D(
                static_cast<int>(newSize.width * intermediatePos.x),
                static_cast<int>(newSize.height * intermediatePos.y)
                ));
        }
    }

    mLogicalBox.resize(newSize);
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
            //TODO: support dock_right, dock_bottom, and dock_top
        case WIDGET_LAYOUT_DOCK_LEFT:
            {
                Area2D childMargined;
                child->getMarginedArea(childMargined);
                child->moveTo(Point2D(dockLeftInsertPos.x + child->mMargin.left, dockLeftInsertPos.y + child->mMargin.top));

                dockLeftInsertPos.y += childMargined.height;
            }
            break;
            
        case WIDGET_LAYOUT_STATIC:
        default:
            break;
        }
    }
}





