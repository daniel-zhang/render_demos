#include "WidgetMgr.h"
#include "GUIRenderer.h"
#include "GUIEvent.h"

WidgetMgr::WidgetMgr(): mInput(0), mRoot(0), mRenderer(0) { }
WidgetMgr::~WidgetMgr() 
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i])
            delete mWidgets[i];
    }
    mWidgets.clear();
}

bool WidgetMgr::init( Input* input, GUIRenderer* renderer)
{
    mInput = input;
    mRenderer = renderer;

    mFontEngine.createFontSheet(mFontSheet, L"debug");

    hookEventHandlers();

    return true;
}


Widget* WidgetMgr::createRootWidget( Area2D& viewportSize, const RGBA& color )
{
    // Root duplication is not allowed
    if (mRoot)
        return NULL;

    mRoot = new Widget( viewportSize, color, &mSolver);
    mRoot->mTextBlock.init(&mFontSheet);
    mWidgets.push_back(mRoot); 

    return mRoot;
}
Widget* WidgetMgr::createChildWidget(Widget* parent, Area2D& size, PixelPadding& padding, const RGBA& color, LayoutType layoutType /*= WIDGET_LAYOUT_NULL */ )
{
    // Parent must be valid
    if (parent == NULL)
        return NULL;

    Widget* w = 0;

    if(layoutType != WIDGET_LAYOUT_NOT_SPECIFIED)
        w = new Widget( parent, size, padding, color, &mSolver, layoutType);
    else
        w = new Widget( parent, size, padding, color, &mSolver);

    w->mTextBlock.init(&mFontSheet);
    mWidgets.push_back(w);

    return w;
}

bool WidgetMgr::hookEventHandlers()
{
    if (mInput== 0)
        return false;

    mInput->EventMove.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr::onMouseMove, this, _1, _2));
    mInput->EventLBtnDown.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr::onMouseLBtnDown, this, _1, _2));
    mInput->EventLBtnUp.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr::onMouseLBtnUp, this, _1, _2));
    mInput->EventViewportResize.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr::onViewportResize, this, _1, _2));
    mInput->EventMouseWheelUp.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr::onMouseWheelUp, this, _1, _2));
    mInput->EventMouseWheelDown.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr::onMouseWheelDown, this, _1, _2));

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// System Event handlers
//////////////////////////////////////////////////////////////////////////////////////
void WidgetMgr::onViewportResize( int width, int height )
{
    WidgetResizeEvent evt;
    evt.mSize = Area2D(width, height);

    mRoot->dispatch(evt);
    mRenderer->onViewportResize(Area2D(width, height));
}

void WidgetMgr::onMouseMove( int x, int y )
{
    Point2D currMousePos(x, y);

    MouseMoveEvent evt;
    evt.mLastMousePos = mLastMousePos;
    evt.mMousePos = currMousePos;
    mRoot->dispatch(evt);

    mLastMousePos = currMousePos;
}

void WidgetMgr::onMouseLBtnDown( int x, int y )
{
    MouseLBtnDownEvent evt;
    evt.mMousePos = Point2D(x, y);

    mRoot->dispatch(evt);
}

void WidgetMgr::onMouseLBtnUp( int x, int y )
{
    MouseLBtnUpEvent evt;
    evt.mMousePos = Point2D(x, y);

    mRoot->dispatch(evt);
}

void WidgetMgr::onMouseWheelUp( int x, int y )
{
   MouseWheelUpEvent evt;
   evt.mMousePos = Point2D(x, y);

   mRoot->dispatch(evt);
}

void WidgetMgr::onMouseWheelDown( int x, int y )
{
   MouseWheelDownEvent evt;
   evt.mMousePos = Point2D(x, y);

   mRoot->dispatch(evt);
}


