#include "Root.h"
#include "WidgetMgr.h"

Root::Root( D3DEnv* env, Area2D& vpSize ) :
    Widget(env, Point2D(), vpSize, PixelPadding(), PixelMargin(), RGBAColor::Background, WIDGET_LAYOUT_NOT_SPECIFIED)
{
    // Begin: Overridden by derivatives
    mType = WIDGET_TYPE_ROOT;
    mVisible = false;
    mState = UN_INITIALIZED;

    mClippedByParent = false;
    mLinkedToParent = false;
    mIsActive = false;
    // End: Overridden by derivatives
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

void Root::resize( Area2D& newSize )
{
    mLogicalBox.resize(newSize);
    synClipAndClickBoxes();
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





