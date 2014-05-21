/*
Notes
1. represent a quad defined in screen space
2. transform that quad into dynamic vertex buffer so it can be updated
3. add a shader to render quad 
*/
#include "SpriteDemo.h"
#include "EffectMgr.h"
#include "RenderStateMgr.h"
#include "Color.h"


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
    // New Renderer Test Begin
    mSpriteRenderer.init(md3dDevice, md3dImmediateContext);
    mSprite.init(mSpriteRenderer.getRenderEnv());
    mSprite.setEffect(EffectMgr::OverlayFX);
    mSprite.setTechique(EffectMgr::OverlayFX->OverlayTech);
    mSprite.resize(Area2D(200, 200));
    mSprite.setColor(RGBAColor::Background);
    mSprite.setRSState(RenderStateMgr::ScissorRS);
    mSprite.setClipBox(Box2D(Point2D(0,0), Area2D(50,50)));
    //mSprite.enableClip();
    // New Renderer Test End
    */

    FontEngine fontEngine;
    fontEngine.createFontSheet(md3dDevice, mFontSheet, L"debug");
    mWidgetMgr.init(&mInput, md3dDevice, md3dImmediateContext);

for (UINT i = 0; i < 3; ++i)
{
    for (UINT j = 0; j < 5; ++j)
    {
        RGBA bgColor;
        switch(j%3)
        {
        case 0:
            bgColor = RGBAColor::Red;
            break;
        case 1:
            bgColor = RGBAColor::Green;
            break;
        case 2:
            bgColor = RGBAColor::Blue;
            break;
        }
        Text* t = new Text(mWidgetMgr.getRenderEnv(), 30, &mFontSheet, Point2D(355*i, 40*j), Area2D(350, 35), PixelPadding(), PixelMargin(), WIDGET_LAYOUT_STATIC);
        t->setText(std::wstring(L"The One Ring that Rules All !!"));
        t->mBgColor = bgColor;
        t->mActiveBgColor = bgColor;
        mWidgetMgr.addWidget(t);
    }
}
 
    mInfoBox = new Text(mWidgetMgr.getRenderEnv(), 20, &mFontSheet, Point2D(250, 150), Area2D(300, 200), PixelPadding(), PixelMargin(), WIDGET_LAYOUT_STATIC);

    std::wostringstream out;
    out << L"Total Widgets: " << 100 << "\nSynced Widgets: " << 32 << std::endl; 
    mInfoBox->setText(out.str());

    mWidgetMgr.addWidget(mInfoBox);

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

    std::wostringstream out;
    out << L"Total Widgets: " << 123 << L"\nSynced Widgets: " << 111 << std::endl
        << L"Time: " << mTimer.totalTime();
    mInfoBox->setText(out.str());

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

