#ifndef INPUT_H
#define INPUT_H

#include "Event.h"

struct Input
{
    MouseEvent EventMove;
    MouseEvent EventLBtnDown;
    MouseEvent EventLBtnUp;
    MouseEvent EventRBtnDown;
    MouseEvent EventRBtnUp;
    MouseEvent EventMouseWheelUp; 
    MouseEvent EventMouseWheelDown; 


    MouseEvent EventViewportResize;
};
#endif