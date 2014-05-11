#ifndef GUI_EVENT_H
#define GUI_EVENT_H

#include "Math2D.h"

class GUIEvent
{
public:
    enum EventType 
    {
        WidgetResize = 0,
        MouseMove,
        MouseLBtnDown,
        MouseLBtnUp,
        MouseEnter,
        MouseLeave,
        KeyboardDown,
        KeyboardUp,
        MouseWheelUp,
        MouseWheelDown,

        GUIEventNumber
    };
public:
    GUIEvent(EventType evtType) : mPropagate(true), mEventType(evtType){}
    ~GUIEvent(){}

    EventType mEventType;
    bool mPropagate;
};

class WidgetResizeEvent : public GUIEvent
{
public:
    WidgetResizeEvent():GUIEvent(WidgetResize){}
    Area2D mSize;
};

class MouseMoveEvent : public GUIEvent
{
public:
    MouseMoveEvent():GUIEvent(MouseMove){}
    Point2D mMousePos;
    Point2D mLastMousePos;
};

class MouseLBtnDownEvent: public GUIEvent
{
public:
    MouseLBtnDownEvent():GUIEvent(MouseLBtnDown){}
    Point2D mMousePos;
};

class MouseLBtnUpEvent : public GUIEvent
{
public:
    MouseLBtnUpEvent():GUIEvent(MouseLBtnUp){}
    Point2D mMousePos;
};

class MouseWheelUpEvent: public GUIEvent
{
public:
    MouseWheelUpEvent():GUIEvent(MouseWheelUp){}
    Point2D mMousePos;
};

class MouseWheelDownEvent: public GUIEvent
{
public:
    MouseWheelDownEvent():GUIEvent(MouseWheelDown){}
    Point2D mMousePos;
};

#endif