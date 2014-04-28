#include "EffectMgr.h"
#include "OverlayGraphics.h"

/*
  v1----------v2
  |           |
  |           |
  v0----------v3
*/
using namespace OverlayUI;

Quad::Quad( XMFLOAT2 ndcPos, XMFLOAT2 ndcSize, XMFLOAT4 color, float depth /*= 0.1f*/ )
{
    mVertices[0].Pos = XMFLOAT3(ndcPos.x + 0.f - ndcSize.x, ndcPos.y + 0.f - ndcSize.y, depth);
    mVertices[1].Pos = XMFLOAT3(ndcPos.x + 0.f - ndcSize.x, ndcPos.y + ndcSize.y, depth);
    mVertices[2].Pos = XMFLOAT3(ndcPos.x + ndcSize.x, ndcPos.y + ndcSize.y, depth);
    mVertices[3].Pos = XMFLOAT3(ndcPos.x + ndcSize.x, ndcPos.y + 0.f - ndcSize.y, depth);

    for (UINT i = 0; i < 4; ++i)
    {
        mVertices[i].Color = color;
        mKeyFrameVertices[i].Color = color;
    }

    mPivot = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
    mTranslation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
    mScaling = XMFLOAT4(1.f, 1.f, 1.f, 0.f);
}

Quad::~Quad()
{

}

void OverlayUI::Quad::updateNdcPos( XMFLOAT2 ndcPos )
{
    mTranslation.x = ndcPos.x;
    mTranslation.y = ndcPos.y;
    
    transformToKeyFrame();
}

void Quad::updateNdcSize( XMFLOAT2 ndcSize )
{
    // TODO
}

void Quad::transformToKeyFrame()
{
    XMMATRIX T = XMMatrixAffineTransformation2D(
        XMLoadFloat4(&mScaling), 
        XMLoadFloat4(&mPivot), 
        0.f,
        XMLoadFloat4(&mTranslation));

    for (UINT i = 0; i < 4; ++i)
    {
        XMVECTOR p = XMLoadFloat3(&mVertices[i].Pos);
        XMVECTOR m = XMVector3TransformCoord(p, T);
        XMStoreFloat3(&mKeyFrameVertices[i].Pos, m);
    }
}

//
//
//

OverlayGraphics::OverlayGraphics()
{
    mVB = 0;
    mIB = 0;
}

OverlayGraphics::~OverlayGraphics()
{

}

bool OverlayGraphics::init( ID3D11Device* dv, ID3D11DeviceContext* ctx )
{
    mDevice = dv;
    mCtx = ctx;

    return true;
}


Quad* OverlayGraphics::createQuad( XMFLOAT2 ndcPos, XMFLOAT2 ndcSize)
{
    Quad* q = new Quad(ndcPos, ndcSize);
    mQuads.push_back(q);

    return q;
}

void OverlayGraphics::beginBatch()
{
    clearQuads();
}

void OverlayGraphics::endBatch()
{
    clearGraphicBuffers();
    createGraphicBuffers();
}

void OverlayGraphics::drawOverlay()
{
    D3D11_MAPPED_SUBRESOURCE mappedData;
    mCtx->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    Vertex::OverlayVertex* vStart = reinterpret_cast<Vertex::OverlayVertex*>(mappedData.pData);
    for(UINT i = 0; i < mQuads.size(); ++i)
    {
        Vertex::OverlayVertex* pV = mQuads[i]->getKeyFrameVertices();
        vStart[4*i + 0] = pV[0];
        vStart[4*i + 1] = pV[1];
        vStart[4*i + 2] = pV[2];
        vStart[4*i + 3] = pV[3];
    }
    mCtx->Unmap(mVB, 0);

    mCtx->IASetInputLayout(InputLayoutMgr::OverlayVertex);
    mCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    UINT stride = sizeof(Vertex::OverlayVertex);
    UINT offset = 0;
    mCtx->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    mCtx->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

    EffectMgr::OverlayFX->OverlayTech->GetPassByIndex(0)->Apply(0, mCtx);

    mCtx->DrawIndexed(mQuads.size()*6, 0, 0);
}

// Adding new window is not supported yet
void OverlayGraphics::createGraphicBuffers()
{
    //
    // Create a dynamic VB 
    //
    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof(Vertex::OverlayVertex) * mQuads.size() * 4;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    vbd.Usage = D3D11_USAGE_DYNAMIC; // make dynamic
    HR(mDevice->CreateBuffer(&vbd, 0, &mVB));

    //
    // Create static IB
    //
    std::vector<UINT> indices(mQuads.size() * 6, 0);
    for (UINT i = 0; i < mQuads.size(); ++i)
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
    ibd.ByteWidth = sizeof(UINT) * mQuads.size() * 6;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA idata;
    idata.pSysMem = &indices[0];
    HR(mDevice->CreateBuffer(&ibd, &idata, &mIB));
}

void OverlayGraphics::clearQuads()
{
    for (UINT i = 0; i < mQuads.size(); ++i)
    {
        if (mQuads[i])
        {
            delete mQuads[i];
            mQuads[i] = 0;
        }
    }
    mQuads.clear();
}

void OverlayGraphics::clearGraphicBuffers()
{
    safe_release(&mVB);
    safe_release(&mIB);
}
