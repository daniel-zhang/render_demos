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

    /*
    mWidgetMgr.init(md3dDevice, md3dImmediateContext, &mMouse);
    mWidgetMgr.batchBegin();
    WidgetBase* p = mWidgetMgr.createWidget(-0.9f, 0.9f, 400, 300, std::wstring(L"HelloGameUI"));
    p->setText(std::wstring(L"The Lord Of The Ring\nThe Fellowship Of The Ring"));
    mWidgetMgr.batchEnd();
    */

    // version 2
    mRenderer.init(md3dDevice, md3dImmediateContext);
    mWidgetMgr2.init(&mInput, &mRenderer);

    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);

    mWidgetMgr2.createBegin();
    Widget* root = mWidgetMgr2.createRootWidget("root",Area2D(static_cast<int>(vp.Width), static_cast<int>(vp.Height)), RGBAColor::Transparent);
    Widget* w1 = mWidgetMgr2.createChildWidget("", root, Area2D(700, 500), PixelPadding(), RGBAColor::Red);
    for (UINT i = 0; i < 500; ++i)
    {
        mWidgetMgr2.createChildWidget("", w1, Area2D(20, 20), PixelPadding(), RGBAColor::Green);
    }

    mWidgetMgr2.createEnd();

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

    mWidgetMgr2.draw();

    HR(mSwapChain->Present(0, 0));
}

void SpriteDemo::onResize()
{
    DemoBasic::onResize();
    
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);
    mWidgetMgr.onViewportResize(static_cast<int>(vp.Width), static_cast<int>(vp.Height));

    mInput.EventViewportResize.fire(static_cast<int>(vp.Width), static_cast<int>(vp.Height));
}

void SpriteDemo::onMouseMove( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseMove(btnState, x, y);

//    mMouse.EventMove.fire(x, y);

    mInput.EventMove.fire(x, y);
}

void SpriteDemo::onMouseDown( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseDown(btnState, x, y);

    if( (btnState & MK_LBUTTON) != 0 )
    {
        mWidgetMgr.onMouseLBtnDown(x, y);

        mInput.EventLBtnDown.fire(x, y);
    }
    else if ( (btnState & MK_RBUTTON) != 0)
    {
        mWidgetMgr.onMouseRBtnDown(x, y);
    }
    else
    {
    }
}

void SpriteDemo::onMouseUp( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseUp(btnState, x, y);

    mWidgetMgr.onMouseLBtnUp(x, y);

    mInput.EventLBtnUp.fire(x, y);

    if( (btnState & MK_LBUTTON) != 0 )
    {
    }
    else if ( (btnState & MK_RBUTTON) != 0)
    {
        mWidgetMgr.onMouseRBtnUp(x, y);
    }
    else
    {
    }
}

void SpriteDemo::onMouseWheel( WPARAM btnState, int x, int y )
{
    int zDelta = GET_WHEEL_DELTA_WPARAM(btnState);
    if (zDelta < 0)
    {
        mWidgetMgr.onMouseWheelUp(x, y);
        mInput.EventMouseWheelUp.fire(x, y);
    }
    else
    {
        mWidgetMgr.onMouseWheelDown(x, y);
        mInput.EventMouseWheelDown.fire(x, y);
    }
}

