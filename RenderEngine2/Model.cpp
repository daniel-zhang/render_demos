#include "Model.h"
#include "MathHelper.h"
#include <algorithm>

#include "EffectMgr.h"
#include "ObjLoader.h"
#include "TextureMgr.h"

ObjLoader gObjLoader;

Model::Model( ID3D11Device* dv, ID3D11DeviceContext* ctx ) 
{
    mDevice = dv;
    mCtx = ctx;
    mVB = 0;
    mIB = 0;

    XMStoreFloat4x4(&mWorld, XMMatrixTranslation(0.f, 0.f, 0.f));
    XMStoreFloat4x4(&mTexTransform, XMMatrixIdentity());
}

Model::~Model()
{
    safe_release(&mVB);
    safe_release(&mIB);
    mSubsets.clear();
    mMaterials.clear();
    mVertices.clear();
    mIndices.clear();
}

// By default, convert from .obj's right-hand system to d3d11 left-hand
void Model::loadObj( std::wstring filename, bool coordConvert)
{
    //
    // Load mesh data
    //
    if ( gObjLoader.load(filename, coordConvert)  == false)
    {
        return;
    }

    // Get vertex data from loader
    for (UINT i = 0; i < gObjLoader.mVertices.size(); ++i)
    {
        Vertex::PosNormalTex vertex;

        vertex.Pos = gObjLoader.mVertices[i].pos;
        vertex.Normal = gObjLoader.mVertices[i].norm;
        vertex.Tex = gObjLoader.mVertices[i].texcd;

        mVertices.push_back(vertex);
    }

    // Get indices from loader
    // std::copy(gObjLoader.mIndices.begin(), gObjLoader.mIndices.end(), mIndices.begin());
    for (UINT i = 0; i < gObjLoader.mIndices.size(); ++i)
    {
        mIndices.push_back(gObjLoader.mIndices[i]); 
    }

    // Get materials and diffuse maps from loader 
    // TODO: Currently mats and texs are set separately at pipeline level, not very smart.
    for (UINT i = 0; i < gObjLoader.mMaterials.size(); ++i)
    {
        Material mat;
        ObjLoader::ObjMaterial& objMat = gObjLoader.mMaterials[i];

        mat.Ambient = objMat.ambient;
        mat.Diffuse = objMat.diffuse;
        mat.Specular = objMat.specular;
        if (objMat.useSpecular == true)
        {
            mat.Specular.w = static_cast<float>(objMat.specPower);
        }
        mMaterials.push_back(mat);

        // Add a placeholder if the material has no texture attached
        ID3D11ShaderResourceView* mapSRV = 0;
        if (objMat.diffuseMapFileName.compare(L"") != 0)
        {
            mapSRV = TextureMgr::getTexture(objMat.diffuseMapFileName);
        }
        mDiffuseMaps.push_back(mapSRV);
    }

    // Get subsets from loader
    for (UINT i = 0; i < gObjLoader.mSubsets.size(); ++i)
    {
        ModelSubset s;
        s.matId = gObjLoader.mSubsets[i].matId;
        s.indexCount = gObjLoader.mSubsets[i].indexCount;
        s.indexOffset= gObjLoader.mSubsets[i].indexOffset;

        mSubsets.push_back(s);
    }

    // Done with ObjLoader
    gObjLoader.clear();

    //
    // Create VB IB
    //
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * mVertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &mVertices[0];
    HR(mDevice->CreateBuffer(&vbd, &vinitData, &mVB));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mIndices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &mIndices[0];
    HR(mDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Model::loadPrimitive( GeometryGenerator::MeshData& mesh )
{
    // Get vertex
    Vertex::PosNormalTex vertex;
    for (UINT i = 0; i < mesh.Vertices.size(); ++i)
    {
        ZeroMemory(&vertex, sizeof(vertex));
        vertex.Pos = mesh.Vertices[i].Position;
        vertex.Normal = mesh.Vertices[i].Normal;
        vertex.Tex = mesh.Vertices[i].TexC;

        mVertices.push_back(vertex);
    }

    // Get index
    for (UINT i = 0; i < mesh.Indices.size(); ++i)
    {
        mIndices.push_back(mesh.Indices[i]);
    }

    // Set the default material, texture(null) and subset
    Material m;
    m.Ambient  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    m.Diffuse  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    m.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
    mMaterials.push_back(m);

    ID3D11ShaderResourceView* mapSRV = 0;
    mDiffuseMaps.push_back(mapSRV);

    ModelSubset s;
    s.matId  = 0;
    s.indexOffset = 0;
    s.indexCount = mIndices.size();
    mSubsets.push_back(s);

    //
    // Create VB IB
    //
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::PosNormalTex) * mVertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &mVertices[0];
    HR(mDevice->CreateBuffer(&vbd, &vinitData, &mVB));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mIndices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &mIndices[0];
    HR(mDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Model::loadSimpleMesh( std::wstring filename )
{

}

void Model::update( XMFLOAT4X4 world, XMFLOAT4X4 texTransform )
{
    mWorld = world;
    mTexTransform = texTransform;
}

void Model::draw( ID3DX11EffectTechnique* pTech, XMFLOAT4X4 viewProj )
{
    UINT stride = sizeof(Vertex::PosNormalTex);
    UINT offset = 0;

    // Set per object constants.
    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
    XMMATRIX vp = XMLoadFloat4x4(&viewProj);
    XMMATRIX worldViewProj = world*vp;

    EffectMgr::BasicFX->setWorld(world);
    EffectMgr::BasicFX->setWorldInvTranspose(worldInvTranspose);
    EffectMgr::BasicFX->setWorldViewProj(worldViewProj);
    EffectMgr::BasicFX->setTexTransform(XMLoadFloat4x4(&mTexTransform));

    mCtx->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    mCtx->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
    
    // Render each subset
    for (UINT i = 0; i < mSubsets.size(); ++i)
    {
        D3DX11_TECHNIQUE_DESC techDesc;
        // select tech
        ID3DX11EffectTechnique* tech; 
        ID3D11ShaderResourceView* mapSRV = mDiffuseMaps[mSubsets[i].matId];
        if (mapSRV)
            tech = EffectMgr::BasicFX->mLight3TexTech;
        else
            tech = EffectMgr::BasicFX->mLight3Tech;

        tech->GetDesc(&techDesc);
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {

            EffectMgr::BasicFX->setMaterial(mMaterials[mSubsets[i].matId]);
            EffectMgr::BasicFX->setDiffuseMap(mapSRV);

            tech->GetPassByIndex(p)->Apply(0, mCtx);
            mCtx->DrawIndexed(mSubsets[i].indexCount, mSubsets[i].indexOffset, 0);
        }
    }
}
