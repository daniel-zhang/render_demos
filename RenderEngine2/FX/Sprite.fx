#include "LightHelper.fx"

Texture1DArray gTransforms;

struct VertexIn
{
    float3 PosNDC: POSITION;
    float2 SizeW: SIZE;
};

struct VertexOut
{
    float4 PosNDC:SV_POSITION;
    float2 SizeW: SIZE;
};

// Pass through
VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosNDC = float4(vin.PosNDC, 1.f);
    vout.SizeW = vin.SizeW;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(0.9f, 0.9f, 0.9f, 0.8f);
}

technique11 DefaultTech
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