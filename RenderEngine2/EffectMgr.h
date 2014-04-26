// A wrapper to ease the use of Microsoft Effect Framework
#ifndef EFFECT_MGR_H 
#define EFFECT_MGR_H

#include "Util.h"
#include "d3dx11Effect.h"
#include "LightHelper.h"

class Effect
{
public:
    Effect(ID3D11Device* device, const std::wstring& filename);
    virtual ~Effect();

private:
    // Not copy-able, no need to provide implementations
    // As they should be never called.
    Effect(const Effect& rhs);
    Effect& operator=(const Effect& rhs);

protected:
    ID3DX11Effect* mFX;
};

class BasicEffect : public Effect 
{
public:
    BasicEffect(ID3D11Device* device, const std::wstring& filename);
    ~BasicEffect();

    //
    // Setters
    //

    // Transform matrices
    void setWorldViewProj(CXMMATRIX M)                  
    { mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

    void setWorld(CXMMATRIX M)                          
    { mfxWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }

    void setWorldInvTranspose(CXMMATRIX M)              
    { mfxWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }

    void setTexTransform(CXMMATRIX M)                   
    { mfxTexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

    // Lighting
    void setEyePosW(const XMFLOAT3& v)                  
    { mfxEyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

    void setDirLights(const DirectionalLight* lights)   
    { mfxDirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }

    // Materials and textures
    void setMaterial(const Material& mat)               
    { mfxMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

    void setDiffuseMap(ID3D11ShaderResourceView* tex)   
    { mfxDiffuseMap->SetResource(tex); }

    // Fog effect
    void setFogColor(const FXMVECTOR v)                 
    { mfxFogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

    void setFogStart(float f)                           
    { mfxFogStart->SetFloat(f); }

    void setFogRange(float f)                           
    { mfxFogRange->SetFloat(f); }

    // Techniques
    ID3DX11EffectTechnique* mLight1Tech;
    ID3DX11EffectTechnique* mLight2Tech;
    ID3DX11EffectTechnique* mLight3Tech;

    ID3DX11EffectTechnique* mLight0TexTech;
    ID3DX11EffectTechnique* mLight1TexTech;
    ID3DX11EffectTechnique* mLight2TexTech;
    ID3DX11EffectTechnique* mLight3TexTech;

    ID3DX11EffectTechnique* mLight0TexAlphaClipTech;
    ID3DX11EffectTechnique* mLight1TexAlphaClipTech;
    ID3DX11EffectTechnique* mLight2TexAlphaClipTech;
    ID3DX11EffectTechnique* mLight3TexAlphaClipTech;

    ID3DX11EffectTechnique* mLight1FogTech;
    ID3DX11EffectTechnique* mLight2FogTech;
    ID3DX11EffectTechnique* mLight3FogTech;

    ID3DX11EffectTechnique* mLight0TexFogTech;
    ID3DX11EffectTechnique* mLight1TexFogTech;
    ID3DX11EffectTechnique* mLight2TexFogTech;
    ID3DX11EffectTechnique* mLight3TexFogTech;

    ID3DX11EffectTechnique* mLight0TexAlphaClipFogTech;
    ID3DX11EffectTechnique* mLight1TexAlphaClipFogTech;
    ID3DX11EffectTechnique* mLight2TexAlphaClipFogTech;
    ID3DX11EffectTechnique* mLight3TexAlphaClipFogTech;

    // The 'mfx' prefix donates an effect variable.
    // An effect variable is managed by effect frame-work 
    // in a batch-write manner to guarantee efficiency.
    // Transform matrices

    ID3DX11EffectMatrixVariable* mfxWorldViewProj;
    ID3DX11EffectMatrixVariable* mfxWorld;
    ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;
    ID3DX11EffectMatrixVariable* mfxTexTransform;

    // Lighting
    ID3DX11EffectVectorVariable* mfxEyePosW;
    ID3DX11EffectVariable* mfxDirLights;

    // Material and texture
    ID3DX11EffectVariable* mfxMaterial;
    ID3DX11EffectShaderResourceVariable* mfxDiffuseMap;

    // Fog
    ID3DX11EffectVectorVariable* mfxFogColor;
    ID3DX11EffectScalarVariable* mfxFogStart;
    ID3DX11EffectScalarVariable* mfxFogRange;
};
class TreeSpriteEffect : public Effect
{
public:
    TreeSpriteEffect(ID3D11Device* device, const std::wstring& filename);
    ~TreeSpriteEffect(){};

    void SetViewProj(CXMMATRIX M)                              { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetEyePosW(const XMFLOAT3& v)                         { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetFogColor(const FXMVECTOR v)                        { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
    void SetFogStart(float f)                                  { FogStart->SetFloat(f); }
    void SetFogRange(float f)                                  { FogRange->SetFloat(f); }
    void SetDirLights(const DirectionalLight* lights)          { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
    void SetMaterial(const Material& mat)                      { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
    void SetTreeTextureMapArray(ID3D11ShaderResourceView* tex) { TreeTextureMapArray->SetResource(tex); }

    ID3DX11EffectTechnique* Light3Tech;
    ID3DX11EffectTechnique* Light3TexAlphaClipTech;
    ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

    ID3DX11EffectMatrixVariable* ViewProj;
    ID3DX11EffectVectorVariable* EyePosW;
    ID3DX11EffectVectorVariable* FogColor;
    ID3DX11EffectScalarVariable* FogStart;
    ID3DX11EffectScalarVariable* FogRange;
    ID3DX11EffectVariable* DirLights;
    ID3DX11EffectVariable* Mat;

    ID3DX11EffectShaderResourceVariable* TreeTextureMapArray;
};

class SpriteEffect:public Effect
{
public:
    SpriteEffect(ID3D11Device* device, const std::wstring& filename);
    ~SpriteEffect(){}

    ID3DX11EffectTechnique* DefaultTech;
    void setData(ID3D11ShaderResourceView* data){mData->SetResource(data);}
    ID3DX11EffectShaderResourceVariable* mData;
};


// A simple effect manager that expose global access to various effects
class EffectMgr
{
public:
    static void initAll(ID3D11Device* device);
    static void destroyAll();

    static BasicEffect* BasicFX;
    static TreeSpriteEffect* TreeSpriteFX; 
    static SpriteEffect* SpriteFX;
};
#endif