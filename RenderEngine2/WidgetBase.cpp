#include "WidgetBase.h"

namespace ColorSet
{
    struct RGBA
    {
        RGBA(int r, int g, int b, float a = 1.f):
            red(r), green(g), blue(b), alpha(a) {}

        XMFLOAT4 normalize()
        {
            XMFLOAT4 normalized;
            normalized.x = static_cast<float>(red)/255;
            normalized.y = static_cast<float>(green)/255;
            normalized.z = static_cast<float>(blue)/255;
            normalized.w = alpha;

            return normalized;
        }

        int red, green, blue;
        float alpha;
    };

    RGBA Background(55, 56, 49);
    RGBA Foreground(248, 248, 242);
}

WidgetBase::WidgetBase( float posX, float posY, int width, int height, Viewport& vp )
{
    mCurrState = NORMAL;
    mPrevState = NORMAL;

    mDefaultColor = ColorSet::Background.normalize();

    mViewport.width = vp.width;
    mViewport.height = vp.height;

    mPosNDC.x = posX;
    mPosNDC.y = posY;

    mWidth = width;
    mHeight = height;

    mSizeNDC.x = 2.f * static_cast<float>(mWidth)/mViewport.width; 
    mSizeNDC.y = 2.f * static_cast<float>(mHeight)/mViewport.height;
    
    // Text
    mText = L"";
    mPaddingX = 5;
    mPaddingY = 3;
    mFontSizePixel = 48.f;

    mHasFocus = false;

    updateVertices();
    setColor(mDefaultColor);
}

WidgetBase::~WidgetBase()
{

}

void WidgetBase::setLabel( std::wstring& labelText )
{

}

void WidgetBase::move( int x, int y, int lastX, int lastY )
{
    XMFLOAT2 src = screenToNDC(lastX, lastY);
    XMFLOAT2 dest = screenToNDC(x, y);

    //TODO: better use simple 2D vector math here
    for (UINT i = 0; i < 4; ++i)
    {
        mVertices[i].Pos.x += dest.x - src.x;
        mVertices[i].Pos.y += dest.y - src.y;
    } 

    // Update mPosNDC as position of text rendering will depend on this
    mPosNDC.x = mVertices[0].Pos.x;
    mPosNDC.y = mVertices[0].Pos.y;
}

void WidgetBase::onMouseEnter(int x, int y)
{
    /*
    setColor(ColorSet::Foreground.normalize());
    // shift a little bit
    for (UINT i = 0; i < 4; ++i)
    {
        mVertices[i].Pos.x += 0.01f;
        mVertices[i].Pos.y += 0.02f;
    }
    */
}

void WidgetBase::onMouseLeave(int x, int y)
{
    /*
    setColor(mDefaultColor);
    // unshift
    for (UINT i = 0; i < 4; ++i)
    {
        mVertices[i].Pos.x -= 0.01f;
        mVertices[i].Pos.y -= 0.02f;
    }
    */
}

void WidgetBase::onMouseMove( int x, int y, int lastX, int lastY )
{
    switch(mCurrState)
    {
    case PRESSED_DOWN:
        move(x, y, lastX, lastY);
        break;

    case NORMAL:
        break;

    default:
        break;
    }
}

void WidgetBase::onLBtnDown( int x, int y )
{
    mPrevState = mCurrState;
    mCurrState = PRESSED_DOWN;
    mHasFocus = true;
}

void WidgetBase::onLBtnUp( int x, int y )
{
    switch(mCurrState)
    {
    case PRESSED_DOWN:
        mPrevState = mCurrState;
        mCurrState = NORMAL;

        //mHasFocus = false;
        
        break;

    default:
        mPrevState = mCurrState;
        mCurrState = NORMAL;
        break;
    }
}

void WidgetBase::onRBtnDown( int x, int y )
{}

void WidgetBase::onRBtnUp( int x, int y )
{}

void WidgetBase::onResize(Viewport& vp)
{
    mViewport.width = vp.width;
    mViewport.height = vp.height;

    widgetResizeInNDC();
}

void WidgetBase::onWheelUp( int x, int y )
{
    move(0, -30, 0, 0);
}

void WidgetBase::onWheelDown( int x, int y )
{
    move(0, 30, 0, 0);
}

bool WidgetBase::hasFocus()
{
    return mHasFocus;
}

bool WidgetBase::isPointInside( int x, int y )
{
    XMFLOAT2 pointNDC = screenToNDC(x, y);
    if (pointNDC.x >= mPosNDC.x && pointNDC.x <= mPosNDC.x + mSizeNDC.x && 
        pointNDC.y >= mPosNDC.y - mSizeNDC.y && pointNDC.y <= mPosNDC.y)
    {
        return true;
    }
    else
        return false;
}

Vertex::OverlayVertex* WidgetBase::getVerticesInNDC()
{
    return &mVertices[0];
}

void WidgetBase::setColor( XMFLOAT4 color )
{
    for (UINT i = 0; i < 4; ++i)
    {
        mVertices[i].Color = color;
    }
}

void WidgetBase::setTextures()
{
    // TODO
}

void WidgetBase::setText( std::wstring& text )
{
    mText = text;
}
const std::wstring& WidgetBase::getText()
{
    return mText;
}

void WidgetBase::getTextArea(RectFloat& rect)
{
    // Add padding in NDC
    float paddingNdcX= 2.f * static_cast<float>(mPaddingX)/mViewport.width;
    float paddingNdcY = 2.f * static_cast<float>(mPaddingY)/mViewport.height;

    rect.left = mVertices[0].Pos.x + paddingNdcX; // left
    rect.top = mVertices[0].Pos.y - paddingNdcY; // top
    rect.right = mVertices[2].Pos.x - paddingNdcX; // right
    rect.bottom = mVertices[2].Pos.y + paddingNdcY; // bottom
}

float WidgetBase::getFontSize()
{
    return mFontSizePixel;
}

const Viewport& WidgetBase::getViewPort()
{
    return mViewport;
}

void WidgetBase::widgetResizeInNDC()
{
    mSizeNDC.x = 2.f * static_cast<float>(mWidth)/mViewport.width; 
    mSizeNDC.y = 2.f * static_cast<float>(mHeight)/mViewport.height;

    updateVertices();
}

XMFLOAT2 WidgetBase::screenToNDC( int x, int y )
{
    XMFLOAT2 ndcPoint(0.f, 0.f);
    ndcPoint.x = 2.f * (static_cast<float>(x)/mViewport.width) - 1.f;
    ndcPoint.y = 1.f - 2.f * (static_cast<float>(y)/mViewport.height);

    return ndcPoint;
}

void WidgetBase::updateVertices()
{
/*
NDC:
  v0--------v1
  |         |
  |         |
  v3--------v2
*/
    mVertices[0].Pos = XMFLOAT3(mPosNDC.x,              mPosNDC.y,              gDefaultWidgetDepth);
    mVertices[1].Pos = XMFLOAT3(mPosNDC.x + mSizeNDC.x, mPosNDC.y,              gDefaultWidgetDepth);
    mVertices[2].Pos = XMFLOAT3(mPosNDC.x + mSizeNDC.x, mPosNDC.y - mSizeNDC.y, gDefaultWidgetDepth);
    mVertices[3].Pos = XMFLOAT3(mPosNDC.x,              mPosNDC.y - mSizeNDC.y, gDefaultWidgetDepth);
}




