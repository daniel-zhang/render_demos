#include "Container.h"


Container::Container( D3DEnv* env, Point2D& pos, Area2D& size, const RGBA& color, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType /*= WIDGET_LAYOUT_STATIC*/ )
    :
    Widget(env, pos, size, padding, margin, color, layoutType)
{
    // Begin: Overridden by derivatives
    mType = WIDGET_TYPE_CONTAINER;
    mVisible = true;
    mState = UN_INITIALIZED;

    mClippedByParent = true;
    mLinkedToParent = true;
    mIsActive = false;
    // End: Overridden by derivatives

    mBkgSprite = 0;
}

bool Container::init()
{
    mBkgSprite = new Sprite2D();
    mBkgSprite->init(
    mEnv, 
    mLogicalBox.point[0], Area2D(mLogicalBox.getWidth(), mLogicalBox.getHeight()), 
    mColor, 
    EffectMgr::OverlayFX, EffectMgr::OverlayFX->OverlayTech);

    synClipAndClickBoxes();
    mBkgSprite->setClipBox(&mClipBox);

    if (mClippedByParent)
    {
        mBkgSprite->enableClip();
    }

    mSprites.push_back(mBkgSprite);

    mState = NORMAL;
    return true;
}

void Container::onMouseEnter( GUIEvent& e )
{
    e.mPropagate = false;
}

void Container::onMouseLeave( GUIEvent& e )
{
    e.mPropagate = false;
}


