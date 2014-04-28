#include "OverlayMgr.h"

OverlayMgr::OverlayMgr()
{
    mMouse = 0;
    mDevice = 0;
    mCtx = 0;
}

OverlayMgr::~OverlayMgr()
{
    if (mMouse)
    {
        delete mMouse;
        mMouse = 0;
    }

    for (UINT i = 0; i < mWindows.size(); ++i)
    {
        if (mWindows[i])
        {
            delete mWindows[i];
            mWindows[i] = 0;
        }
    }
}

bool OverlayMgr::init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    mMouse = new Mouse();
    
    mDevice = device;
    mCtx = context;

    mVp.updateViewport(mCtx);

    return true;
}

void OverlayMgr::step( float dt )
{
    mVp.updateViewport(mCtx);

    for (UINT i = 0; i < mWindows.size(); ++i)
    {
        // dispatch onHover event
        if (isPointInWindow(mMouse->mPosX, mMouse->mPosY, mWindows[i]))
        {
            mWindows[i]->onHover();
        }
    }
}

Mouse* OverlayMgr::getMouse()
{
    return mMouse;
}

void OverlayMgr::draw()
{
    // Batch all window renderings


}

BaseWindow* OverlayMgr::createWindow( std::string winName, int width, int height, XMFLOAT2 position, std::string winClass /*= "Window/DefaultWindow"*/ )
{
    BaseWindow* pWindow = 0;
    if (winClass.compare("Window/DefaultWindow") == 0)
    {
        pWindow = new BaseWindow(width, height, position, winName);
    }
    else if (winClass.compare("Window/FrameWindow") == 0)
    {
        //TODO
        pWindow = 0;
    }
    else
    {
        // not identified window class
        return NULL;
    }

    mWindows.push_back(pWindow);

    return pWindow;
}

bool OverlayMgr::isPointInWindow( int x, int y, BaseWindow* pWindow )
{
    // Transform window position to screen space according to the current viewport
    XMFLOAT2 screenPos = ndc2screen(pWindow->getPos());

    int left = static_cast<int>(screenPos.x);
    int upper = static_cast<int>(screenPos.y);
    int right = static_cast<int>(screenPos.x + pWindow->getSize().x);
    int bottom = static_cast<int>(screenPos.y + pWindow->getSize().y);

    if (x >= left && x <= right && y <= bottom && y >= upper )
        return true;
    else
        return false;
}

XMFLOAT2 OverlayMgr::ndc2screen( XMFLOAT2 ndcPos )
{
    XMFLOAT2 pos;
    pos.x = (ndcPos.x + 1.f)*0.5f * mVp.mWidth;
    pos.y = (1.f - ndcPos.y)*0.5f * mVp.mHeight;

    return pos;
}