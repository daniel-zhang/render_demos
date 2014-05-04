#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include "Util.h"

// Window position is specified in NDC space
// Window size is specified in screen pixel
// As we do not want button size to be fixed and independent of VP size changes.
//
// Window does not have to know any details about rendering
class BaseWidget
{
public:
    BaseWidget(int width, int height, XMFLOAT2 position, std::string winName);
    ~BaseWidget();

    void onHover(int x, int y);
    void onClick(int x, int y);

    XMFLOAT2 getPos(){return mPos;}
    XMFLOAT2 getSize(){return mSize;}

private:
    std::string mWidgetName;
    XMFLOAT2 mSize;
    XMFLOAT2 mPos;
};

#endif