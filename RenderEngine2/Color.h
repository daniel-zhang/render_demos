#ifndef COLOR_H
#define COLOR_H
#include "Util.h"
struct RGBA
{
    RGBA(){red = 0; green = 0; blue = 0; alpha = 255;}
    RGBA(int r, int g, int b, int a = 255 ): red(r), green(g), blue(b), alpha(a) {}

    XMFLOAT4 normalize()
    {
        XMFLOAT4 normalized;
        normalized.x = static_cast<float>(red)/255;
        normalized.y = static_cast<float>(green)/255;
        normalized.z = static_cast<float>(blue)/255;
        normalized.w = static_cast<float>(alpha)/255;

        return normalized;
    }

    int red, green, blue;
    int alpha;
};
#endif