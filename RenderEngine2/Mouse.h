#ifndef MOUSE_H
#define MOUSE_H

#include "MouseEvent.h"

struct Mouse
{
    Mouse();

    int mLastPosX, mLastPosY;

    MouseEvent EventMove;

    MouseEvent EventLBtnDown;
    MouseEvent EventLBtnUp;

    MouseEvent EventRBtnDown;
    MouseEvent EventRBtnUp;
};



#endif
