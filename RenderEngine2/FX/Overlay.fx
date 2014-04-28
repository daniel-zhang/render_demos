#include "LightHelper.fx"

Texture1DArray gTransforms;

struct VertexIn
{
    float3 PosNDC: POSITION;
    float2 Tex: TEXCOORD;
    float4 Color: COLOR;
};

struct VertexOut
{
    float4 PosNDC: SV_POSITION;
    float2 Tex: TEXCOORD;
    float4 Color: COLOR;
};

// Pass through
VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosNDC = float4(vin.PosNDC, 1.f);
    vout.Tex = vin.Tex;
    vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}

technique11 OverlayTech
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetHullShader( NULL );
        SetDomainShader( NULL );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}