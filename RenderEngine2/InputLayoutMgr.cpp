#include "InputLayoutMgr.h"
#include "EffectMgr.h"

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTex[3] = 
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0} 
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::TreePointSprite[2] = 
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::OverlayVertex[3] = 
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0} 
};


ID3D11InputLayout* InputLayoutMgr::PosNormalTex = 0;
ID3D11InputLayout* InputLayoutMgr::TreePointSprite = 0;
ID3D11InputLayout* InputLayoutMgr::OverlayVertex= 0;

void InputLayoutMgr::initAll( ID3D11Device* device )
{
    D3DX11_PASS_DESC passDesc;

    //
    // PosNormalTex
    //
    EffectMgr::BasicFX->mLight1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(device->CreateInputLayout( 
        InputLayoutDesc::PosNormalTex, 
        3, 
        passDesc.pIAInputSignature, 
        passDesc.IAInputSignatureSize, 
        &PosNormalTex));

    //
    // TreePointSprite
    //
    EffectMgr::TreeSpriteFX->Light3Tech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(device->CreateInputLayout(InputLayoutDesc::TreePointSprite, 2, passDesc.pIAInputSignature, 
        passDesc.IAInputSignatureSize, &TreePointSprite));

    //
    // OverlayVertex
    //
    //TODO
    EffectMgr::OverlayFX->OverlayTech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(device->CreateInputLayout(InputLayoutDesc::OverlayVertex, 3, passDesc.pIAInputSignature, 
        passDesc.IAInputSignatureSize, &OverlayVertex));

}

void InputLayoutMgr::destroyAll()
{
    safe_release(&PosNormalTex);
    safe_release(&TreePointSprite);
    safe_release(&OverlayVertex);
    safe_release(&OverlayVertex);
}
