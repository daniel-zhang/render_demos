#include "Text.h"
#include "WidgetMgr.h"

Text::Text( Widget* parent, int fontSize, FontSheet* fontSheet, Area2D& size, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType )
        :Widget(parent, size, padding, margin, RGBA(), layoutType)
{
    mFontSize = fontSize;
    mFontSheet = fontSheet;

    mVisible = true;
    mHasTexture = false;
    mSortKey = -1;
}

Text::Text( Widget* parent, int fontSize, FontSheet* fontSheet, Point2D& pos, Area2D& size, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType /*= WIDGET_LAYOUT_STATIC*/ )
        :Widget(parent, size, padding, margin, RGBA(), layoutType)
{
    mFontSize = fontSize;
    mFontSheet = fontSheet;

    mLocalRect.moveTo(pos);

    mVisible = true;
    mHasTexture = false;
    mSortKey = -1;
}

void Text::updateRenderable( Point2D& parentAbsPos, Point2D& myAbsPos )
{
    myAbsPos = parentAbsPos + mLocalRect.point[0];

    mVisibleRect.resize(Area2D(mLocalRect.getWidth(), mLocalRect.getHeight()));
    mVisibleRect.moveTo(myAbsPos);
}

void Text::onResize( GUIEvent& e )
{

}

void Text::onLBtnDown( GUIEvent& e )
{
    mState = PRESSED_DOWN;
    e.mPropagate = false;
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

}

void Text::onMouseLeave( GUIEvent& e )
{

}

void Text::move( Vector2D& movement )
{
    mLocalRect.move(movement);

    mWidgetMgr->setSubmit();
}

void Text::moveTo( Point2D& pos )
{
    mLocalRect.moveTo(pos);

    mWidgetMgr->setSubmit();
}

void Text::resize( Area2D& newSize )
{
    mLocalRect.resize(newSize);
    solveLayout();

    mWidgetMgr->setSubmit();
}

void Text::solveLayout()
{
    Box2D textArea;
    getPaddedRectLocal(textArea);

    Point2D insertPos(textArea.point[0]);
    int lineSpace = mFontSize;

    for (UINT i = 0; i < mChildren.size(); ++i)
    {
        // TODO: Should use dynamic_cast...
        TextElement* child = static_cast<TextElement*>(mChildren[i]);
        switch(child->mLayoutType)
        {
        case WIDGET_LAYOUT_TEXT_STREAM:
            {
                const wchar_t& c = child->getChar();
                Box2D childMarginedBox;
                child->getMarginRect(childMarginedBox);

                // ASCII code 10: '\n'
                if (c == 10 || insertPos.x + childMarginedBox.getWidth() > textArea.getRight())
                {
                    insertPos.x = textArea.getLeft();
                    insertPos.y += lineSpace;
                }

                child->moveTo(Point2D(insertPos.x + child->mMargin.left, insertPos.y + child->mMargin.top));
                child->mLayerDepth = mLayerDepth + 1;

                insertPos.x += childMarginedBox.getWidth();
            }
            break;

        default:
            break;
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
