#include "LightHelper.fx"

cbuffer cbPerFrame
{
    DirectionalLight gDirLights[3];
    float3 gEyePosW;
    float gFogStart;
    float gFogRange;
    float gFogColor;
};

cbuffer cbPerObject
{
    float4x4 gViewProj;
    Material gMaterial;
};

cbuffer cbFixed
{
    float2 gTexC[4] = 
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };
};

Texture2DArray gTreeMapArray;

SamplerState linearSampling
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

// Vertex Shader does nothing
struct VertexIn
{
    float3 PosW: POSITION;
    float2 SizeW: SIZE;
};

struct VertexOut
{
    float3 PosW: POSITION;
    float2 SizeW: SIZE;
};

struct GeoOut
{
    float4 PosH: SV_POSITION;
    float3 PosW: POSITION;
    float3 Normal: NORMAL;
    float2 Tex: TEXCOORD;
    uint PrimID: SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosW = vin.PosW;
    vout.SizeW = vin.SizeW;
    return vout;
}

// SV_PrimitiveID is passed in by hardware
[maxvertexcount(4)]
void GS(point VertexOut gin[1], uint PrimID:SV_PrimitiveID, 
    inout TriangleStream<GeoOut> triStream)
{
    float3 up = float3(0.f, 1.f, 0.f);
    float3 look = gEyePosW - gin[0].PosW;
    look.y = 0.f;
    look = normalize(look);
    float3 right = cross(up, look);

    float3 halfWidth = gin[0].SizeW.x*0.5f;
    float3 halfHeight = gin[0].SizeW.y*0.5f;

    // Construct the quad vertices in world space homospace coord.
    // Note that the input vertex is given in world spaces so world 
    // transform is not required.
    float4 v[4];
    v[0] = float4(gin[0].PosW + halfWidth*right - halfHeight*up, 1.0f);
    v[1] = float4(gin[0].PosW + halfWidth*right + halfHeight*up, 1.0f);
    v[2] = float4(gin[0].PosW - halfWidth*right - halfHeight*up, 1.0f);
    v[3] = float4(gin[0].PosW - halfWidth*right + halfHeight*up, 1.0f);

    // Transform the quad vertices from world to projection space
    // then construct a triangle trip stream. Index is not required.
    GeoOut gout;
    [unroll]
    for(int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = gout.PosH.xyz;
        gout.Normal = look;
        gout.Tex = gTexC[i];
        gout.PrimID = PrimID;

        triStream.Append(gout);
    }
}

// Note
// pin is not a vertex in the triangle stream output from GS 
// It is a machine generated vertex whose attributes are interpolated
float4 PS(GeoOut pin, 
    uniform int lightCount, uniform bool useTexture, 
    uniform bool alphaClip, uniform bool useFog) : SV_Target
{
    // Re-normalize interpolated normals
    pin.Normal = normalize(pin.Normal);
    float3 toEye = gEyePosW - pin.PosW;
    float distToEye = length(toEye);
    toEye /= distToEye;

    //
    // Sample texture array
    //
    float4 texColor = float4(1, 1, 1, 1);
    if(useTexture)
    {
        float3 uvw = float3(pin.Tex, pin.PrimID%4);
        texColor = gTreeMapArray.Sample(linearSampling, uvw);
        if(alphaClip)
        {
            // Discard pixel if texture alpha < 0.05.  Note that we do this
            // test as soon as possible so that we can potentially exit the shader 
            // early, thereby skipping the rest of the shader code.
            clip(texColor.a - 0.05f);
        }
    }

    //
    // Lighting.
    //
    float4 litColor = texColor;
    if( lightCount > 0  )
    {
        // Start with a sum of zero.
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

        // Sum the light contribution from each light source.  
        [unroll]
        for(int i = 0; i < lightCount; ++i)
        {
            float4 A, D, S;
            ComputeDirectionalLight(gMaterial, gDirLights[i], pin.Normal, toEye, 
                A, D, S);

            ambient += A;
            diffuse += D;
            spec    += S;
        }

        // Modulate with late add.
        litColor = texColor*(ambient + diffuse) + spec;
    }


    if(useFog)
    {
        float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 

        // Blend the fog color and the lit color.
        litColor = lerp(litColor, gFogColor, fogLerp);
    }

    return litColor;
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( CompileShader( gs_5_0, GS() ) );
        SetPixelShader( CompileShader( ps_5_0, PS(3, false, false, false) ) );
    }
}

technique11 Light3TexAlphaClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( CompileShader( gs_5_0, GS() ) );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, true, false) ) );
    }
}
            
technique11 Light3TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( CompileShader( gs_5_0, GS() ) );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, true, true) ) );
    }
}
