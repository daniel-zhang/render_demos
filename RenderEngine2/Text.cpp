#include "Text.h"
#include "WidgetMgr.h"

Text::Text( Widget* parent, int fontSize, FontSheet* fontSheet, Area2D& size, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType )
        :Widget(parent, size, padding, margin, RGBA(), layoutType)
{
    mFontSize = fontSize;
    mFontSheet = fontSheet;

    mBgColor = RGBAColor::Background;
    mFontColor = RGBAColor::White;
    mActiveBgColor = RGBAColor::Background;
    mActiveFontColor = RGBAColor::Blue;

    mVisible = true;
    mHasTexture = false;
    mSortKey = -1;
}

Text::Text( Widget* parent, int fontSize, FontSheet* fontSheet, Point2D& pos, Area2D& size, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType /*= WIDGET_LAYOUT_STATIC*/ )
        :Widget(parent, size, padding, margin, RGBA(), layoutType)
{
    mFontSize = fontSize;
    mFontSheet = fontSheet;

    mBgColor = RGBAColor::Background;
    mFontColor = RGBAColor::White;
    mActiveBgColor = RGBAColor::Background;
    mActiveFontColor = RGBAColor::Yellow;

    mRect.moveTo(pos);

    mVisible = true;
    mHasTexture = false;
    mSortKey = -1;
}

void Text::updateRenderable( )
{
    mVisibleRect = mRect;
}

void Text::onResize( GUIEvent& e )
{

}

void Text::onLBtnDown( GUIEvent& e )
{
    MouseLBtnDownEvent& evt = reinterpret_cast<MouseLBtnDownEvent&>(e);
    
    if (mRect.isPointInside(evt.mMousePos))
    {
        mState = PRESSED_DOWN;
        e.mPropagate = false;
    }
}

void Text::onLBtnUp( GUIEvent& e )
{
    if (mState == PRESSED_DOWN)
    {
        mState = NORMAL;
        e.mPropagate = false;
    }
}

void Text::onMouseMove( GUIEvent& e )
{
    MouseMoveEvent& evt = reinterpret_cast<MouseMoveEvent&>(e);

    // TODO: emit mouse enter/ mouse leave event here...
    if (mRect.isPointInside(evt.mLastMousePos) == false && mRect.isPointInside(evt.mMousePos)== true)
    {
        dispatch(MouseEnterEvent());
    }
    else if (mRect.isPointInside(evt.mLastMousePos) == true && mRect.isPointInside(evt.mMousePos)== false)
    {
        dispatch(MouseLeaveEvent());
    }

    if (mState == PRESSED_DOWN )
    {
        move(static_cast<Vector2D>(evt.mMousePos - evt.mLastMousePos));
        evt.mPropagate = false;
    }
}

void Text::onMouseWheelUp( GUIEvent& e )
{

}

void Text::onMouseWheelDown( GUIEvent& e )
{

}

void Text::onMouseEnter( GUIEvent& e )
{
    MouseEnterEvent& evt = reinterpret_cast<MouseEnterEvent&>(e);

    setFontColor(mActiveFontColor);
    setBgColor(mActiveBgColor);
    mWidgetMgr->setSubmit();

    evt.mPropagate = false;
}

void Text::onMouseLeave( GUIEvent& e )
{
    MouseLeaveEvent& evt = reinterpret_cast<MouseLeaveEvent&>(e);

    setFontColor(mFontColor);
    setBgColor(mBgColor);
    mWidgetMgr->setSubmit();

    evt.mPropagate = false;
}

void Text::resize( Area2D& newSize )
{
    mRect.resize(newSize);
    solveLayout();

    mWidgetMgr->setSubmit();
}

void Text::solveLayout()
{
    Box2D textArea;
    getPaddedRect(textArea);

    Point2D insertPos(textArea.point[0]);
    Point2D insertPos_Centered(textArea.point[0]);
    int lineSpace = mFontSize;

    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        TextElement* child = static_cast<TextElement*>(mChildren[i]);
        switch(child->mLayoutType)
        {
        case WIDGET_LAYOUT_TEXT_STREAM:
            {
                const wchar_t& c = child->getChar();
                Area2D marginedArea;
                child->getMarginedArea(marginedArea);

                // ASCII code 10: '\n'
                if (c == 10 || insertPos.x + marginedArea.width > textArea.getRight())
                {
                    insertPos.x = textArea.getLeft();
                    insertPos.y += lineSpace;
                }

                child->moveTo(Point2D(insertPos.x + child->mMargin.left, insertPos.y + child->mMargin.top));
                child->mLayerDepth = mLayerDepth + 1;

                insertPos.x += marginedArea.width;
            }
            break;

        case WIDGET_LAYOUT_TEXT_STREAM_CENTERED:
            {
                const wchar_t& c = child->getChar();
                Area2D marginedArea;
                child->getMarginedArea(marginedArea);

                // ASCII code 10: '\n'
                if (c == 10 || insertPos_Centered.x + marginedArea.width > textArea.getRight())
                {
                    insertPos_Centered.x = textArea.getLeft();
                    insertPos_Centered.y += lineSpace;
                }

                child->moveTo(Point2D(insertPos_Centered.x + child->mMargin.left, insertPos_Centered.y + child->mMargin.top));
                child->mLayerDepth = mLayerDepth + 1;

                insertPos_Centered.x += marginedArea.width;
            }
            break;

        default:
            break;
        }
    }// end of for loop

    // Center the texts if any
    if (insertPos_Centered != textArea.point[0])
    {
        Area2D filledArea;
        // If there's only 1 line
        if (insertPos_Centered.y == textArea.getTop())
        {
            filledArea = Area2D(insertPos_Centered.x - textArea.getLeft(), lineSpace);
        }
        // Multiple lines
        else
        {
            filledArea = Area2D(textArea.getWidth(), insertPos_Centered.y + lineSpace - textArea.getTop());
        }

        int x = (textArea.getWidth() - filledArea.width) / 2;
        int y = (textArea.getHeight() - filledArea.height) / 2;
        Vector2D centered(x, y);
        for (UINT i = 0; i < mChildren.size(); ++i)
        {
            if (mChildren[i]->mLayoutType == WIDGET_LAYOUT_TEXT_STREAM_CENTERED)
            {
                mChildren[i]->move(centered);
            }
        }
    }
}

void Text::setText( std::wstring& s )
{
    clearText();
    mString = s;

    for (UINT i = 0; i < mString.size(); ++i)
    {
        TextElement* te = new TextElement(this, mFontSize, mString[i], mFontSheet);
    }
    solveLayout();

    setBgColor(mBgColor);
    setFontColor(mFontColor);
}

void Text::clearText()
{
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        if (mChildren[i])
        {
            delete mChildren[i];
            mChildren[i] = 0;
        }
    }
    mChildren.clear();
    mString.clear();
}

void Text::setFontSize( int newSize )
{

}
void Text::setFontColor(RGBA& color)
{
    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        static_cast<TextElement*>(mChildren[i])->setColor(color);
    }
}

void Text::setBgColor(RGBA& color)
{
   mColor = color; 
}

