#include "RenderStateMgr.h"

// Init static member variables
ID3D11RasterizerState* RenderStateMgr::WireframeRS = 0;
ID3D11RasterizerState* RenderStateMgr::NoCullRS    = 0;
ID3D11RasterizerState* RenderStateMgr::ScissorRS = 0;
     
ID3D11DepthStencilState* RenderStateMgr::NoDepthTestDS = 0;

ID3D11BlendState*      RenderStateMgr::AlphaToCoverageBS = 0;
ID3D11BlendState*      RenderStateMgr::TransparentBS     = 0;

void RenderStateMgr::initAll( ID3D11Device* device )
{
    //
    // WireframeRS
    //
    D3D11_RASTERIZER_DESC wireframeDesc;
    ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
    wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
    wireframeDesc.CullMode = D3D11_CULL_BACK;
    wireframeDesc.FrontCounterClockwise = false;
    wireframeDesc.DepthClipEnable = true;

    HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

    //
    // NoCullRS
    //
    D3D11_RASTERIZER_DESC noCullDesc;
    ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
    noCullDesc.FillMode = D3D11_FILL_SOLID;
    noCullDesc.CullMode = D3D11_CULL_NONE;
    noCullDesc.FrontCounterClockwise = false;
    noCullDesc.DepthClipEnable = true;

    HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

    //
    // ScissorRS
    //
    D3D11_RASTERIZER_DESC scissorDesc;
    ZeroMemory(&scissorDesc, sizeof(D3D11_RASTERIZER_DESC));
    scissorDesc.FillMode = D3D11_FILL_SOLID;
    scissorDesc.CullMode = D3D11_CULL_BACK;
    scissorDesc.FrontCounterClockwise = FALSE;
    scissorDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
    scissorDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
    scissorDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    scissorDesc.DepthClipEnable = FALSE;
    scissorDesc.ScissorEnable = TRUE;
    scissorDesc.MultisampleEnable = FALSE;
    scissorDesc.AntialiasedLineEnable = FALSE;

    HR(device->CreateRasterizerState(&scissorDesc, &ScissorRS));
    
    //
    // NoDepthTestDS
    //
    CD3D11_DEPTH_STENCIL_DESC noDepthTestDesc(D3D11_DEFAULT);
    noDepthTestDesc.DepthEnable = FALSE;
    HR(device->CreateDepthStencilState(&noDepthTestDesc, &NoDepthTestDS));

    //
    // AlphaToCoverageBS
    //

    D3D11_BLEND_DESC alphaToCoverageDesc = {0};
    alphaToCoverageDesc.AlphaToCoverageEnable = true;
    alphaToCoverageDesc.IndependentBlendEnable = false;
    alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
    alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

    //
    // TransparentBS
    //

    D3D11_BLEND_DESC transparentDesc = {0};
    transparentDesc.AlphaToCoverageEnable = false;
    transparentDesc.IndependentBlendEnable = false;

    transparentDesc.RenderTarget[0].BlendEnable = true;
    transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
    transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
    transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
    transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
    transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
    transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HR(device->CreateBlendState(&transparentDesc, &TransparentBS));
}

void RenderStateMgr::destroyAll()
{
    safe_release(&WireframeRS);
    safe_release(&NoCullRS);
    safe_release(&AlphaToCoverageBS);
    safe_release(&TransparentBS);
}



