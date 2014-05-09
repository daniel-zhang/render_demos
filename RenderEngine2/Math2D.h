#ifndef MATH_2D_H 
#define MATH_2D_H

#include "Util.h"

struct Point2D
{
    Point2D();
    Point2D(int _x, int _y);
    Point2D(const Point2D& other);
    Point2D& operator=(const Point2D& rhs);
    Point2D& operator+=(const Point2D& rhs);
    Point2D& operator-=(const Point2D& rhs);
    const Point2D operator+(const Point2D& other) const;
    const Point2D operator-(const Point2D& other) const;

    int x;
    int y;
};

typedef Point2D Vector2D;

struct Area2D
{
    Area2D();
    Area2D(int w, int h);
    Area2D(const Area2D& pa);

    int width;
    int height; 
};

struct Box2D 
{
    Box2D();
    Box2D(Point2D& pos, Area2D& size);
    Box2D(int left, int top, int right, int bottom);

    Box2D(const Box2D& box);
    Box2D& operator=(const Box2D& box);

    Point2D getCenter();
    int getWidth();
    int getHeight();
    int getLeft();
    int getRight();
    int getTop();
    int getBottom();
    bool isPointInside(Point2D& pos);

    Point2D point[4];
};

class PixelPadding
{
public:
    PixelPadding():left(0), top(0), right(0), bottom(0){}
    PixelPadding(int l, int t, int r, int b):left(l), top(t), right(r), bottom(b){}
    int left, top, right, bottom;
};

class PixelMargin
{
public:
    PixelMargin() :left(0), top(0), right(0), bottom(0) {}

    int left, top, right, bottom;
};

struct FPoint2D
{
    FPoint2D();
    FPoint2D(float _x, float _y);
    FPoint2D(const FPoint2D& other);
    FPoint2D& operator=(const FPoint2D& rhs);
    FPoint2D& operator+=(const FPoint2D& rhs);
    FPoint2D& operator-=(const FPoint2D& rhs);
    const FPoint2D operator+(const FPoint2D& other) const;
    const FPoint2D operator-(const FPoint2D& other) const;

    float x;
    float y;
};

struct FArea2D
{
public:
    FArea2D():width(0.f), height(0.f){}
    FArea2D(float w, float h):width(w), height(h){}
    FArea2D(FArea2D& other)
    {
        if (this != &other)
        {
            this->width = other.width;
            this->height = other.height;
        }
    }
    float width;
    float height;
};

struct NdcBox2D 
{
    NdcBox2D() {}
    NdcBox2D(const Box2D& screenBox, const Area2D& viewport);

    FPoint2D screenToNdc(const Point2D& screenPoint, const Area2D& viewport);
    
    FPoint2D point[4];

};

#endif