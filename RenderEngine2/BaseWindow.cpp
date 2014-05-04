#include "BaseWindow.h"

BaseWidget::BaseWidget( int width, int height, XMFLOAT2 position, std::string winName )
{
    mSize.x = static_cast<float>(width);
    mSize.y = static_cast<float>(height);
    mPos = position;
    mWidgetName = winName;
}

BaseWidget::~BaseWidget()
{

}

void BaseWidget::onHover( int x, int y )
{

}

void BaseWidget::onClick( int x, int y )
{

}

