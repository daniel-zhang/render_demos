#ifndef COLOR_H
#define COLOR_H

#include "Util.h"

struct RGBA
{
    RGBA()
    {
        red = 0; green = 0; blue = 0; alpha = 255;
        _normalize(); 
    }
    RGBA(int r, int g, int b, int a = 255 ): red(r), green(g), blue(b), alpha(a) 
    {
        _normalize();
    }
    RGBA& operator=(const RGBA& rhs)
    {
        if (this == & rhs)
        {
            return *this;
        }
        this->red = rhs.red;
        this->green = rhs.green;
        this->blue = rhs.blue;
        this->alpha = rhs.alpha;
        _normalize();

        return *this; 
    }

    XMFLOAT4 normalize()
    {
        return normalized;
    }

    XMFLOAT4 normalize(float alpha)
    {
        XMFLOAT4 ret = normalized;
        ret.w = alpha;
        return ret;
    }

    XMFLOAT4 getNormalized()
    {
        return normalized;
    }
    void _normalize()
    {
        normalized.x = static_cast<float>(red)/255;
        normalized.y = static_cast<float>(green)/255;
        normalized.z = static_cast<float>(blue)/255;
        normalized.w = static_cast<float>(alpha)/255;
    }

    int red, green, blue;
    int alpha;
    XMFLOAT4 normalized;
};

#define _GLOABAL_CONST extern CONST __declspec(selectany)

namespace RGBAColor
{
    _GLOABAL_CONST RGBA Transparent(0, 0, 0, 0);
    _GLOABAL_CONST RGBA Background(55, 56, 49, 255);
    _GLOABAL_CONST RGBA Green(100, 248, 100, 255);
    _GLOABAL_CONST RGBA Red(255, 100, 100, 255);
    _GLOABAL_CONST RGBA Blue(100, 100, 255, 255);
    _GLOABAL_CONST RGBA White(230, 230, 230, 255);
    _GLOABAL_CONST RGBA Yellow(200, 200, 155, 255);
}

#endif