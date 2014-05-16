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

    mRenderer.init(md3dDevice, md3dImmediateContext);
    mWidgetMgr.init(&mInput, &mRenderer);

    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);

    mWidgetMgr.createBegin();
    Widget* root = mWidgetMgr.createRootWidget(Area2D(static_cast<int>(vp.Width), static_cast<int>(vp.Height)), RGBAColor::Transparent);
    Widget* w1 = mWidgetMgr.createChildWidget(root, Area2D(300, 600), PixelPadding(), RGBAColor::Blue);
    for (UINT i = 0; i < 1; ++i)
    {
        Widget* w2 = mWidgetMgr.createChildWidget(w1, Area2D(250, 340), PixelPadding(), RGBAColor::Background,  WIDGET_LAYOUT_VERTICAL );
        w2->setText(std::wstring(
           L"// Font sheet bmp must be freed here...\n"
           L"// Basically it does not make sense to let FontSheet hold a pointer to the Bitmap object...\n"
           L"// Let me fix it later.\n"), 24);
    }

    mWidgetMgr.createEnd();

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

    mWidgetMgr.draw();

    HR(mSwapChain->Present(0, 0));
}

void SpriteDemo::onResize()
{
    DemoBasic::onResize();
    
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);

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

        mInput.EventLBtnDown.fire(x, y);
    }
    else if ( (btnState & MK_RBUTTON) != 0)
    {
    }
    else
    {
    }
}

void SpriteDemo::onMouseUp( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseUp(btnState, x, y);

    mInput.EventLBtnUp.fire(x, y);

    if( (btnState & MK_LBUTTON) != 0 )
    {
    }
    else if ( (btnState & MK_RBUTTON) != 0)
    {
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
        mInput.EventMouseWheelUp.fire(x, y);
    }
    else
    {
        mInput.EventMouseWheelDown.fire(x, y);
    }
}

