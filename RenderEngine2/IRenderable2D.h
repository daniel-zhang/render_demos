#ifndef I_RENDERABLE_H
#define I_RENDERABLE_H

#include "Math2D.h"
#include "Color.h"
#include <string>

class IRenderable2D
{
public:
    IRenderable2D()
    {
        mVisible = true;
        mHasTexture = false;
        mSortKey = -1;
    }
    bool mVisible;
    bool mHasTexture;
    UINT mLayerDepth;

    //Point2D mAbsolutePos;
    //Area2D mSize;
    Box2D mDestRect;
    FBox2D mTextureRect;
    RGBA mColor;

    std::wstring mTextureName;
    int mSortKey;
};

#endif