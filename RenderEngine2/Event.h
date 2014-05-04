#ifndef EVENT_H
#define EVENT_H

/*
usecase #1
EventSrc src;
EventSink sink;

sink.regEvent(&src);
source.fire(something);
sink.unregEvent(&src);
*/

enum MouseKeyEvent
{
    LeftButtonDown,
    LeftButtonDoubleClick,
    LeftButtonUp,
    RightButtonDown,
    RightButtonUp
};

template<typename klass, typename F>
class EventFunctor
{
public:
    EventFunctor(klass*, F){}
};

class EventFunctionoid
{
public:
    EventFunctionoid(){}
    virtual ~EventFunctionoid(){}
};

template <typename klass>
class MouseEventFunctionoid : public EventFunctionoid
{
public:
    typedef int(*FuncType)(int, int);
    MouseEventFunctionoid(klass* kls, FuncType func)
    {
        mKlass = kls;
        mFunc = func;
    }
    int operator()(int x, int y)
    {
        return mKlass->(mFunc*)(x, y);
    }

private:
    klass* mKlass;
    FuncType mFunc;
};

class SampleListener
{
public:
    SampleListener()
    {
        EventFunctor<SampleListener, int(SampleListener::*)(int, int)>
            (this, &SampleListener::onLButtonDown);
    }
    int onLButtonDown(int x, int y)
    {
        return x + y;
    }
};

#endif