#ifndef MODEL_H
#define MODEL_H

#include "Util.h"
#include "GeometryGenerator.h"
#include "InputLayoutMgr.h"
#include "LightHelper.h"

class Model
{
public:
    Model(ID3D11Device* dv, ID3D11DeviceContext* ctx);
    virtual ~Model();

    //
    // Supports loading from .obj, GeometryGenerator, and the simple meshes
    //
    void loadObj(std::wstring filename, bool coordConvert = true);
    void loadPrimitive(GeometryGenerator::MeshData& mesh);
    void loadSimpleMesh(std::wstring filename);

    void update(XMFLOAT4X4 world, XMFLOAT4X4 texTransform);
    // TODO: RenderState management
    void draw(ID3DX11EffectTechnique* pTech, XMFLOAT4X4 viewProj);
    
    // TODO: Binary save/load 
    virtual void save(){}
    virtual void load(){}

private:
    // Vertex structure is defined in InputLayoutManager
    struct ModelSubset 
    {
        ModelSubset(): matId(-1), indexOffset(0), indexCount(0)
        {}
        int matId;
        UINT indexOffset;
        UINT indexCount;
    };

private:
    // Pipeline
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mCtx;

    // Mesh
    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;
    std::vector<ModelSubset> mSubsets;
    std::vector<Material> mMaterials;
    std::vector<ID3D11ShaderResourceView*> mDiffuseMaps;

    // System memory copy of mesh
    std::vector<Vertex::PosNormalTex> mVertices;
    std::vector<UINT> mIndices;

    // Matrices
    XMFLOAT4X4 mWorld;
    XMFLOAT4X4 mTexTransform;
};
#endif
