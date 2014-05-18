#include "WidgetMgr.h"
#include "GUIRenderer.h"
#include "GUIEvent.h"

WidgetMgr::WidgetMgr(): mInput(0), mRoot(0), mSubmit(true), mRenderer(0) { }
WidgetMgr::~WidgetMgr() 
{
    if (mRenderer)
    {
        delete mRenderer;
        mRenderer = 0;
    }

    if (mRoot)
    {
        delete mRoot;
        mRoot = 0;
    }
}

bool WidgetMgr::init( Input* input, ID3D11Device* device, ID3D11DeviceContext* context )
{
    mInput = input;

    mRenderer = new GUIRenderer();
    if(mRenderer->init(device, context) == false)
        return false;

    mFontEngine.createFontSheet(mFontSheet, L"debug");

    hookEventHandlers();

    if (mRoot)
    {
        delete mRoot; mRoot = 0;
    }
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    context->RSGetViewports(&vpNum, &vp);
    mRoot = new Root( Area2D( static_cast<int>(vp.Width), static_cast<int>(vp.Height) ) );
    mRoot->setWidgetMgr(this);

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

Widget* WidgetMgr::getRoot()
{
    return mRoot;
}

FontSheet* WidgetMgr::getFontSheet()
{
    return &mFontSheet;
}

void WidgetMgr::draw()
{
    if (mSubmit)
    {
        mSubmit = false;

        mRenderer->beginBatch();
        submit();
        mRenderer->endBatch();
    }

    mRenderer->draw();
}

void WidgetMgr::setSubmit()
{
    mSubmit = true;
}

void WidgetMgr::submit()
{
    _submit(mRoot);
}

void WidgetMgr::_submit( Widget* w)
{
    w->updateRenderable();
    mRenderer->sortedBatch(static_cast<IRenderable2D*>(w));

    for (UINT i = 0; i < w->mChildren.size(); ++i)
    {
        _submit(w->mChildren[i]);
    }
}


