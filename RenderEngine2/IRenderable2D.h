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
    }

    Point2D mAbsolutePos;
    Area2D mSize;
    RGBA mColor;

    std::string mTextureName;
    Box2D mTextureQuad;

    bool mVisible;

    UINT mLayerDepth;
};

#endif