#ifndef I_RENDERABLE_H
#define I_RENDERABLE_H

#include "Math2D.h"
#include "Color.h"

class IRenderable2D
{
public:
    IRenderable2D()
    {
        mVisible = true;
    }

    void setColor(RGBA& color)
    {
        mColor = color;
    }

    Point2D mAbsolutePos;
    Area2D mSize;
    RGBA mColor;

    bool mVisible;

    UINT mLayerDepth;
};

#endif