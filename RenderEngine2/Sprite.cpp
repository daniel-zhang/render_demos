#include "Sprite.h"
#include "EffectMgr.h"

SpriteGenertaor::SpriteGenertaor( ID3D11Device* dv, ID3D11DeviceContext* ctx)
    :mDevice(dv), mCtx(ctx)
{
    mSpriteUpdated = false;
}

void SpriteGenertaor::buildVBFromQuads( std::vector<Quad>& quads)
{
    // Keep a local copy of quads data
    mQuads.resize(quads.size());
    std::copy(quads.begin(), quads.end(), mQuads.begin());

    // Create dynamic vb and static ib
    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof(Vertex::TreePointSprite) * quads.size() * 4;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    vbd.Usage = D3D11_USAGE_DYNAMIC; // make dynamic
    HR(mDevice->CreateBuffer(&vbd, 0, &mVB));

    std::vector<UINT> indices(quads.size() * 6, 0);
    for (UINT i = 0; i < quads.size(); ++i)
    {
        indices[i*6+0] = i*4+0;
        indices[i*6+1] = i*4+1;
        indices[i*6+2] = i*4+2;
        indices[i*6+3] = i*4+0;
        indices[i*6+4] = i*4+2;
        indices[i*6+5] = i*4+3;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(UINT) * quads.size() * 6;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA idata;
    idata.pSysMem = &indices[0];
    HR(mDevice->CreateBuffer(&ibd, &idata, &mIB));
}

XMFLOAT3 SpriteGenertaor::screenToNDC( XMFLOAT3 srcPos )
{
    XMFLOAT3 p;
    p.x = 2.f * srcPos.x / mVPSize.x - 1.f;
    p.y = 1.f - 2.f * srcPos.y / mVPSize.y;
    p.z = srcPos.z;

    return p;
}

// Rebuild VB and IB on resize
void SpriteGenertaor::drawSprite()
{
    D3D11_VIEWPORT vp;
    UINT vpNum = 1;
    mCtx->RSGetViewports(&vpNum, &vp);
    mVPSize.x = vp.Width;
    mVPSize.y = vp.Height;

    std::vector<Vertex::TreePointSprite> vertices;


    for (UINT i = 0; i < mQuads.size(); ++i)
    {
        // In screen space: 
        // upper-left as origin, left as positive x, down as positive y
        float halfW = mQuads[i].mSize.x*0.5f;
        float halfH = mQuads[i].mSize.y*0.5f;

        XMFLOAT3 s0 = XMFLOAT3(mQuads[i].mCenter.x - halfW, mQuads[i].mCenter.y - halfH, mQuads[i].mCenter.z);
        XMFLOAT3 s1 = XMFLOAT3(mQuads[i].mCenter.x + halfW, mQuads[i].mCenter.y - halfH, mQuads[i].mCenter.z);
        XMFLOAT3 s2 = XMFLOAT3(mQuads[i].mCenter.x + halfW, mQuads[i].mCenter.y + halfH, mQuads[i].mCenter.z);
        XMFLOAT3 s3 = XMFLOAT3(mQuads[i].mCenter.x - halfW, mQuads[i].mCenter.y + halfH, mQuads[i].mCenter.z);

        // to NDC space
        Vertex::TreePointSprite v0, v1, v2, v3;
        v0.Pos = screenToNDC(s0);
        v1.Pos = screenToNDC(s1);
        v2.Pos = screenToNDC(s2);
        v3.Pos = screenToNDC(s3);

        // TODO: rotate, scale quad
        XMVECTOR scaling = XMVectorSet(1.f, 1.f, 1.0f, 0.0f);
        XMVECTOR origin = XMVectorSet(0.f, 0.f, 0.0f, 0.0f);
        XMVECTOR translation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMMATRIX T = XMMatrixAffineTransformation2D(scaling, origin, 0.f, translation);

        // Rotate and scale the quad in NDC space.
        XMVECTOR p = XMLoadFloat3(&v0.Pos);
        XMVECTOR m = XMVector3TransformCoord(p, T);
        XMStoreFloat3(&v0.Pos, m);

        p = XMLoadFloat3(&v1.Pos);
        m = XMVector3TransformCoord(p, T);
        XMStoreFloat3(&v1.Pos, p);

        p = XMLoadFloat3(&v2.Pos);
        m = XMVector3TransformCoord(p, T);
        XMStoreFloat3(&v2.Pos, p);

        p = XMLoadFloat3(&v3.Pos);
        m = XMVector3TransformCoord(p, T);
        XMStoreFloat3(&v3.Pos, p);

        // Turn 1 quad into 2 triangles
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }

    // Map
    D3D11_MAPPED_SUBRESOURCE mappedData;
    mCtx->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    Vertex::TreePointSprite* vStart = reinterpret_cast<Vertex::TreePointSprite*>(mappedData.pData);
    for(UINT i = 0; i < vertices.size(); ++i)
    {
        vStart[i] = vertices[i];
    }
    mCtx->Unmap(mVB, 0);

    mCtx->IASetInputLayout(InputLayoutMgr::TreePointSprite);
    mCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    UINT stride = sizeof(Vertex::TreePointSprite);
    UINT offset = 0;
    mCtx->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    mCtx->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

    EffectMgr::SpriteFX->DefaultTech->GetPassByIndex(0)->Apply(0, mCtx);

    mCtx->DrawIndexed(mQuads.size()*6, 0, 0);
}

