#ifndef INPUT_LAYOUT_MGR_H 
#define INPUT_LAYOUT_MGR_H

#include "Util.h"
#include "d3dx11Effect.h"

namespace Vertex
{
    struct PosNormalTex
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 Tex;
    };
    struct TreePointSprite
    {
        TreePointSprite()
        {
            ZeroMemory(this, sizeof(TreePointSprite));
        }
        XMFLOAT3 Pos;
        XMFLOAT2 Size;
    };
    struct OverlayVertex
    {
        OverlayVertex()
        {
            ZeroMemory(this, sizeof(OverlayVertex));
        }
        XMFLOAT3 Pos;
        XMFLOAT2 Tex;
        XMFLOAT4 Color;
    };
}

class InputLayoutDesc
{
public:
    // Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
    static const D3D11_INPUT_ELEMENT_DESC PosNormalTex[3];
    static const D3D11_INPUT_ELEMENT_DESC TreePointSprite[2];
    static const D3D11_INPUT_ELEMENT_DESC OverlayVertex[3];
};

// A simple manager that expose access to input layouts of various vertex formats
class InputLayoutMgr
{
public:
    static void initAll(ID3D11Device* device);
    static void destroyAll();

    static ID3D11InputLayout* PosNormalTex;
    static ID3D11InputLayout* TreePointSprite;
    static ID3D11InputLayout* OverlayVertex;

    // Overload method to get input layout by vertex type
    static ID3D11InputLayout* get(Vertex::OverlayVertex* p)
    {
        return OverlayVertex;
    }
    static ID3D11InputLayout* get(Vertex::PosNormalTex* p)
    {
        return PosNormalTex;
    }
    static ID3D11InputLayout* get(Vertex::TreePointSprite* p)
    {
        return TreePointSprite;
    }

};


#endif