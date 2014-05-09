#ifndef QUAD_BUFFER_H 
#define QUAD_BUFFER_H

#include "Util.h"
//
// Support only triangle_list for simplicity
//
template <typename VertexType>
class QuadBuffer
{
public:
    QuadBuffer()
    {
        mVB = 0;
        mIB = 0;
        mMaxQuadNum = 1024;
        mCurrQuadNum = 0;
    }

    ~QuadBuffer()
    {
        clearBuffer();
    }

    bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, UINT initialQuadNum = 1024)
    {
        mDevice = pDevice;
        mCtx = pContext;
        mMaxQuadNum = initialQuadNum;
        mCurrQuadNum = 0;

        return createBuffer();
    }

    bool clear()
    {
        clearBuffer();
        mMaxQuadNum = 1024;
    }

    bool update(std::vector<VertexType>& vertices)
    {
        // Input vertices does not represent valid quads
        if (vertices.size() % 4 != 0)
        {
            return false;
        }

        mCurrQuadNum = vertices.size()/4;

        // Double the buffer if needed
        if (mCurrQuadNum > mMaxQuadNum)
        {
            mMaxQuadNum = 2 * mCurrQuadNum;
            clearBuffer();
            createBuffer();
        }

        // Map the buffer
        D3D11_MAPPED_SUBRESOURCE mappedData;
        mCtx->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
        VertexType* vStart = reinterpret_cast<VertexType*>(mappedData.pData);
        for (UINT i = 0; i < vertices.size(); ++i)
        {
            vStart[i] = vertices[i];
        }
        mCtx->Unmap(mVB, 0);

        return true;
    }

    void setPipeline()
    {
        mCtx->IASetInputLayout(InputLayoutMgr::get(static_cast<VertexType*>(0)));
        mCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = sizeof(VertexType);
        UINT offset = 0;
        mCtx->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
        mCtx->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
    }

    UINT getIndexNum()
    {
        return mCurrQuadNum * 6;
    }

private:
    bool createBuffer()
    {
        // Create an empty VB
        D3D11_BUFFER_DESC vbd;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.ByteWidth = sizeof(VertexType) * mMaxQuadNum * 4;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vbd.MiscFlags = 0;
        vbd.StructureByteStride = 0;
        vbd.Usage = D3D11_USAGE_DYNAMIC; // make dynamic
        HR(mDevice->CreateBuffer(&vbd, 0, &mVB));

        // Create an empty IB
        // IB can be immutable, as the actual quads drawn is controlled via 
        // DrawIndexed(index_num, 0, 0)
        std::vector<UINT> indices(mMaxQuadNum * 6, 0);
        // winding order: left-hand,
        for (UINT i = 0; i < mMaxQuadNum; ++i)
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
        ibd.ByteWidth = sizeof(UINT) * mMaxQuadNum * 6;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
        ibd.StructureByteStride = 0;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA idata;
        idata.pSysMem = &indices[0];
        HR(mDevice->CreateBuffer(&ibd, &idata, &mIB));

        return true;
    }

    void clearBuffer()
    {
        safe_release(&mVB);
        safe_release(&mIB);
        mCurrQuadNum = 0;
    }

    UINT mMaxQuadNum;
    UINT mCurrQuadNum;
    
    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;

public:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mCtx;
};

#endif