#ifndef MOUSE_H
#define MOUSE_H

#include "Event.h"

struct Mouse
{
    Mouse()
    {
        mLastPosX = 0;
        mLastPosY = 0;
    }

    int mLastPosX, mLastPosY;

    MouseEvent EventMove;

    MouseEvent EventLBtnDown;
    MouseEvent EventLBtnUp;

    MouseEvent EventRBtnDown;
    MouseEvent EventRBtnUp;

    // TODO: this does not make many sense
    // Need an abstract event type: IO_EVENT
    MouseEvent EventViewportResize;
};



#endif
