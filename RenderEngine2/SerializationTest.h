#ifndef SERIALIZAION_TEST_H
#define SERIALIZAION_TEST_H

#include "DemoBasic.h"

namespace boost { namespace serialization {
    class access; 
} }

class SrcRect 
{
public:
    SrcRect(): left(0), top(0), bottom(0), right(0) { }
    SrcRect(int l, int t, int b, int r):left(l), top(t), bottom(b), right(r){}
    bool operator==(const SrcRect& other) const
    {
        return (left == other.left && right == other.right && top == other.top && bottom == other.bottom );
    }

private:
    int left, top, bottom, right;

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        ar & left & top & bottom & right;
    }
};



class SerializationTest : public DemoBasic
{
public:
    SerializationTest();
    ~SerializationTest();

    virtual bool init();
    /*
    virtual void drawScene();
    virtual void onResize();
    virtual void onMouseMove(WPARAM btnState, int x, int y);
    virtual void onMouseDown(WPARAM btnState, int x, int y);
    virtual void onMouseUp(WPARAM btnState, int x, int y);
    virtual void onMouseWheel(WPARAM btnState, int x, int y);
    */
};

#endif