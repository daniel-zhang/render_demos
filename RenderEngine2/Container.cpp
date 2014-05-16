#include "Container.h"

Container::Container( Widget* parent, Area2D& size, PixelPadding& padding, PixelMargin& margin, const RGBA& color, LayoutType layoutType )
    : Widget(parent, size, padding, margin, color, layoutType)
{
}

void Container::onLayoutChanged( GUIEvent& evt )
{

}

void Container::onResize( GUIEvent& e )
{

}

void Container::onLBtnDown( GUIEvent& e )
{

}

void Container::onLBtnUp( GUIEvent& e )
{

}

void Container::onMouseMove( GUIEvent& e )
{

}

void Container::onMouseWheelUp( GUIEvent& e )
{

}

void Container::onMouseWheelDown( GUIEvent& e )
{

}

void Container::onMouseEnter( GUIEvent& e )
{

}

void Container::onMouseLeave( GUIEvent& e )
{

}
