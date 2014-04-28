#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include "Util.h"

// Window position is specified in NDC space
// Window size is specified in screen pixel
// As we do not want button size to be fixed and independent of VP size changes.
//
// Window does not have to know any details about rendering
class BaseWindow
{
public:
    BaseWindow(int width, int height, XMFLOAT2 position, std::string winName);
    ~BaseWindow();
    void onHover();

    XMFLOAT2 getPos(){return mPos;}
    XMFLOAT2 getSize(){return mSize;}

private:
    std::string mWinName;
    XMFLOAT2 mSize;
    XMFLOAT2 mPos;
};

#endif