#include "TextElement.h"
#include "TextureMgr.h"
#include "WidgetMgr.h"

TextElement::TextElement( Widget* parent, int fontSize, wchar_t charactor, FontSheet* fontSheet )
    :Widget(parent, Area2D(), PixelPadding(), PixelMargin(0,0,2,0), RGBA(), WIDGET_LAYOUT_TEXT_STREAM_CENTERED)

{
    mFontSheet = fontSheet;
    mFontSize = fontSize;
    mCharacter = charactor;
    
    mVisible = true;
    mHasTexture = true;
    mTextureName = mFontSheet->mFontSheetBmpFileName;
    TextureMgr::getTexture(mTextureName);
    mSortKey = TextureMgr::getTextureID(mTextureName);

    // Sprite size and texture rect can be initialized here
    CD3D11_RECT srcRect = *(mFontSheet->getSrcRect(mCharacter));
    float resizeRatio = static_cast<float>(mFontSize) / mFontSheet->mCharHeight;

    int dstRectWidth = static_cast<int>((srcRect.right - srcRect.left) * resizeRatio);
    int dstRectHeight = static_cast<int>((srcRect.bottom - srcRect.top) * resizeRatio);
    mRect.resize( Area2D(dstRectWidth, dstRectHeight) );

    mTextureRect = FBox2D(
        static_cast<float>(srcRect.left)/mFontSheet->mTextureWidth,
        static_cast<float>(srcRect.top)/mFontSheet->mTextureHeight,
        static_cast<float>(srcRect.right)/mFontSheet->mTextureWidth,
        static_cast<float>(srcRect.bottom)/mFontSheet->mTextureHeight
        );
}

void TextElement::onResize( GUIEvent& e )
{

}

void TextElement::onLBtnDown( GUIEvent& e )
{

}

void TextElement::onLBtnUp( GUIEvent& e )
{

}

void TextElement::onMouseMove( GUIEvent& e )
{

}

void TextElement::onMouseWheelUp( GUIEvent& e )
{

}

void TextElement::onMouseWheelDown( GUIEvent& e )
{

}

void TextElement::onMouseEnter( GUIEvent& e )
{

}

void TextElement::onMouseLeave( GUIEvent& e )
{

}

void TextElement::resize( Area2D& newSize )
{

}

void TextElement::solveLayout()
{
    // does nothing since charactor will be the leaf widget
}

void TextElement::setColor( RGBA& color )
{
    mColor = color;
}

