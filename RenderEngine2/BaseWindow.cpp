#include "BaseWindow.h"

BaseWindow::BaseWindow( int width, int height, XMFLOAT2 position, std::string winName )
{
    mSize.x = static_cast<float>(width);
    mSize.y = static_cast<float>(height);
    mPos = position;
    mWinName = winName;
}

BaseWindow::~BaseWindow()
{

}

void BaseWindow::onHover()
{
    // do something, like applying a small translation (0.1, 0.1) to 'activate' the window
}
