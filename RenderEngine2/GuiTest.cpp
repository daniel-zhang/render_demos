#include "GuiTest.h"
#include "EffectMgr.h"
#include "RenderStateMgr.h"

GUITest::GUITest()
{
    mIsBtnDown = false;
}

GUITest::~GUITest()
{
    for(UINT i  =0; i < mSprites.size(); ++i)
    {
        delete mSprites[i];
    }
}

bool GUITest::init()
{
    if (!DemoBasic::init())
        return false;

    mClipBox = Box2D(Point2D(10, 10), Area2D(300, 300));

    mRenderer.init(md3dDevice, md3dImmediateContext);
    for (UINT i = 0; i < 3; ++i)
    {
        Sprite2D* s = new Sprite2D();
        mSprites.push_back(s);
        RGBA color;
        switch(i%3)
        {
        case 0:
            color = RGBAColor::Red;
            break;
        case 1:
            color = RGBAColor::Green;
            break;
        case 2:
            color = RGBAColor::Blue;
            break;
        }
        s->init(
        mRenderer.getRenderEnv(), 
        Point2D(4*i, 3*i), Area2D(60, 60), color,
        EffectMgr::OverlayFX, EffectMgr::OverlayFX->OverlayTech);
        s->setClipBox(&mClipBox);
        s->enableClip();
    }
    return true;
}

void GUITest::drawScene()
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

    md3dImmediateContext->OMSetDepthStencilState(RenderStateMgr::NoDepthTestDS, 0);

    for (UINT i = 0; i < mSprites.size(); ++i)
    {
        mRenderer.draw(*mSprites[i]);
    }

    md3dImmediateContext->OMSetDepthStencilState(0, 0);

    HR(mSwapChain->Present(0, 0));
}

void GUITest::onResize()
{
    DemoBasic::onResize();

    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);
    Area2D vpSize = Area2D(static_cast<int>(vp.Width), static_cast<int>(vp.Height));
    mRenderer.onViewportResize(vpSize);

}

void GUITest::onMouseMove( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseMove( btnState, x, y );

    Point2D curPos(x, y);
    if(mIsBtnDown)
    {
        for (UINT i = 0; i < mSprites.size(); ++i)
        {
            mSprites[i]->move(static_cast<Vector2D>(curPos - mLasMousePos));
        }
    }
    mLasMousePos = curPos;
}

void GUITest::onMouseDown( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseDown( btnState, x, y );
    mIsBtnDown = true;
}

void GUITest::onMouseUp( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseUp( btnState, x, y );
    mIsBtnDown = false;
}

void GUITest::onMouseWheel( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseWheel( btnState, x, y );
}
