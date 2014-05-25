#include "Math2D.h"
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
Point2D::Point2D()
{
    x = 0; y = 0;
}

Point2D::Point2D(int _x, int _y):x(_x), y(_y)
{}

Point2D::Point2D(const Point2D& other)
{
    if (this != &other)
    {
        this->x = other.x; 
        this->y = other.y;
    }
}

Point2D& Point2D::operator=(const Point2D& rhs)
{
    if (this == &rhs)
        return *this;

    this->x = rhs.x; 
    this->y = rhs.y; 
    return *this;
}

Point2D& Point2D::operator+=(const Point2D& rhs)
{
    this->x += rhs.x; 
    this->y += rhs.y; 
    return *this;
}

Point2D& Point2D::operator-=(const Point2D& rhs)
{
    this->x -= rhs.x; 
    this->y -= rhs.y; 
    return *this;
}

const Point2D Point2D::operator+(const Point2D& other) const
{ 
    return Point2D(*this) += other; 
}

const Point2D Point2D::operator-(const Point2D& other) const
{ 
    return Point2D(*this) -= other; 
}

bool Point2D::operator==( const Point2D& rhs ) const
{
    if (this->x == rhs.x && this->y == rhs.y)
        return true;
    return false;
}

bool Point2D::operator!=( const Point2D& rhs ) const
{
    return !(*this == rhs);
}

FPoint2D Point2D::toNDC( const Area2D& viewport )
{
    FPoint2D ndcPoint;
    ndcPoint.x = 2.f * (static_cast<float>(this->x)/viewport.width) - 1.f;
    ndcPoint.y = 1.f - 2.f * (static_cast<float>(this->y)/viewport.height);
    return ndcPoint;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
Area2D::Area2D():width(0), height(0) {}
Area2D::Area2D(int w, int h):width(w), height(h) {}
Area2D::Area2D(const Area2D& pa)
{
    if (this != &pa)
    {
        this->width = pa.width; 
        this->height = pa.height;
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
Box2D::Box2D() {}
Box2D::Box2D( Point2D& pos, Area2D& size )
{
    point[0] = pos;
    point[1] = pos + Vector2D(size.width, 0);
    point[2] = pos + Vector2D(size.width, size.height);
    point[3] = pos + Vector2D(0, size.height);
}
Box2D::Box2D( int left, int top, int right, int bottom )
{
    point[0].x = left; point[0].y = top;
    point[1].x = right; point[1].y = top;
    point[2].x = right; point[2].y = bottom;
    point[3].x = left; point[3].y = bottom;
}
Box2D::Box2D(const Box2D& box )
{
    if (this != &box)
    {
        for (int i = 0; i < 4; ++i)
        {
            point[i] = box.point[i];
        }
    }
}

Box2D& Box2D::operator=( const Box2D& box )
{
    if (this == &box)
        return *this;

    for (int i = 0; i < 4; ++i)
    {
        point[i] = box.point[i];
    }
    return *this;
}

Point2D Box2D::getCenter()
{
    Point2D center;
    center.x = (point[0] + point[1]).x/2;
    center.y = (point[1] + point[2]).y/2;

    return center;
}

int Box2D::getWidth()
{
    return (point[1].x - point[0].x);
}

int Box2D::getHeight()
{
    return (point[2].y - point[1].y);
}

int Box2D::getLeft()
{
    return point[0].x;
}

int Box2D::getRight()
{
    return point[1].x;
}

int Box2D::getTop()
{
    return point[0].y;
}

int Box2D::getBottom()
{

    return point[2].y;
}

bool Box2D::isPointInside( Point2D& pos )
{
    if ( pos.x >= point[0].x && pos.x <= point[1].x
        && pos.y >= point[0].y && pos.y <= point[2].y )
        return true;
    else
        return false;
}

// resize() does not change point[0](top-left) position
void Box2D::resize( Area2D& size )
{
    point[1] = point[0] + Vector2D(size.width, 0);
    point[2] = point[0] + Vector2D(size.width, size.height);
    point[3] = point[0] + Vector2D(0, size.height);
}

void Box2D::move( Vector2D& vctr )
{
    for (UINT i = 0; i < 4; ++i)
    {
        point[i] += vctr;
    }
}

void Box2D::moveTo( Point2D& pos )
{
    Vector2D delta = pos - point[0];
    move(delta);
}

Area2D Box2D::getArea()
{
    return Area2D(this->getWidth(), this->getHeight());
}

void Box2D::zero()
{
    for (UINT i = 0; i < 4; ++i)
    {
        point[i].x = 0; point[i].y = 0;
    }
}

void Box2D::getIntersection( Box2D& box1, Box2D& box2 )
{
    int left = box1.getLeft() < box2.getLeft() ? box2.getLeft() : box1.getLeft();
    int right = box1.getRight() < box2.getRight() ? box1.getRight() : box2.getRight();
    int top = box1.getTop() < box2.getTop() ? box2.getTop() : box1.getTop();
    int bottom = box1.getBottom() < box2.getBottom() ? box1.getBottom() : box2.getBottom();

    if (left > right || top > bottom)
    {
        left = right = top = bottom = 0;
    }

    point[0].x = left; point[0].y = top;
    point[1].x = right; point[1].y = top;
    point[2].x = right; point[2].y = bottom;
    point[3].x = left; point[3].y = bottom;
}

//////////////////////////////////////////////////////////////////
FPoint2D::FPoint2D()
{
    x=0.f; y = 0.f;
}

FPoint2D::FPoint2D( float _x, float _y ) :x(_x), y(_y) {}

FPoint2D::FPoint2D( const FPoint2D& other )
{
    if (this != &other )
    {
        this->x = other.x; 
        this->y = other.y;
    }
}

FPoint2D& FPoint2D::operator=( const FPoint2D& rhs )
{
    if (this == &rhs)
        return *this;

    this->x = rhs.x; 
    this->y = rhs.y; 
    return *this;
}

FPoint2D& FPoint2D::operator+=( const FPoint2D& rhs )
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

FPoint2D& FPoint2D::operator-=( const FPoint2D& rhs )
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

const FPoint2D FPoint2D::operator+( const FPoint2D& other ) const
{
    return FPoint2D(*this) += other;
}

const FPoint2D FPoint2D::operator-( const FPoint2D& other ) const
{
    return FPoint2D(*this) -= other;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


NdcBox2D::NdcBox2D( const Box2D& screenBox, const Area2D& viewport )
{
    for (int i = 0; i < 4; ++i)
        point[i] = screenToNdc(screenBox.point[i], viewport);
}

NdcBox2D::NdcBox2D( FPoint2D& pos, FArea2D& size )
{
    point[0] = pos;
    point[1] = pos + FPoint2D(size.width, 0);
    point[2] = pos + FPoint2D(size.width, -1.f * size.height);
    point[3] = pos + FPoint2D(0, -1.f * size.height);
}

FPoint2D NdcBox2D::screenToNdc( const Point2D& screenPoint, const Area2D& viewport )
{
    FPoint2D ndcPoint;
    ndcPoint.x = 2.f * (static_cast<float>(screenPoint.x)/viewport.width) - 1.f;
    ndcPoint.y = 1.f - 2.f * (static_cast<float>(screenPoint.y)/viewport.height);
    return ndcPoint;
}

NdcBox2D& NdcBox2D::operator=( const NdcBox2D& rhs )
{
    for (UINT i = 0; i < 4; ++i)
    {
        point[i] = rhs.point[i];
    }
    return (*this);
}

float NdcBox2D::getWidth()
{
    return (point[1].x - point[0].x);
}

float NdcBox2D::getHeight()
{
    return (point[0].y - point[3].y);
}

void NdcBox2D::buildFromScreenArea( Area2D& size, Area2D& vp )
{
    float width = 2.f * static_cast<float>(size.width)/vp.width;
    float height = 2.f * static_cast<float>(size.height)/vp.height;
    point[0] = FPoint2D(0.f, 0.f);
    point[1] = FPoint2D(width, 0);
    point[2] = FPoint2D(width, -1.f* height);
    point[3] = FPoint2D(0, -1.f * height);
}

void NdcBox2D::zero()
{
    for (UINT i = 0; i < 4; ++i)
    {
        point[i].x = 0.f; point[i].y = 0.f;
    }
}

int NdcBox2D::getScreenWidth( Area2D& vpSize )
{
    return static_cast<int>(getWidth()*vpSize.width / 2);
}

int NdcBox2D::getScreenHeight( Area2D& vpSize )
{
    return static_cast<int>(getHeight()*vpSize.height / 2);
}

PixelPadding& PixelPadding::operator=( const PixelPadding& rhs )
{
    if (this == &rhs)
        return *this;

    left = rhs.left;
    top = rhs.top;
    right = rhs.right;
    bottom = rhs.bottom;

    return *this;
}

FBox2D::FBox2D( float left, float top, float right, float bottom )
{
    build(left, top, right, bottom);
}

void FBox2D::build( float left, float top, float right, float bottom )
{
    point[0].x = left; point[0].y = top;
    point[1].x = right; point[1].y = top;
    point[2].x = right; point[2].y = bottom;
    point[3].x = left; point[3].y = bottom;
}

FBox2D::FBox2D( const FBox2D& box )
{
    if (this != &box)
    {
        for (int i = 0; i < 4; ++i)
        {
            point[i] = box.point[i];
        }
    }
}

FBox2D& FBox2D::operator=( const FBox2D& box )
{
    if (this == &box)
        return *this;

    for (int i = 0; i < 4; ++i)
    {
        point[i] = box.point[i];
    }

    return *this;
}

FArea2D::FArea2D( FArea2D& other )
{
    if (this != &other)
    {
        this->width = other.width;
        this->height = other.height;
    }
}
