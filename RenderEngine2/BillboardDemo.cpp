#include "BillboardDemo.h"

#include "EffectMgr.h"
#include "InputLayoutMgr.h"
#include "RenderStateMgr.h"
#include "TextureMgr.h"

static const UINT TreeCount = 2;
ID3D11ShaderResourceView* createTexture2DArraySRV(
    ID3D11Device* device, ID3D11DeviceContext* context,
    std::vector<std::wstring>& filenames,
    DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
    UINT filter = D3DX11_FILTER_NONE, 
    UINT mipFilter = D3DX11_FILTER_LINEAR)
{
    //
    // Load the texture elements individually from file.  These textures
    // won't be used by the GPU (0 bind flags), they are just used to 
    // load the image data from file.  We use the STAGING usage so the
    // CPU can read the resource.
    //

    UINT size = filenames.size();

    std::vector<ID3D11Texture2D*> srcTex(size);
    for(UINT i = 0; i < size; ++i)
    {
        D3DX11_IMAGE_LOAD_INFO loadInfo;

        loadInfo.Width  = D3DX11_FROM_FILE;
        loadInfo.Height = D3DX11_FROM_FILE;
        loadInfo.Depth  = D3DX11_FROM_FILE;
        loadInfo.FirstMipLevel = 0;
        loadInfo.MipLevels = D3DX11_FROM_FILE;
        loadInfo.Usage = D3D11_USAGE_STAGING;
        loadInfo.BindFlags = 0;
        loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        loadInfo.MiscFlags = 0;
        loadInfo.Format = format;
        loadInfo.Filter = filter;
        loadInfo.MipFilter = mipFilter;
        loadInfo.pSrcInfo  = 0;

        HR(D3DX11CreateTextureFromFile(device, filenames[i].c_str(), 
            &loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
    }

    //
    // Create the texture array.  Each element in the texture 
    // array has the same format/dimensions.
    //

    D3D11_TEXTURE2D_DESC texElementDesc;
    srcTex[0]->GetDesc(&texElementDesc);

    D3D11_TEXTURE2D_DESC texArrayDesc;
    texArrayDesc.Width              = texElementDesc.Width;
    texArrayDesc.Height             = texElementDesc.Height;
    texArrayDesc.MipLevels          = texElementDesc.MipLevels;
    texArrayDesc.ArraySize          = size;
    texArrayDesc.Format             = texElementDesc.Format;
    texArrayDesc.SampleDesc.Count   = 1;
    texArrayDesc.SampleDesc.Quality = 0;
    texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
    texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texArrayDesc.CPUAccessFlags     = 0;
    texArrayDesc.MiscFlags          = 0;

    ID3D11Texture2D* texArray = 0;
    HR(device->CreateTexture2D( &texArrayDesc, 0, &texArray));

    //
    // Copy individual texture elements into texture array.
    //

    // for each texture element...
    for(UINT texElement = 0; texElement < size; ++texElement)
    {
        // for each mipmap level...
        for(UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
        {
            D3D11_MAPPED_SUBRESOURCE mappedTex2D;
            HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

            context->UpdateSubresource(texArray, 
                D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
                0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

            context->Unmap(srcTex[texElement], mipLevel);
        }
    }	

    //
    // Create a resource view to the texture array.
    //

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    viewDesc.Format = texArrayDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    viewDesc.Texture2DArray.MostDetailedMip = 0;
    viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
    viewDesc.Texture2DArray.FirstArraySlice = 0;
    viewDesc.Texture2DArray.ArraySize = size;

    ID3D11ShaderResourceView* texArraySRV = 0;
    HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

    //
    // Cleanup--we only need the resource view.
    //

    safe_release(&texArray);

    for(UINT i = 0; i < size; ++i)
        safe_release(&srcTex[i]);

    return texArraySRV;
}
BillboardDemo::BillboardDemo()
{
    //
    // Billboard
    //
    mTreeSpritesVB = 0;
    mTreeTextureMapArraySRV = 0;
    mAlphaToCoverageOn = true;
    mTreeMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mTreeMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mTreeMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

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
}

BillboardDemo::~BillboardDemo()
{
    md3dImmediateContext->ClearState();

    if (mpModel)
    {
        delete mpModel;
        mpModel = 0;
    }

    safe_release(&mTreeSpritesVB);
    safe_release(&mTreeTextureMapArraySRV);

    EffectMgr::destroyAll();
    InputLayoutMgr::destroyAll();
    RenderStateMgr::destroyAll();
    TextureMgr::destroyAll();
}

PCWSTR BillboardDemo::getClassName() const
{
    return L"BillboardDemo";
}

PCWSTR BillboardDemo::getWindowCaption() const
{
    return L"Billboard Demo";
}

bool BillboardDemo::init()
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

    // Load grid
    mpModel = new Model(md3dDevice, md3dImmediateContext);
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid;
    geoGen.CreateGrid(20.0f, 20.0f, 30, 30, grid);
    mpModel->loadPrimitive(grid);

    // Set grid position
    XMFLOAT4X4 world, texTransform;
    XMMATRIX scale = XMMatrixScaling(1.f, 1.f, 1.f);
    XMMATRIX translation = XMMatrixTranslation(0.f, 0.f, 0.f);
    XMStoreFloat4x4(&world, XMMatrixMultiply(scale, translation));
    XMStoreFloat4x4(&texTransform, XMMatrixIdentity());
    mpModel->update(world, texTransform);

    //
    // Create texture array for trees
    //
    std::vector<std::wstring> treeFilenames;
    treeFilenames.push_back(L"../Textures/tree0.dds");
    treeFilenames.push_back(L"../Textures/tree1.dds");
    treeFilenames.push_back(L"../Textures/tree2.dds");
    treeFilenames.push_back(L"../Textures/tree3.dds");
    mTreeTextureMapArraySRV = 
        createTexture2DArraySRV(md3dDevice, md3dImmediateContext, treeFilenames, DXGI_FORMAT_R8G8B8A8_UNORM);

    //
    // Create tree sprite vertices
    //
    Vertex::TreePointSprite v[TreeCount];

    for(UINT i = 0; i < TreeCount; ++i)
    {
        float x = MathHelper::RandF(-35.0f, 35.0f);
        float z = MathHelper::RandF(-35.0f, 35.0f);
        float y = 0.f;

        // Move tree slightly above land height.
        y += 10.0f;

        v[i].Pos  = XMFLOAT3(x,y,z);
        v[i].Size = XMFLOAT2(24.0f, 24.0f);
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::TreePointSprite) * TreeCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mTreeSpritesVB));

    return true;
}

void BillboardDemo::onResize()
{
    RenderCore::onResize();

    mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio(), 1.f, 1000.f);
}

void BillboardDemo::updateScene( float dt )
{
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

void BillboardDemo::drawScene()
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
    // Draw tree sprites
    //
    EffectMgr::TreeSpriteFX->SetDirLights(mDirLights);
    EffectMgr::TreeSpriteFX->SetEyePosW(mEyePosWorld);
    EffectMgr::TreeSpriteFX->SetFogColor(Colors::Silver);
    EffectMgr::TreeSpriteFX->SetFogStart(15.0f);
    EffectMgr::TreeSpriteFX->SetFogRange(175.0f);
    EffectMgr::TreeSpriteFX->SetViewProj(mCam.ViewProj());
    EffectMgr::TreeSpriteFX->SetMaterial(mTreeMat);
    EffectMgr::TreeSpriteFX->SetTreeTextureMapArray(mTreeTextureMapArraySRV);

    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    md3dImmediateContext->IASetInputLayout(InputLayoutMgr::TreePointSprite);
    UINT stride = sizeof(Vertex::TreePointSprite);
    UINT offset = 0;

    ID3DX11EffectTechnique* treeTech = EffectMgr::TreeSpriteFX->Light3TexAlphaClipTech;
    D3DX11_TECHNIQUE_DESC techDesc;
    treeTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mTreeSpritesVB, &stride, &offset);

        float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        if(mAlphaToCoverageOn)
        {
            md3dImmediateContext->OMSetBlendState(RenderStateMgr::AlphaToCoverageBS, blendFactor, 0xffffffff);
        }
        treeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->Draw(TreeCount, 0);

        md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
    }
    HR(mSwapChain->Present(0, 0));
}

void BillboardDemo::onMouseDown( WPARAM btnState, int x, int y )
{
    SetCapture(mHwnd);
}

void BillboardDemo::onMouseUp( WPARAM btnState, int x, int y )
{
    ReleaseCapture();
}

void BillboardDemo::onMouseMove( WPARAM btnState, int x, int y )
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