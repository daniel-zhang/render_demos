#include "WidgetMgr.h"
#include "GUIEvent.h"

WidgetMgr::WidgetMgr(): mInput(0), mRoot(0){ }
WidgetMgr::~WidgetMgr() 
{
    if (mRoot)
    {
        delete mRoot;
        mRoot = 0;
    }
}

bool WidgetMgr::init( Input* input, ID3D11Device* device, ID3D11DeviceContext* context )
{
    mInput = input;
    hookEventHandlers();

    if(!mSpriteRenderer.init(device, context))
        return false;

    // In case of init() is called twice
    if (mRoot)
    {
        delete mRoot; mRoot = 0;
    }

    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    context->RSGetViewports(&vpNum, &vp);
    mRoot = new Root(getRenderEnv(), Area2D( static_cast<int>(vp.Width), static_cast<int>(vp.Height) ) );

    return true;
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
    mSpriteRenderer.onViewportResize(Area2D(width, height));

    WidgetResizeEvent evt;
    evt.mSize = Area2D(width, height);

    mRoot->dispatch(evt);
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

void WidgetMgr::draw()
{
    mSpriteRenderer.beforeDraw();
    submitToRenderer(mRoot);
    mSpriteRenderer.afterDraw();
}

void WidgetMgr::submitToRenderer( Widget* widget )
{
    if (widget->mVisible)
    {
        widget->beforeDrawSelf();
        for(UINT i = 0;  i < widget->getValidSpritesNumber(); ++i)
            mSpriteRenderer.draw(*(widget->mSprites[i]));
        widget->afterDrawSelf();
    }
    
    widget->beforeDrawChildren();
    for (UINT i = 0; i < widget->mDepthQueue.size(); ++i)
    {
        submitToRenderer(widget->mDepthQueue[i]);
    }
    widget->afterDrawChildren();
}

void WidgetMgr::addWidget( Widget* widget, Widget* parent /*= NULL*/ )
{
    if (widget)
    {
        if (parent)
        {
            parent->addChild(widget);
        }
        else
        {
            mRoot->addChild(widget);
        }
        widget->init();
    }
}

D3DEnv* WidgetMgr::getRenderEnv()
{
    return mSpriteRenderer.getRenderEnv();
}


