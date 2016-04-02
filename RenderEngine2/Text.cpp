#include "Text.h"
#include "FontEngine.h"
#include "TextureMgr.h"

Text::Text( D3DEnv* env, Attributes& attr )
    :
    mSRV(NULL),
    Widget(env, attr.position, attr.size, attr.padding, attr.margin, RGBA(), attr.layout_type)
{
    // Begin: Overridden by derivatives
    mType            = WIDGET_TYPE_TEXT;
    mState           = UN_INITIALIZED;
    mIsActive        = false;
    mVisible         = attr.visible;
    mClippedByParent = attr.clipped_by_parent;
    mLinkedToParent  = attr.linked_to_parent;
    // End: Overridden by derivatives
    mString          = attr.text_string;
    mCenterText      = attr.center_text;
    mFontName        = attr.font_name;
    mFontSize        = attr.font_size;
    mFontSheet       = attr.font_sheet;
    mFontColor       = attr.font_color;
    mActiveFontColor = attr.active_font_color;

    //Singleton<FontMgr>::getInstance().setActiveFont(attr.font_name, attr.font_size);
    Singleton<FontMgr>::getInstance().setActiveFont(attr.font_name, 20);
    mSRV = Singleton<FontMgr>::getInstance().getSRV();

    if (mString.size() > 0)
    {
        generateTextSprites();
    }
}

bool Text::init()
{
    mState = NORMAL;
    return true;
}

void Text::setText( std::wstring& s )
{
    if (s.compare(mString) == 0)
        return;

    mString = s;
    generateTextSprites();
}

void Text::generateTextSprites()
{
    if (mSprites.size() < mString.size())
    {
        mSprites.resize(mString.size(), NULL);
    }

    for (UINT i = 0; i < mString.size(); ++i)
    {
        Area2D charPixelSize; 
        FBox2D charSrcRect;
        getCharactorRects(mString[i], charPixelSize, charSrcRect);
        // Create once and re-use, rather than create & destroy each frame
        if (mSprites[i] == NULL)
        {
            mSprites[i] = new Sprite2D();
            mSprites[i]->init(
                mEnv, Point2D(), Area2D(), mFontColor, 
                EffectMgr::OverlayFX, EffectMgr::OverlayFX->OverlayTexTech, 
                true // Sprite with dynamic VB to enable texcoord update on the fly.
                );
            mSprites[i]->enableExternalTextureManagement();
        }
        mSprites[i]->dynamicSync(Point2D(), charPixelSize, charSrcRect, mFontSheet->mFontSRV);
    }
    layoutText(mCenterText);
}

void Text::setTextSize( int newSize )
{
    // TODO
}

void Text::setTextColor(RGBA& color)
{
    for (UINT i = 0; i < mSprites.size(); ++i)
    {
        mSprites[i]->setColor(color);
    }
}

void Text::onMouseEnter( GUIEvent& e )
{
    MouseEnterEvent& evt = reinterpret_cast<MouseEnterEvent&>(e);

    setTextColor(mActiveFontColor);

    evt.mPropagate = false;
}

void Text::onMouseLeave( GUIEvent& e )
{
    MouseLeaveEvent& evt = reinterpret_cast<MouseLeaveEvent&>(e);

    setTextColor(mFontColor);

    evt.mPropagate = false;
}

void Text::resize( Area2D& newSize )
{
    mLogicalBox.resize(newSize);
    mClickBox.resize(newSize);
    //layoutText(mCenterText);
}

struct WordElement
{
    WordElement()
    {
        clear();
    }
    void foreSeek(std::vector<Sprite2D*>& sprites, std::wstring& str, UINT pos)
    {
        clear();
        if (str.size() == 0) return;

        start = pos;
        end = start;
        while(true)
        {
            if (end == str.size())
            {
                return;
            }
            width += sprites[end]->getCacheBox()->getWidth();

            wchar_t c = str[end];
            // if c is not '0-9a-zA-Z'
            if (   (c < L'0' || c > L'9') && (c < L'A' || c > L'Z') && (c < L'a' || c > L'z')  )
            {
                end++;
                return;
            }
            end++;
        }
    }

    void clear()
    {
        width = 0;
        start = end = 0;
    }

    int width;
    UINT start, end;
};

void Text::layoutText(bool centered)
{
    Box2D textArea;
    getPaddedRect(textArea);

    Point2D insertPos(textArea.point[0]);
    int lineHeight = mFontSize;
    int spaceWidth = mFontSize;
    int charGap = 1;

    WordElement word;
    UINT i = 0;
    while(i < mString.size())
    {
        if (mString[i] == L'\n')
        {
            insertPos.x = textArea.getLeft();
            insertPos.y += lineHeight;
            mSprites[i]->moveTo(insertPos);
            i++; continue;
        }

        // Fore-seek the word
        word.foreSeek(mSprites, mString, i);
        // Start a newline if soft wrap is needed
        if (insertPos.x + word.width > textArea.getRight())
        {
            insertPos.x = textArea.getLeft();
            insertPos.y += lineHeight;
        }
        // Layout the word
        for (UINT j = word.start; j < word.end; ++j)
        {
            mSprites[j]->moveTo(Point2D(insertPos.x, insertPos.y));
            insertPos.x += mSprites[j]->getCacheBox()->getWidth() + charGap;
        }
        i += word.end - word.start;
    }

    Area2D filledArea;
    if (insertPos.y == textArea.getTop())
    {
        filledArea.width = insertPos.x - textArea.getLeft();
        filledArea.height = lineHeight;
    }
    else
    {
        filledArea.width = textArea.getWidth();
        filledArea.height = insertPos.y + lineHeight - textArea.getTop();
    }

    // Calculate height if it is not specified by user.
    if(mLogicalBox.getHeight() == 0)
    {
        mLogicalBox.resize(Area2D(textArea.getWidth(), filledArea.height));
        mClickBox = mLogicalBox;
    }

    if(centered)
    {
        int x = (textArea.getWidth() - filledArea.width) / 2;
        int y = (textArea.getHeight() - filledArea.height) / 2;
        Vector2D centered(x, y);
        for (UINT i = 0; i < mString.size(); ++i)
            mSprites[i]->move(centered);
    }
}

void Text::layoutText_bak(bool centered)
{
    Box2D textArea;
    getPaddedRect(textArea);

    Point2D insertPos(textArea.point[0]);

    //int lineSpace = mFontSheet->mCharHeight;
    int lineSpace = mFontSize;
    int charSpace = 1;

    for (UINT i = 0; i < mString.size(); ++i)
    {
        wchar_t c = mString[i];
        Sprite2D* sprite = mSprites[i];

        // ASCII code 10: '\n'
        if (c  == 10 || insertPos.x + sprite->getCacheBox()->getWidth() > textArea.getRight())
        {
            insertPos.x = textArea.getLeft();
            insertPos.y += lineSpace;
        }
        sprite->moveTo(Point2D(insertPos.x, insertPos.y));
        insertPos.x += sprite->getCacheBox()->getWidth() + charSpace;
    }

    if(centered)
    {
        Area2D filledArea;
        if (insertPos.y == textArea.getTop())
        {
            filledArea.width = insertPos.x - textArea.getLeft();
            filledArea.height = lineSpace;
        }
        else
        {
            filledArea.width = textArea.getWidth();
            filledArea.height = insertPos.y + lineSpace - textArea.getTop();
        }

        int x = (textArea.getWidth() - filledArea.width) / 2;
        int y = (textArea.getHeight() - filledArea.height) / 2;
        Vector2D centered(x, y);
        for (UINT i = 0; i < mString.size(); ++i)
            mSprites[i]->move(centered);
    }
}

void Text::beforeDrawSelf()
{
    EffectMgr::OverlayFX->setDiffuseMap(mSRV);
}

void Text::afterDrawSelf() {}

void Text::getCharactorRects( wchar_t c, Area2D& dstSize, FBox2D& srcRect )
{
    float resizeRatio = static_cast<float>(mFontSize) / mFontSheet->mCharHeight;

    //Box2D* texRect = (mFontSheet->getSrcRect(c));
    Box2D* texRect = Singleton<FontMgr>::getInstance().getSrcRect(c);
    if (texRect)
    {
        // Simple rounding
        dstSize.width = static_cast<int>((texRect->getRight()- texRect->getLeft()) * resizeRatio + 0.5f);
        dstSize.height = static_cast<int>((texRect->getBottom()- texRect->getTop()) * resizeRatio + 0.5f);

        srcRect.build(
        static_cast<float>(texRect->getLeft())/mFontSheet->mTextureWidth,
        static_cast<float>(texRect->getTop())/mFontSheet->mTextureHeight,
        static_cast<float>(texRect->getRight())/mFontSheet->mTextureWidth,
        static_cast<float>(texRect->getBottom())/mFontSheet->mTextureHeight );
    }
    // Construct null size/rect data for non-printable chars
    else
    {
        dstSize = Area2D();
        srcRect = FBox2D();
    }
}

UINT Text::getValidSpritesNumber()
{
    return mString.size();
}

