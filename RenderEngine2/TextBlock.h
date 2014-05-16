#ifndef TEXT_BLOCK_H
#define TEXT_BLOCK_H

#include "IRenderable2D.h"
#include <vector>
#include "FontEngine.h"
#include "TextureMgr.h"

class TextElement: public IRenderable2D
{
public:
    TextElement(const wchar_t& c):mCharacter(c)
    {
        mHasTexture = true;
    }

    wchar_t mCharacter;
};

class TextBlock
{
public:
    TextBlock(): mFontSheet(0), mFontSize(20) {}

    bool init(FontSheet* fs)
    {
        if (!fs)
            return false;
        mFontSheet = fs;
        return true;
    }

    void setText(std::wstring& text, const UINT fontSize = 20)
    {
        mFontSize = fontSize;

        mText = text;
        for (UINT i = 0; i < mText.size(); ++i)
        {
            TextElement te(mText[i]);
            te.mHasTexture = true;
            te.mTextureName = mFontSheet->mFontSheetBmpFileName;
            TextureMgr::getTexture(te.mTextureName);
            te.mSortKey = TextureMgr::getTextureID(te.mTextureName);

            mTextQuads.push_back(te);
        }
    }

    bool hasText()
    {
        return !(mText.empty());
    }

    //
    // Solver specific
    //
    void solveLayout(Box2D& textArea, UINT layerDepth )
    {
        float resizeRatio = static_cast<float>(mFontSize) / mFontSheet->mCharHeight;
        int lineSpace = mFontSize;

        Point2D insertPos = textArea.point[0];

        for (UINT i = 0; i < mTextQuads.size(); ++i)
        {
            const wchar_t& c = mTextQuads[i].mCharacter;

            // start a new line if \n
            if (c == 10)
            {
                insertPos.x = textArea.getLeft();
                insertPos.y += lineSpace;
            }

            CD3D11_RECT srcRect = *(mFontSheet->getSrcRect(c));

            int dstRectWidth = static_cast<int>((srcRect.right - srcRect.left) * resizeRatio);
            int dstRectHeight = static_cast<int>((srcRect.bottom - srcRect.top) * resizeRatio);

            mTextQuads[i].mLayerDepth = layerDepth;
            mTextQuads[i].mAbsolutePos = insertPos;
            mTextQuads[i].mSize = Area2D(dstRectWidth, dstRectHeight);

            mTextQuads[i].mTextureRect = FBox2D(
                static_cast<float>(srcRect.left)/mFontSheet->mTextureWidth,
                static_cast<float>(srcRect.top)/mFontSheet->mTextureHeight,
                static_cast<float>(srcRect.right)/mFontSheet->mTextureWidth,
                static_cast<float>(srcRect.bottom)/mFontSheet->mTextureHeight
                );

            // Step insertPos
            insertPos.x += dstRectWidth;

            // Start a new line
            if (insertPos.x > textArea.getRight())
            {
                insertPos.x = textArea.getLeft();
                insertPos.y += lineSpace;
            }
        }
    }

    FontSheet* mFontSheet;
    std::wstring mText;
    UINT mFontSize;
    std::vector<TextElement> mTextQuads;
};

#endif