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
    mSpriteVB = 0;
    mSpriteGen = 0;
}

SpriteDemo::~SpriteDemo()
{
    safe_release(&mSpriteVB);
    if (mSpriteGen != 0)
    {
        delete mSpriteGen;
        mSpriteGen = 0;
    }
}

bool SpriteDemo::init()
{
    if (DemoBasic::init() == false)
    {
        return false;
    }

    mSpriteGen = new SpriteGenertaor(md3dDevice, md3dImmediateContext);
    // Z: [0, 1)
    Quad q1, q2;
    q1.mCenter = XMFLOAT3(400.f, 550.f, 0.1f);
    q1.mSize = XMFLOAT2(600.f, 60.f);
    q2.mCenter = XMFLOAT3(750.f, 300.f, 0.1f);
    q2.mSize = XMFLOAT2(60.f, 400.f);
    std::vector<Quad> qs;
    qs.push_back(q1);
    qs.push_back(q2);

    mSpriteGen->buildVBFromQuads(qs); 

    mFont.init();
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
    // Draw sprites
    //
    // Input layout is changed
    mSpriteGen->drawSprite();

    HR(mSwapChain->Present(0, 0));
}

