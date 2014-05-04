/*
Notes
1. represent a quad defined in screen space
2. transform that quad into dynamic vertex buffer so it can be updated
3. add a shader to render quad 
*/
#include "SpriteDemo.h"
#include "EffectMgr.h"
#include "RenderStateMgr.h"

SpriteDemo::SpriteDemo()
{
}

SpriteDemo::~SpriteDemo()
{
}

bool SpriteDemo::init()
{
    if (DemoBasic::init() == false)
    {
        return false;
    }

    //mFont.init();

    mWidgetMgr.init(md3dDevice, md3dImmediateContext, &mMouse);
    mWidgetMgr.batchBegin();
    /*
    float x_step = 0.4f;
    float y_step = 0.3f;
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            WidgetBase* p = mWidgetMgr.createWidget(-0.9f + i*x_step, 0.9f - j*y_step, 100, 60, std::wstring(L"HelloGameUI"));
            p->setText(std::wstring(L"Hello-World"));
        }
    }
    */
    WidgetBase* p = mWidgetMgr.createWidget(-0.9f, 0.9f, 400, 300, std::wstring(L"HelloGameUI"));
    p->setText(std::wstring(L"The Lord Of The Ring The Fellowship Of The Ring"));
    mWidgetMgr.batchEnd();

    return true;
}

void SpriteDemo::drawScene()
{
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    md3dImmediateContext->IASetInputLayout(InputLayoutMgr::PosNormalTex);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set per frame constants.
    EffectMgr::BasicFX->setDirLights(mDirLights);
    EffectMgr::BasicFX->setEyePosW(mEyePosWorld);
    EffectMgr::BasicFX->setFogColor(Colors::Silver);
    EffectMgr::BasicFX->setFogStart(15.0f);
    EffectMgr::BasicFX->setFogRange(175.0f);

    //
    // Draw grid
    //
    // Update Cam viewproj to shader program
    XMFLOAT4X4 vp;
    XMStoreFloat4x4(&vp, mCam.ViewProj());
    // TODO: let model to choose tech based on the model to be rendered.
    ID3DX11EffectTechnique* tech = 0;
    md3dImmediateContext->RSSetState(RenderStateMgr::WireframeRS);
    mpModel->draw(tech, vp);
    md3dImmediateContext->RSSetState(0);

    //
    // Draw UI
    //
    mWidgetMgr.drawUI();

    HR(mSwapChain->Present(0, 0));
}

void SpriteDemo::onResize()
{
    DemoBasic::onResize();
    
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);
    mWidgetMgr.onViewportResize(static_cast<int>(vp.Width), static_cast<int>(vp.Height));
}

void SpriteDemo::onMouseMove( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseMove(btnState, x, y);

    mMouse.EventMove.fire(x, y);
}

