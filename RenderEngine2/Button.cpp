#include "Button.h"

Button::Button( D3DEnv* env, Attributes& attr )
    :
    Widget(env, attr.position, attr.size, attr.padding, attr.margin, RGBA(), attr.layout_type)
{
    // Begin: Overridden by derivatives
    mType = WIDGET_TYPE_BUTTON;
    mVisible = attr.label_visible;
    mState = UN_INITIALIZED;

    mClippedByParent = attr.clipped_by_parent;
    mLinkedToParent = attr.linked_to_parent;
    mIsActive = false;
    // End: Overridden by derivatives

    mLabelBkgSprite = 0;
    mInfoBkgSprite = 0;
    mLabelTextWidget = 0;
    mInfoTextWidget = 0;

    mAttr = attr;
}


    /*
Button::Button( D3DEnv* env, Point2D& pos, Area2D& size, const RGBA& color, std::wstring& label, std::wstring& info, FontSheet* fontSheet, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType /*= WIDGET_LAYOUT_STATIC/ )
    :
    Widget(env, pos, size, padding, margin, color, layoutType)
{
    // Begin: Overridden by derivatives
    mType = WIDGET_TYPE_BUTTON;
    mVisible = true;
    mState = UN_INITIALIZED;

    mClippedByParent = true;
    mLinkedToParent = true;
    mIsActive = false;
    // End: Overridden by derivatives

    mLabelBkgSprite = 0;
    mLabelTextWidget = 0;
    mInfoTextWidget = 0;

    mFontSheet = fontSheet;
    mLabelString = label;
    mInfoString = info;
}
    */

bool Button::init()
{
    synClipAndClickBoxes();

    // Create panels for label and info
    mLabelBkgSprite = new Sprite2D();
    mLabelBkgSprite->init( mEnv, 
        mLogicalBox.point[0], mLogicalBox.getArea(), mAttr.label_background_color, 
        EffectMgr::OverlayFX, EffectMgr::OverlayFX->OverlayTech);

    mInfoBkgSprite = new Sprite2D();
    mInfoBkgSprite->init( mEnv,
        mLogicalBox.point[1], mAttr.info_frame_size, mAttr.info_background_color,
        EffectMgr::OverlayFX, EffectMgr::OverlayFX->OverlayTech);

    // This order is important for masking out infoBkgSprite
    mSprites.push_back(mLabelBkgSprite);
    mSprites.push_back(mInfoBkgSprite);

    // Create text widget for label and info 
    Text::Attributes labelAttr;
    labelAttr.position          = mLogicalBox.point[0];
    labelAttr.size              = mLogicalBox.getArea();
    labelAttr.font_size         = mAttr.label_font_size;
    labelAttr.font_sheet        = mAttr.label_font_sheet;
    labelAttr.font_color        = mAttr.label_string_color;
    labelAttr.active_font_color = mAttr.active_label_string_color;
    labelAttr.text_string       = mAttr.label_string;
    labelAttr.center_text       = mAttr.center_label_string;

    Text::Attributes infoAttr;
    infoAttr.position          = mLogicalBox.point[1];
    infoAttr.size              = mAttr.info_frame_size;
    infoAttr.font_size         = mAttr.info_font_size;
    infoAttr.font_sheet        = mAttr.info_font_sheet;
    infoAttr.font_color        = mAttr.info_string_color;
    infoAttr.active_font_color = mAttr.active_info_string_color;
    infoAttr.text_string       = mAttr.info_string;
    infoAttr.center_text       = mAttr.center_info_string;
    infoAttr.visible           = mAttr.info_visible;

    mLabelTextWidget = new Text(mEnv, labelAttr);
    mLabelTextWidget->init();
    this->addChild(mLabelTextWidget);

    mInfoTextWidget = new Text( mEnv, infoAttr);
    mInfoTextWidget->init();
    mLabelTextWidget->addChild(mInfoTextWidget);

    //////////////////////////////////////
    if (mClippedByParent)
    {
        mLabelBkgSprite->setClipBox(&mClipBox);
        mLabelBkgSprite->enableClip();
        for (UINT i = 0; i < mLabelTextWidget->mSprites.size(); ++i)
        {
            mLabelTextWidget->mSprites[i]->setClipBox(&mClipBox);
            mLabelTextWidget->mSprites[i]->enableClip();
        }
    }

    //////////////////////////////////////
    mState = NORMAL;
    return true;
}


void Button::onLBtnDown( GUIEvent& e )
{
    MouseLBtnUpEvent& evt = static_cast<MouseLBtnUpEvent&>(e);

    if (mClickBox.isPointInside(evt.mMousePos))
    {
        setActive();
        evt.mPropagate = false;
        mState = PRESSED_DOWN;
    }
}

void Button::onMouseEnter( GUIEvent& e )
{
    mInfoTextWidget->mVisible = true;
}

void Button::onMouseLeave( GUIEvent& e )
{
    mInfoTextWidget->mVisible = false;
}

UINT Button::getValidSpritesNumber()
{
    if (mInfoTextWidget->mVisible)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

Button::Attributes& Button::Attributes::operator=( const Attributes& rhs )
{
    position                      = rhs.position;
    size                          = rhs.size;
    padding                       = rhs.padding;
    margin                        = rhs.margin;
    layout_type                   = rhs.layout_type;
    clipped_by_parent             = rhs.clipped_by_parent;
    linked_to_parent              = rhs.linked_to_parent;
    label_visible                 = rhs.label_visible;
    label_string                  = rhs.label_string;
    label_font_sheet              = rhs.label_font_sheet;
    label_font_size               = rhs.label_font_size;
    center_label_string           = rhs.center_label_string;
    label_string_color            = rhs.label_string_color;
    active_label_string_color     = rhs.active_label_string_color;
    label_background_color        = rhs.label_background_color;
    active_label_background_color = rhs.active_label_background_color;
    info_visible                  = rhs.info_visible;
    info_frame_size               = rhs.info_frame_size;
    info_string                   = rhs.info_string;
    info_font_sheet               = rhs.info_font_sheet;
    info_font_size                = rhs.info_font_size;
    center_info_string            = rhs.center_info_string;
    info_string_color             = rhs.info_string_color;
    active_info_string_color      = rhs.active_info_string_color;
    info_background_color         = rhs.info_background_color;
    active_info_background_color  = rhs.active_info_background_color;
    return *this;
}

Button::Attributes::Attributes()
{
    position          = Point2D();
    size              = Area2D(150, 80);
    padding           = PixelPadding();
    margin            = PixelMargin();
    layout_type       = WIDGET_LAYOUT_STATIC;
    clipped_by_parent = true;
    linked_to_parent  = true;

    label_visible                 = true;
    label_string                  = L"Button";
    label_font_sheet              = NULL;
    label_font_size               = 24;
    center_label_string           = true;
    label_string_color            = RGBAColor::White;
    active_label_string_color     = RGBAColor::Blue;
    label_background_color        = RGBAColor::Background;
    active_label_background_color = RGBAColor::Yellow;

    info_visible                 = false;
    info_frame_size              = Area2D(200, 300);
    info_string                  = L"Info";
    info_font_sheet              = NULL;
    info_font_size               = 22;
    center_info_string           = false;
    info_string_color            = RGBAColor::White;
    active_info_string_color     = RGBAColor::Blue;
    info_background_color        = RGBAColor::Background;
    active_info_background_color = RGBAColor::Yellow;
}
