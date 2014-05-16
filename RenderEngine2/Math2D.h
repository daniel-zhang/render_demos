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

    // Setters
    void resize(Area2D& size);
    void move(Vector2D& vctr);
    void moveTo(Point2D& pos);

    // Getters
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
    PixelPadding& operator=(const PixelPadding& rhs)
    {
        if (this == &rhs)
            return *this;

        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;

        return *this;
    }
    int left, top, right, bottom;
};
typedef PixelPadding PixelMargin;

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

// Transforms a screen box into a NDC box using the viewport size
struct NdcBox2D 
{
    NdcBox2D() {}
    NdcBox2D(const Box2D& screenBox, const Area2D& viewport);

    FPoint2D screenToNdc(const Point2D& screenPoint, const Area2D& viewport);
    
    FPoint2D point[4];

};

struct FBox2D
{
    FBox2D(){}
    FBox2D(float left, float top, float right, float bottom)
    {
        point[0].x = left; point[0].y = top;
        point[1].x = right; point[1].y = top;
        point[2].x = right; point[2].y = bottom;
        point[3].x = left; point[3].y = bottom;
    }
    FBox2D(const FBox2D& box)
    {
        if (this != &box)
        {
            for (int i = 0; i < 4; ++i)
            {
                point[i] = box.point[i];
            }
        }
    }

    FBox2D& operator=(const FBox2D& box)
    {
        if (this == &box)
            return *this;

        for (int i = 0; i < 4; ++i)
        {
            point[i] = box.point[i];
        }

        return *this;
    }

    FPoint2D point[4];
};

#endif