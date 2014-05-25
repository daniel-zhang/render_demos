#ifndef MATH_2D_H 
#define MATH_2D_H

#include "Util.h"

struct FPoint2D;
struct Area2D;

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
    bool operator==(const Point2D& rhs) const;
    bool operator!=(const Point2D& rhs) const;

    FPoint2D toNDC(const Area2D& viewport);

    int x;
    int y;
};
typedef Point2D Vector2D;

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

struct Area2D
{
    Area2D();
    Area2D(int w, int h);
    Area2D(const Area2D& rhs);

    int width;
    int height; 
};

struct FArea2D
{
public:
    FArea2D():width(0.f), height(0.f){}
    FArea2D(float w, float h):width(w), height(h){}
    FArea2D(FArea2D& other);
    float width;
    float height;
};

// An int box in screen space
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
    Area2D getArea();
    int getWidth();
    int getHeight();
    int getLeft();
    int getRight();
    int getTop();
    int getBottom();

    // Misc
    bool isPointInside(Point2D& pos);
    void zero();
    void getIntersection(Box2D& box1, Box2D& box2);

    Point2D point[4];
};

class PixelPadding
{
public:
    PixelPadding():left(0), top(0), right(0), bottom(0){}
    PixelPadding(int l, int t, int r, int b):left(l), top(t), right(r), bottom(b){}
    PixelPadding& operator=(const PixelPadding& rhs);
    int left, top, right, bottom;
};
typedef PixelPadding PixelMargin;

// A float box in NDC space
// where positive x is left to right,
// positive y is bottom to top,
// and the origin is at the center of viewport
struct NdcBox2D 
{
    NdcBox2D() {}
    NdcBox2D(FPoint2D& pos, FArea2D& size);
    // Transforms a screen box into a NDC box using the viewport size
    NdcBox2D(const Box2D& screenBox, const Area2D& viewport);
    NdcBox2D& operator=(const NdcBox2D& rhs);

    // Build a NDC space box whose left-top is at the NDC space origin
    // and whose size is specified in screen pixel by @parameter 'size'
    void buildFromScreenArea(Area2D& size, Area2D& vp);
    FPoint2D screenToNdc(const Point2D& screenPoint, const Area2D& viewport);
    float getWidth();
    float getHeight();
    int getScreenWidth(Area2D& vpSize);
    int getScreenHeight(Area2D& vpSize);
    void zero();
    
    FPoint2D point[4];
};


// A float box in texture space
struct FBox2D
{
    FBox2D(){}
    FBox2D(float left, float top, float right, float bottom);
    FBox2D(const FBox2D& box);

    FBox2D& operator=(const FBox2D& box);

    void build(float left, float top, float right, float bottom);

    FPoint2D point[4];
};

#endif