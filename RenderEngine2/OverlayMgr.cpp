#include "OverlayMgr.h"

OverlayMgr::OverlayMgr()
{
    mDevice = 0;
    mCtx = 0;
}

OverlayMgr::~OverlayMgr()
{

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
    mDevice = device;
    mCtx = context;

    mVp.updateViewport(mCtx);

    return true;
}

void OverlayMgr::step( float dt )
{
    mVp.updateViewport(mCtx);
}


void OverlayMgr::draw()
{
    // Batch all window renderings


}

BaseWidget* OverlayMgr::createWindow( std::string winName, int width, int height, XMFLOAT2 position, std::string winClass /*= "Window/DefaultWindow"*/ )
{
    BaseWidget* pWindow = 0;
    if (winClass.compare("Window/DefaultWindow") == 0)
    {
        pWindow = new BaseWidget(width, height, position, winName);
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

bool OverlayMgr::isPointInWindow( int x, int y, BaseWidget* pWindow )
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
