#include "ObjLoaderDemo.h"

#include "EffectMgr.h"
#include "InputLayoutMgr.h"
#include "RenderStateMgr.h"
#include "TextureMgr.h"

ObjLoaderDemo::ObjLoaderDemo()
{
    //
    // Model
    //
    mpModel = 0;

    //
    // Environmental Lights
    //
    mLightCount = 1;
    mEyePosWorld = XMFLOAT3(0.f, 0.f, 0.f); 

    mDirLights[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    mDirLights[0].Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

    mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
    mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
    mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

    mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

    //
    // User input
    //
    mLastMousePos.x = 0;
    mLastMousePos.y = 0;

    //
    // Camera
    //
    mCam.SetPosition(0.0f, 2.0f, -15.0f);


    //
    // RenderOptions
    //
    mRenderOptions = Textures;

}

ObjLoaderDemo::~ObjLoaderDemo()
{
    md3dImmediateContext->ClearState();

    if (mpModel)
    {
        delete mpModel;
        mpModel = 0;
    }

    EffectMgr::destroyAll();
    InputLayoutMgr::destroyAll();
    RenderStateMgr::destroyAll();
    TextureMgr::destroyAll();
}

bool ObjLoaderDemo::init()
{
    if(RenderCore::init() == false)
    {
        return false;
    }

    //
    // Must init Effects first since InputLayouts depend on shader signatures.
    //
    EffectMgr::initAll(md3dDevice);
    InputLayoutMgr::initAll(md3dDevice);
    RenderStateMgr::initAll(md3dDevice);
    
    TextureMgr::initAll(md3dDevice);
    TextureMgr::addPath(std::wstring(L"../Models/"));
    TextureMgr::addPath(std::wstring(L"../Textures/"));

    mpModel = new Model(md3dDevice, md3dImmediateContext);
    //mpModel->loadObj(L"../Models/cup.obj");
    mpModel->loadObj(L"../Models/clock_triangulate.obj", true);
    //mpModel->loadObj(L"../Models/PM3D_Cylinder3D.obj");

    XMFLOAT4X4 world, texTransform;
    XMMATRIX scale = XMMatrixScaling(1.f, 1.f, 1.f);
    XMMATRIX translation = XMMatrixTranslation(0.f, 0.f, 0.f);
    XMStoreFloat4x4(&world, XMMatrixMultiply(scale, translation));
    XMStoreFloat4x4(&texTransform, XMMatrixIdentity());
    mpModel->update(world, texTransform);

    return true;
}

void ObjLoaderDemo::drawScene()
{
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    md3dImmediateContext->IASetInputLayout(InputLayoutMgr::PosNormalTex);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Should be put into updateScene()
    mCam.UpdateViewMatrix();

    // Set per frame constants.
    EffectMgr::BasicFX->setDirLights(mDirLights);
    EffectMgr::BasicFX->setEyePosW(mEyePosWorld);
    EffectMgr::BasicFX->setFogColor(Colors::Silver);
    EffectMgr::BasicFX->setFogStart(15.0f);
    EffectMgr::BasicFX->setFogRange(175.0f);

    // Config techniques to use
    ID3DX11EffectTechnique* tech;
    switch(mRenderOptions)
    {
    case Lighting:
        tech = EffectMgr::BasicFX->mLight3Tech;
        break;
        
    case Textures:
        tech = EffectMgr::BasicFX->mLight3TexTech;
        break;

    case TexturesAndFog:
        tech = EffectMgr::BasicFX->mLight3TexFogTech;
        break;
    }

    XMFLOAT4X4 vp;
    XMStoreFloat4x4(&vp, mCam.ViewProj());
    mpModel->draw(tech, vp);

    HR(mSwapChain->Present(0, 0));
}

PCWSTR ObjLoaderDemo::getClassName() const
{
    return L"ObjLoaderDemo";
}

PCWSTR ObjLoaderDemo::getWindowCaption() const
{
    return L"Obj Loader Demo";
}

void ObjLoaderDemo::onResize()
{
    RenderCore::onResize();

    mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio(), 1.f, 1000.f);
}

void ObjLoaderDemo::updateScene( float dt )
{

    //
    // Switch the render mode based in key input.
    //
    if( GetAsyncKeyState('1') & 0x8000 )
        mRenderOptions = Lighting; 

    if( GetAsyncKeyState('2') & 0x8000 )
        mRenderOptions = Textures; 

    if( GetAsyncKeyState('3') & 0x8000 )
        mRenderOptions = TexturesAndFog; 

    //
    // Control the camera.
    //
    if( GetAsyncKeyState('W') & 0x8000 )
        mCam.Walk(20.0f*dt);

    if( GetAsyncKeyState('S') & 0x8000 )
        mCam.Walk(-20.0f*dt);

    if( GetAsyncKeyState('A') & 0x8000 )
        mCam.Strafe(-20.0f*dt);

    if( GetAsyncKeyState('D') & 0x8000 )
        mCam.Strafe(20.0f*dt);

    // The mEyePosWorld is then updated to shader program as 
    // eye position is required in light computation.
    mEyePosWorld = mCam.GetPosition();
}

void ObjLoaderDemo::onMouseDown( WPARAM btnState, int x, int y )
{
    SetCapture(mHwnd);
}

void ObjLoaderDemo::onMouseUp( WPARAM btnState, int x, int y )
{
    ReleaseCapture();
}

void ObjLoaderDemo::onMouseMove( WPARAM btnState, int x, int y )
{
    if( (btnState & MK_RBUTTON) != 0 )
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

        mCam.Pitch(dy);
        mCam.RotateY(dx);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}
