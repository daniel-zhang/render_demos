#include "TextElement.h"
#include "TextureMgr.h"
#include "FontEngine.h"

TextElement::TextElement(D3DEnv* env, wchar_t charactor, int fontSize, FontSheet* fontSheet ) :
    Widget(env, Point2D(), Area2D(), PixelPadding(), PixelMargin(0,0,2,0), RGBA(), WIDGET_LAYOUT_TEXT_STREAM_CENTERED)
{
    mType = WIDGET_TYPE_TEXTELEMENT; 
    mFontSheet = fontSheet;
    mFontSize = fontSize;
    mCharactor = charactor;
    mVisible = true;
}

bool TextElement::init()
{
    mSprite.init(*mEnv);
    mSprite.setEffect(EffectMgr::OverlayFX);
    mSprite.setTechique(EffectMgr::OverlayFX->OverlayTexTech);

    // Sprite size and texture rect are initialized here
    setChar(mCharactor);

    mState = NORMAL;
    return true;
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
    mLogicalBox.resize(newSize);
    mSprite.resize(newSize);
}

void TextElement::solveLayout()
{
    // does nothing since charactor will be the leaf widget
}

void TextElement::setColor(const RGBA& color )
{
    mSprite.setColor(color);
}

void TextElement::setChar( wchar_t charactor )
{
    mCharactor = charactor;

    CD3D11_RECT srcRect = *(mFontSheet->getSrcRect(mCharactor));
    float resizeRatio = static_cast<float>(mFontSize) / mFontSheet->mCharHeight;

    int dstRectWidth = static_cast<int>((srcRect.right - srcRect.left) * resizeRatio);
    int dstRectHeight = static_cast<int>((srcRect.bottom - srcRect.top) * resizeRatio);

    this->resize(Area2D(dstRectWidth, dstRectHeight));

    FBox2D srcBox(  static_cast<float>(srcRect.left)/mFontSheet->mTextureWidth,
                    static_cast<float>(srcRect.top)/mFontSheet->mTextureHeight,
                    static_cast<float>(srcRect.right)/mFontSheet->mTextureWidth,
                    static_cast<float>(srcRect.bottom)/mFontSheet->mTextureHeight );
    mSprite.setTexture(mFontSheet->mFontSRV, srcBox);
}

void TextElement::setFontSize( int fontSize )
{
    mFontSize = fontSize;
    setChar(mCharactor);
}

void TextElement::enableExternalTextureSrc()
{
    mSprite.enableExternalTextureManagement();
}

void TextElement::disableExternalTextureSrc()
{
    mSprite.disableExternalTextureManagement();
}


