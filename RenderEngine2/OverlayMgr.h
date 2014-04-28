#ifndef OVERLAY_MGR_H
#define OVERLAY_MGR_H

#include "Util.h"
#include "Input.h"
#include "BaseWindow.h"

struct Viewport
{
    Viewport()
    {
        ZeroMemory(this, sizeof(Viewport));
    }

    void updateViewport(ID3D11DeviceContext* ctx)
    {
        D3D11_VIEWPORT vp;
        UINT vpNum = 1;
        ctx->RSGetViewports(&vpNum, &vp);
        mWidth = vp.Width;
        mHeight = vp.Height;
    }
    float mWidth;
    float mHeight;
};

class OverlayMgr
{
public:
    OverlayMgr();
    ~OverlayMgr();

    bool init(ID3D11Device* device, ID3D11DeviceContext* context);
    void step(float dt);
    void draw();
    Mouse* getMouse();

    BaseWindow* createWindow(std::string winName, int width, int height, XMFLOAT2 position, std::string winClass = "Window/DefaultWindow");

protected:
    void updateViewport();
    bool isPointInWindow(int x, int y, BaseWindow* pWindow);
    XMFLOAT2 ndc2screen(XMFLOAT2 ndcPos);

protected:
    
    Viewport mVp;
    Mouse* mMouse;
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mCtx;

    std::vector<BaseWindow*> mWindows;
};



#endif