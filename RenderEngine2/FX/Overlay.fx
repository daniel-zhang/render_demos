#include "LightHelper.fx"

// Nonnumeric values cannot be added to a cbuffer.
cbuffer cbPerObject
{
    float4x4 gTranslation;
    float4x4 gScale;
    float4 gVertexColor;
}; 

Texture2D gDiffuseMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

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
    vout.PosNDC = mul(float4(vin.PosNDC, 1.f), mul(gScale, gTranslation));
    vout.Tex = vin.Tex;
    vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin, uniform bool useTex, uniform bool setFontColor) : SV_Target
{
    if(useTex && setFontColor)
    {
        float4 texColor = gDiffuseMap.Sample( samLinear, pin.Tex );
        clip( texColor.a - 0.25f );

        //return gVertexColor;
        return texColor * gVertexColor;
        //return texColor;
    }
    else if(useTex)
    {
        float4 texColor = float4(1.f, 1.f, 1.f, 1.f);
        texColor = gDiffuseMap.Sample( samLinear, pin.Tex );
        clip( texColor.x - 0.2f );

        return texColor;
    }
    else
    {
        return gVertexColor;
    }
}

technique11 OverlayTech
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetHullShader( NULL );
        SetDomainShader( NULL );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false, false) ) );
    }
}

technique11 OverlayTexTech
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetHullShader( NULL );
        SetDomainShader( NULL );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, true) ) );
    }
}