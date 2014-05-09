#include "WidgetMgr2.h"
#include "GUIRenderer.h"

WidgetMgr2::WidgetMgr2(): mInput(0), mRoot(0), mRenderer(0) { }
WidgetMgr2::~WidgetMgr2() 
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i])
            delete mWidgets[i];
    }
    mWidgets.clear();
}

bool WidgetMgr2::init( Input* input, GUIRenderer* renderer)
{
    mInput = input;
    mRenderer = renderer;
    mSolver.init(renderer);

    hookEventHandlers();

    return true;
}


Widget* WidgetMgr2::createRootWidget(std::string name,  Area2D& viewportSize, RGBA& color )
{
    // Root duplication is not allowed
    if (mRoot)
        return NULL;

    mRoot = new Widget(name, viewportSize, color, &mSolver);
    mWidgets.push_back(mRoot); 

    return mRoot;
}
Widget* WidgetMgr2::createChildWidget(std::string name,  Widget* parent, Area2D& size, PixelPadding& padding, RGBA& color, LayoutType layoutType /*= WIDGET_LAYOUT_NULL */ )
{
    // Parent must be valid
    if (parent == NULL)
        return NULL;

    Widget* w = 0;

    if(layoutType != WIDGET_LAYOUT_NOT_SPECIFIED)
        w = new Widget(name, parent, size, padding, color, &mSolver, layoutType);
    else
        w = new Widget(name, parent, size, padding, color, &mSolver);

    mWidgets.push_back(w);

    return w;
}

bool WidgetMgr2::hookEventHandlers()
{
    if (mInput== 0)
        return false;

    mInput->EventMove.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr2::onMouseMove, this, _1, _2));
    mInput->EventLBtnDown.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr2::onMouseLBtnDown, this, _1, _2));
    mInput->EventLBtnUp.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr2::onMouseLBtnUp, this, _1, _2));
    mInput->EventViewportResize.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr2::onViewportResize, this, _1, _2));

    return true;
}

void WidgetMgr2::onViewportResize( int width, int height )
{
    mRoot->onResize(Area2D(width, height));
    mRenderer->onViewportResize(Area2D(width, height));
}

void WidgetMgr2::onMouseMove( int x, int y )
{
    Point2D currMousePos(x, y);
    Vector2D movement = currMousePos - mLastMousePos;

    mRoot->onMouseMove(movement);

    mLastMousePos = currMousePos;
}

void WidgetMgr2::onMouseLBtnDown( int x, int y )
{
    mRoot->onLBtnDown(Point2D(x, y));
}

void WidgetMgr2::onMouseLBtnUp( int x, int y )
{
    mRoot->onLBtnUp(Point2D(x, y));
}


