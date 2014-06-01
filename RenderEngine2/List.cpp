#include "List.h"

List::Attributes::Attributes()
{
    visible                 = true;
    position                = Point2D();
    size                    = Area2D(200, 400);
    padding                 = PixelPadding();
    margin                  = PixelMargin();
    layout_type             = WIDGET_LAYOUT_STATIC;
    clipped_by_parent       = true;
    linked_to_parent        = true;
    background_color        = RGBAColor::Background;
    active_background_color = RGBAColor::Blue;
    font_size               = 22;
    font_sheet              = NULL;
}

List::Attributes& List::Attributes::operator=( Attributes& rhs )
{
    this->visible                 = rhs.visible;
    this->position                = rhs.position;
    this->size                    = rhs.size;
    this->padding                 = rhs.padding;
    this->margin                  = rhs.margin;
    this->clipped_by_parent       = rhs.clipped_by_parent;
    this->linked_to_parent        = rhs.linked_to_parent;
    this->background_color        = rhs.background_color;
    this->active_background_color = rhs.active_background_color;
    this->font_size               = rhs.font_size;
    this->font_sheet              = rhs.font_sheet;

    for (UINT i = 0; i < rhs.item_desc.size(); ++i)
    {
        ItemDesc desc;
        desc.label = rhs.item_desc[i].label;
        desc.info = rhs.item_desc[i].info;
        this->item_desc.push_back(desc);
    }
    return *this;
}

List::List( D3DEnv* env, Attributes& attr )
    :
    Widget(env, attr.position, attr.size, attr.padding, attr.margin, attr.background_color, attr.layout_type)
{
    // Begin: Overridden by derivatives
    mType = WIDGET_TYPE_BUTTON;
    mVisible = attr.visible;
    mState = UN_INITIALIZED;

    mClippedByParent = attr.clipped_by_parent;
    mLinkedToParent = attr.linked_to_parent;
    mIsActive = false;
    // End: Overridden by derivatives

    mAttr = attr;

    mBkgSprite = 0;
}

bool List::init()
{
    mBkgSprite = new Sprite2D();
    mBkgSprite->init(
        mEnv, 
        mLogicalBox.point[0], Area2D(mLogicalBox.getWidth(), mLogicalBox.getHeight()), 
        mAttr.background_color, 
        EffectMgr::OverlayFX, EffectMgr::OverlayFX->OverlayTech);

    synClipAndClickBoxes();
    mBkgSprite->setClipBox(&mClipBox);

    if (mClippedByParent)
    {
        mBkgSprite->enableClip();
    }

    mSprites.push_back(mBkgSprite);

    for (UINT i = 0; i < mAttr.item_desc.size(); ++i)
    {
        Button::Attributes buttonAttr;
        buttonAttr.margin = PixelMargin(0, 0, 0, 2);
        buttonAttr.position = Point2D(0, 0);
        buttonAttr.size = Area2D(mAttr.size.width, mAttr.font_size + 5);
        buttonAttr.label_font_sheet = mAttr.font_sheet;
        buttonAttr.label_font_size = mAttr.font_size;
        buttonAttr.label_string =  mAttr.item_desc[i].label;
        buttonAttr.info_font_sheet = mAttr.font_sheet;
        buttonAttr.info_string = mAttr.item_desc[i].info;
        buttonAttr.info_frame_size = Area2D(mAttr.size.width, 0);
        buttonAttr.info_font_size = mAttr.font_size;

        Button* b = new Button(mEnv, buttonAttr);
        b->init();
        this->addChild(b);
    }
    solveLayout();

    mState = NORMAL;
    return true;
}

void List::onMouseWheelUp( GUIEvent& e )
{
    MouseWheelUpEvent& evt = static_cast<MouseWheelUpEvent&>(e);

    if (mClickBox.isPointInside(evt.mMousePos))
    {
        evt.mPropagate = false;
        for (UINT i = 0; i < mChildren.size(); ++i)
        {
            mChildren[i]->move(Vector2D(0, 15));
            if (mChildren[i]->mClickBox.isPointInside(evt.mMousePos))
                mChildren[i]->dispatch(MouseEnterEvent());
            else
                mChildren[i]->dispatch(MouseLeaveEvent());
        }
    }
}

void List::onMouseWheelDown( GUIEvent& e )
{
    MouseWheelDownEvent& evt = static_cast<MouseWheelDownEvent&>(e);

    if (mClickBox.isPointInside(evt.mMousePos))
    {
        evt.mPropagate = false;
        for (UINT i = 0; i < mChildren.size(); ++i)
        {
            mChildren[i]->move(Vector2D(0, -15));
            if (mChildren[i]->mClickBox.isPointInside(evt.mMousePos))
                mChildren[i]->dispatch(MouseEnterEvent());
            else
                mChildren[i]->dispatch(MouseLeaveEvent());
        }
    }
}

void List::onMouseEnter( GUIEvent& e )
{
    e.mPropagate = false;
}

void List::onMouseLeave( GUIEvent& e )
{
    e.mPropagate = false;
}

void List::solveLayout()
{
    UINT childNum = mChildren.size();
    if(childNum < 1)
        return;

    // Vertical layout
    Point2D insertPos = mLogicalBox.point[0] + Vector2D(mChildren[0]->mMargin.left, mChildren[0]->mMargin.top);
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        mChildren[i]->moveTo(insertPos);
        Area2D childMarginedArea;
        mChildren[i]->getMarginedArea(childMarginedArea);
        insertPos.y += childMarginedArea.height;
    }
}

