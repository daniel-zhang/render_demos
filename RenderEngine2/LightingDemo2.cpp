#include "LightingDemo2.h"

#include "EffectMgr.h"
#include "InputLayoutMgr.h"
#include "RenderStateMgr.h"

LightingDemo2::LightingDemo2()
{
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
    mRenderOptions = TexturesAndFog;

    //
    // Skull
    //
    mSkullVB = 0; 
    mSkullIB = 0; 
    mSkullIndexCount = 0;

    XMMATRIX skullScale = XMMatrixScaling(1.f, 1.f, 1.f);
    XMMATRIX skullTranslation = XMMatrixTranslation(0.f, 10.f, 0.f);
    XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullTranslation));

    mSkullMaterial.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    mSkullMaterial.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    mSkullMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

    //
    // Cigar Pack
    //
    mCigVB = 0;
    mCigIB = 0;
    mCigVertexOffset = 0;
    mCigIndexOffset = 0;
    mCigIndexCount = 0;
    mCigMapSRV = 0;

    XMMATRIX cigScale = XMMatrixScaling(1.f, 1.f, 1.f);
    XMMATRIX cigTranslation = XMMatrixTranslation(-10.f, 15.f, 10.f);
    XMStoreFloat4x4(&mCigWorld, XMMatrixMultiply(cigScale, cigTranslation));
    mCigMaterial.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    mCigMaterial.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    mCigMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 128.0f);


    //
    // Box
    //
    mBoxVB = 0;
    mBoxIB = 0;
    mBoxVertexOffset = 0;
    mBoxIndexOffset = 0;
    mBoxIndexCount = 0;
    mBoxMapSRV = 0;

    XMMATRIX boxScale = XMMatrixScaling(15.f, 15.f, 15.f);
    XMMATRIX boxTranslation = XMMatrixTranslation(8.f, 5.f, -15.f);
    XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxTranslation));

    mBoxMaterial.Ambient  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    mBoxMaterial.Diffuse  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    mBoxMaterial.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    //
    // Land/Terrain
    //
    mLandVB = 0;
    mLandIB = 0;
    mLandIndexCount = 0;
    mGrassMapSRV = 0;

    ;

    XMStoreFloat4x4(&mLandWorld, XMMatrixIdentity());

    XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
    XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);

    mLandMaterial.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mLandMaterial.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mLandMaterial.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    //
    // Waves
    //
    mWavesVB = 0;
    mWavesIB = 0;
    mWavesMapSRV = 0; 
    mWaterTexOffset = XMFLOAT2(0.f, 0.f);

    XMStoreFloat4x4(&mWavesWorld, XMMatrixIdentity());

    mWavesMaterial.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mWavesMaterial.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
    mWavesMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

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
}

LightingDemo2::~LightingDemo2()
{
    md3dImmediateContext->ClearState();

    safe_release(&mSkullVB);
    safe_release(&mSkullIB);
    safe_release(&mCigVB);
    safe_release(&mCigIB);
    safe_release(&mBoxVB);
    safe_release(&mBoxIB);
    safe_release(&mWavesVB);
    safe_release(&mWavesIB);
    safe_release(&mLandVB);
    safe_release(&mLandIB);
    safe_release(&mGrassMapSRV);
    safe_release(&mBoxMapSRV);
    safe_release(&mWavesMapSRV);
    safe_release(&mCigMapSRV);

    EffectMgr::destroyAll();
    InputLayoutMgr::destroyAll();
    RenderStateMgr::destroyAll();
}

PCWSTR LightingDemo2::getClassName() const
{
    return L"LightingDemo2";
}

PCWSTR LightingDemo2::getWindowCaption() const
{
    return L"Lighting Demo 2";
}

bool LightingDemo2::init()
{
    if (RenderCore::init() == false)
    {
        return false;
    }

    mWaves.Init(160, 160, 1.0f, 0.03f, 5.0f, 0.3f);

    // Must init Effects first since InputLayouts depend on shader signatures.
    EffectMgr::initAll(md3dDevice);
    InputLayoutMgr::initAll(md3dDevice);
    RenderStateMgr::initAll(md3dDevice);

    HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"../Textures/grass.dds", 0, 0, &mGrassMapSRV, 0 ));
    HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"../Textures/water2.dds", 0, 0, &mWavesMapSRV, 0 ));
    HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"../Textures/WireFence.dds", 0, 0, &mBoxMapSRV, 0 ));
    HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"../Textures/cig_texture2.dds", 0, 0, &mCigMapSRV, 0 ));

    //buildSceneBuffers();
    buildSkull();
    buildCigar();
    buildBox();
    buildLand();
    buildWave();

    return true;
}

void LightingDemo2::onResize()
{
    RenderCore::onResize();

    mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio(), 1.f, 1000.f);
}

void LightingDemo2::updateScene( float dt )
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

    //
    // Every 0.1 second, generate a wave with random strength(0.1, 2) .
    //
    static float t_base = 0.0f;
    if( (mTimer.totalTime() - t_base) >= 0.1f )
    {
        t_base += 0.1f;

        DWORD i = 5 + rand() % (mWaves.RowCount()-10);
        DWORD j = 5 + rand() % (mWaves.ColumnCount()-10);

        float r = MathHelper::RandF(0.1f, 2.0f);

        mWaves.Disturb(i, j, r);
    }
    mWaves.Update(dt);

    //
    // Update the wave vertex buffer with the new solution.
    //
    D3D11_MAPPED_SUBRESOURCE mappedData;
    HR(md3dImmediateContext->Map(mWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

    Vertex::PosNormalTex* v = reinterpret_cast<Vertex::PosNormalTex*>(mappedData.pData);
    for(UINT i = 0; i < mWaves.VertexCount(); ++i)
    {
        v[i].Pos    = mWaves[i];
        v[i].Normal = mWaves.Normal(i);

        // Derive tex-coords in [0,1] from position.
        // Note that UV's originate is at upper-left corner
        v[i].Tex.x  = 0.5f + mWaves[i].x / mWaves.Width();
        v[i].Tex.y  = 0.5f - mWaves[i].z / mWaves.Depth();
    }

    md3dImmediateContext->Unmap(mWavesVB, 0);

    //
    // Animate water texture coordinates.
    //

    // Tile water texture.
    XMMATRIX wavesScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

    // Translate texture over time.
    mWaterTexOffset.y += 0.05f*dt;
    mWaterTexOffset.x += 0.1f*dt;	
    XMMATRIX wavesOffset = XMMatrixTranslation(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);

    // Combine scale and translation.
    XMStoreFloat4x4(&mWaterTexTransform, wavesScale*wavesOffset);

}

void LightingDemo2::drawScene()
{
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    md3dImmediateContext->IASetInputLayout(InputLayoutMgr::PosNormalTex);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

    UINT stride = sizeof(Vertex::PosNormalTex);
    UINT offset = 0;

    mCam.UpdateViewMatrix();
    XMMATRIX view  = mCam.View();
    XMMATRIX proj  = mCam.Proj();
    // A viewProj matrix is used to save the computation
    XMMATRIX viewProj = mCam.ViewProj();

    // Set per frame constants.
    EffectMgr::BasicFX->setDirLights(mDirLights);
    EffectMgr::BasicFX->setEyePosW(mEyePosWorld);
    EffectMgr::BasicFX->setFogColor(Colors::Silver);
    EffectMgr::BasicFX->setFogStart(15.0f);
    EffectMgr::BasicFX->setFogRange(175.0f);

    // Config techniques to use
    ID3DX11EffectTechnique* woodBoxTech;
    ID3DX11EffectTechnique* fenceBoxTech;
    ID3DX11EffectTechnique* landAndWavesTech;
    ID3DX11EffectTechnique* skullTech;
    ID3DX11EffectTechnique* cigTech;

    switch(mRenderOptions)
    {
    case Lighting:
        fenceBoxTech = EffectMgr::BasicFX->mLight3Tech;
        landAndWavesTech = EffectMgr::BasicFX->mLight3Tech;
        woodBoxTech = 0;
        skullTech = EffectMgr::BasicFX->mLight3Tech;
        cigTech = EffectMgr::BasicFX->mLight3Tech;
        break;

    case Textures:
        fenceBoxTech = EffectMgr::BasicFX->mLight3TexAlphaClipTech;
        landAndWavesTech = EffectMgr::BasicFX->mLight3TexTech;
        woodBoxTech = 0;
        skullTech = EffectMgr::BasicFX->mLight3Tech;
        cigTech = EffectMgr::BasicFX->mLight3TexTech;
        break;

    case TexturesAndFog:
        fenceBoxTech = EffectMgr::BasicFX->mLight3TexAlphaClipFogTech;
        landAndWavesTech = EffectMgr::BasicFX->mLight3TexFogTech;
        woodBoxTech = 0;
        skullTech = EffectMgr::BasicFX->mLight3Tech;
        cigTech = EffectMgr::BasicFX->mLight3TexFogTech;
        break;
    }

    D3DX11_TECHNIQUE_DESC techDesc;

    //
    // Draw the skull with lights only
    //
    skullTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
        md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

        // Set per object constants.
        XMMATRIX world = XMLoadFloat4x4(&mSkullWorld);
        XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
        XMMATRIX worldViewProj = world*view*proj;

        EffectMgr::BasicFX->setWorld(world);
        EffectMgr::BasicFX->setWorldInvTranspose(worldInvTranspose);
        EffectMgr::BasicFX->setWorldViewProj(worldViewProj);
        EffectMgr::BasicFX->setTexTransform(XMMatrixIdentity());
        EffectMgr::BasicFX->setMaterial(mSkullMaterial);

        skullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
    }

    cigTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mCigVB, &stride, &offset);
        md3dImmediateContext->IASetIndexBuffer(mCigIB, DXGI_FORMAT_R32_UINT, 0);
        XMMATRIX world = XMLoadFloat4x4(&mCigWorld);
        XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
        XMMATRIX worldViewProj = world*view*proj;

        EffectMgr::BasicFX->setWorld(world);
        EffectMgr::BasicFX->setWorldInvTranspose(worldInvTranspose);
        EffectMgr::BasicFX->setWorldViewProj(worldViewProj);
        EffectMgr::BasicFX->setTexTransform(XMMatrixIdentity());
        EffectMgr::BasicFX->setMaterial(mCigMaterial);
        EffectMgr::BasicFX->setDiffuseMap(mCigMapSRV);

        cigTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(36, 0, 0);
    }

    //
    // Draw the box with alpha clipping.
    // 
    fenceBoxTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
        md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

        // Set per object constants.
        XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);
        XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
        XMMATRIX worldViewProj = world*view*proj;

        EffectMgr::BasicFX->setWorld(world);
        EffectMgr::BasicFX->setWorldInvTranspose(worldInvTranspose);
        EffectMgr::BasicFX->setWorldViewProj(worldViewProj);
        EffectMgr::BasicFX->setTexTransform(XMMatrixIdentity());
        EffectMgr::BasicFX->setMaterial(mBoxMaterial);
        EffectMgr::BasicFX->setDiffuseMap(mBoxMapSRV);

        md3dImmediateContext->RSSetState(RenderStateMgr::NoCullRS);
        fenceBoxTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(36, 0, 0);

        // Restore default render state.
        // md3dImmediateContext->RSSetState(0);
    }

    //
    // Draw the hills and water with texture and fog (no alpha clipping needed).
    //

    landAndWavesTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        //
        // Draw the hills.
        //
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mLandVB, &stride, &offset);
        md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);

        // Set per object constants.
        XMMATRIX world = XMLoadFloat4x4(&mLandWorld);
        XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
        XMMATRIX worldViewProj = world*view*proj;

        EffectMgr::BasicFX->setWorld(world);
        EffectMgr::BasicFX->setWorldInvTranspose(worldInvTranspose);
        EffectMgr::BasicFX->setWorldViewProj(worldViewProj);
        EffectMgr::BasicFX->setTexTransform(XMLoadFloat4x4(&mGrassTexTransform));
        EffectMgr::BasicFX->setMaterial(mLandMaterial);
        EffectMgr::BasicFX->setDiffuseMap(mGrassMapSRV);

        landAndWavesTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);

        //
        // Draw the waves.
        //
        md3dImmediateContext->IASetVertexBuffers(0, 1, &mWavesVB, &stride, &offset);
        md3dImmediateContext->IASetIndexBuffer(mWavesIB, DXGI_FORMAT_R32_UINT, 0);

        // Set per object constants.
        world = XMLoadFloat4x4(&mWavesWorld);
        worldInvTranspose = MathHelper::InverseTranspose(world);
        worldViewProj = world*view*proj;

        EffectMgr::BasicFX->setWorld(world);
        EffectMgr::BasicFX->setWorldInvTranspose(worldInvTranspose);
        EffectMgr::BasicFX->setWorldViewProj(worldViewProj);
        EffectMgr::BasicFX->setTexTransform(XMLoadFloat4x4(&mWaterTexTransform));
        EffectMgr::BasicFX->setMaterial(mWavesMaterial);
        EffectMgr::BasicFX->setDiffuseMap(mWavesMapSRV);

        md3dImmediateContext->OMSetBlendState(RenderStateMgr::TransparentBS, blendFactor, 0xffffffff);
        landAndWavesTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(3*mWaves.TriangleCount(), 0, 0);

        // Restore default blend state
        md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
        // Restore default render state.
        md3dImmediateContext->RSSetState(0);
    }


    HR(mSwapChain->Present(0, 0));
}

void LightingDemo2::onMouseDown( WPARAM btnState, int x, int y )
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture(mHwnd);
}

void LightingDemo2::onMouseUp( WPARAM btnState, int x, int y )
{
    ReleaseCapture();
}

void LightingDemo2::onMouseMove( WPARAM btnState, int x, int y )
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

void LightingDemo2::buildSkull()
{
    //
    // Load the skull mesh from file and create related VB and IB for it
    // For reference, see MeshLoaderDemo
    //
    std::ifstream fin("../Models/skull.txt");

    if(!fin)
    {
        MessageBox(0, L"../Models/skull.txt not found.", 0, 0);
        return;
    }

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    std::vector<Vertex::PosNormalTex> skullVertices(vcount);
    for(UINT i = 0; i < vcount; ++i)
    {
        fin >> skullVertices[i].Pos.x >> skullVertices[i].Pos.y >> skullVertices[i].Pos.z;
        fin >> skullVertices[i].Normal.x >> skullVertices[i].Normal.y >> skullVertices[i].Normal.z;
        // Tex-coord is not contained in mesh data
        skullVertices[i].Tex.x = 0.f;
        skullVertices[i].Tex.y = 0.f;
    }

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    mSkullIndexCount = 3*tcount;
    std::vector<UINT> skullIndices(mSkullIndexCount);
    for(UINT i = 0; i < tcount; ++i)
    {
        fin >> skullIndices[i*3+0] >> skullIndices[i*3+1] >> skullIndices[i*3+2];
    }

    fin.close();

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * vcount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &skullVertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSkullVB));

    //
    // Pack the indices of all the meshes into one index buffer.
    //
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mSkullIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &skullIndices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSkullIB));
}


void LightingDemo2::buildCigar()
{
    GeometryGenerator::MeshData cig;
    GeometryGenerator geoGen;
    geoGen.CreateCigarPack(15.f, cig);

    std::vector<Vertex::PosNormalTex> vertices(cig.Vertices.size());
    for (UINT i = 0; i < cig.Vertices.size(); ++i)
    {
        vertices[i].Pos    = cig.Vertices[i].Position;
        vertices[i].Normal = cig.Vertices[i].Normal;
        vertices[i].Tex    = cig.Vertices[i].TexC;
    }
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * cig.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mCigVB));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * cig.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &cig.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mCigIB));
}

void LightingDemo2::buildBox()
{
    GeometryGenerator::MeshData box;

    GeometryGenerator geoGen;
    geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

    //
    // Extract the vertex elements we are interested in and pack the
    // vertices of all the meshes into one vertex buffer.
    //
    std::vector<Vertex::PosNormalTex> vertices(box.Vertices.size());

    for(UINT i = 0; i < box.Vertices.size(); ++i)
    {
        vertices[i].Pos    = box.Vertices[i].Position;
        vertices[i].Normal = box.Vertices[i].Normal;
        vertices[i].Tex    = box.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * box.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &box.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}

void LightingDemo2::buildLand()
{
    GeometryGenerator::MeshData grid;

    GeometryGenerator geoGen;

    geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

    mLandIndexCount = grid.Indices.size();

    //
    // Extract the vertex elements we are interested and apply the height function to
    // each vertex.  
    //
    std::vector<Vertex::PosNormalTex> vertices(grid.Vertices.size());
    for(UINT i = 0; i < grid.Vertices.size(); ++i)
    {
        XMFLOAT3 p = grid.Vertices[i].Position;

        p.y = getHillHeight(p.x, p.z);

        vertices[i].Pos    = p;
        vertices[i].Normal = getHillNormal(p.x, p.z);
        vertices[i].Tex    = grid.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * grid.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mLandIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &grid.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
}

void LightingDemo2::buildWave()
{
    // Create the vertex buffer.  Note that we allocate space only, as
    // we will be updating the data every time step of the simulation.
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * mWaves.VertexCount();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVB));

    // Create the index buffer.  The index buffer is fixed, so we only 
    // need to create and set once.
    std::vector<UINT> indices(3*mWaves.TriangleCount()); // 3 indices per face

    // Iterate over each quad.
    UINT m = mWaves.RowCount();
    UINT n = mWaves.ColumnCount();
    int k = 0;
    for(UINT i = 0; i < m-1; ++i)
    {
        for(DWORD j = 0; j < n-1; ++j)
        {
            indices[k]   = i*n+j;
            indices[k+1] = i*n+j+1;
            indices[k+2] = (i+1)*n+j;

            indices[k+3] = (i+1)*n+j;
            indices[k+4] = i*n+j+1;
            indices[k+5] = (i+1)*n+j+1;

            k += 6; // next quad
        }
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWavesIB));
}

float LightingDemo2::getHillHeight( float x, float z ) const
{
    return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

XMFLOAT3 LightingDemo2::getHillNormal( float x, float z ) const
{
    // n = (-df/dx, 1, -df/dz)
    XMFLOAT3 n(
        -0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
        1.0f,
        -0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

    XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
    XMStoreFloat3(&n, unitNormal);

    return n;
}

