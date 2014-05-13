#include "EffectMgr.h"

// device is used only once in the ctor, thus it is not held in this class
Effect::Effect( ID3D11Device* device, const std::wstring& filename ) 
    : mFX(0)
{
    std::ifstream fin(filename, std::ios::binary);
    fin.seekg(0, std::ios_base::end);
    int size = (int)fin.tellg();
    fin.seekg(0, std::ios_base::beg);
    std::vector<char> shaderByteCode(size);

    fin.read(&shaderByteCode[0], size);
    fin.close();

    HR(D3DX11CreateEffectFromMemory(&shaderByteCode[0], size, 0, device, &mFX));
}

Effect::~Effect()
{
    safe_release(&mFX);
}

BasicEffect::BasicEffect( ID3D11Device* device, const std::wstring& filename ) 
    : Effect(device, filename)
{
    //
    // Get techniques via Effect framework
    //
    mLight1Tech          = mFX->GetTechniqueByName("Light1");
    mLight2Tech          = mFX->GetTechniqueByName("Light2");
    mLight3Tech		     = mFX->GetTechniqueByName("Light3");

    mLight0TexTech = mFX->GetTechniqueByName("Light0Tex");
    mLight1TexTech = mFX->GetTechniqueByName("Light1Tex");
    mLight2TexTech = mFX->GetTechniqueByName("Light2Tex");
    mLight3TexTech = mFX->GetTechniqueByName("Light3Tex");

    mLight0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
    mLight1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
    mLight2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
    mLight3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

    mLight1FogTech    = mFX->GetTechniqueByName("Light1Fog");
    mLight2FogTech    = mFX->GetTechniqueByName("Light2Fog");
    mLight3FogTech    = mFX->GetTechniqueByName("Light3Fog");

    mLight0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
    mLight1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
    mLight2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
    mLight3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

    mLight0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
    mLight1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
    mLight2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
    mLight3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

    //
    // Bind Effect variables in C++ application to their mirrored ones in HLSL
    //

    // Transform matrices
    mfxWorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
    mfxWorld             = mFX->GetVariableByName("gWorld")->AsMatrix();
    mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
    mfxTexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();

    // Lighting
    mfxEyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
    mfxDirLights         = mFX->GetVariableByName("gDirLights");

    // Materials and textures
    mfxMaterial          = mFX->GetVariableByName("gMaterial");
    mfxDiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

    // Fog
    mfxFogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
    mfxFogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
    mfxFogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
}

BasicEffect::~BasicEffect()
{
}

TreeSpriteEffect::TreeSpriteEffect( ID3D11Device* device, const std::wstring& filename )
    : Effect(device, filename)
{
    Light3Tech                = mFX->GetTechniqueByName("Light3");
    Light3TexAlphaClipTech    = mFX->GetTechniqueByName("Light3TexAlphaClip");
    Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

    ViewProj            = mFX->GetVariableByName("gViewProj")->AsMatrix();
    EyePosW             = mFX->GetVariableByName("gEyePosW")->AsVector();
    FogColor            = mFX->GetVariableByName("gFogColor")->AsVector();
    FogStart            = mFX->GetVariableByName("gFogStart")->AsScalar();
    FogRange            = mFX->GetVariableByName("gFogRange")->AsScalar();
    DirLights           = mFX->GetVariableByName("gDirLights");
    Mat                 = mFX->GetVariableByName("gMaterial");
    TreeTextureMapArray = mFX->GetVariableByName("gTreeMapArray")->AsShaderResource();
}

//
// Static member variables need to be explicitly initialized
//
BasicEffect* EffectMgr::BasicFX = 0;
TreeSpriteEffect* EffectMgr::TreeSpriteFX = 0;
SpriteEffect* EffectMgr::SpriteFX= 0;
OverlayEffect* EffectMgr::OverlayFX = 0;

void EffectMgr::initAll( ID3D11Device* device )
{
    BasicFX = new BasicEffect(device, L"../FX/Basic.fxo");
    TreeSpriteFX = new TreeSpriteEffect(device, L"../FX/Billboard.fxo");
    SpriteFX = new SpriteEffect(device, L"../FX/Sprite.fxo");
    OverlayFX = new OverlayEffect(device, L"../FX/Overlay.fxo");
}

void EffectMgr::destroyAll()
{
    // Safe delete
    if(BasicFX) 
    {
        delete BasicFX; 
        BasicFX = 0;
    }
    if(TreeSpriteFX) 
    {
        delete TreeSpriteFX; 
        TreeSpriteFX= 0;
    }
    if(SpriteFX) 
    {
        delete SpriteFX; 
        SpriteFX = 0;
    }
    if(OverlayFX) 
    {
        delete OverlayFX; 
        OverlayFX = 0;
    }
}
//
//
//
SpriteEffect::SpriteEffect( ID3D11Device* device, const std::wstring& filename ):
    Effect(device, filename)
{
    mData = mFX->GetVariableByName("gTransforms")->AsShaderResource();
    DefaultTech = mFX->GetTechniqueByName("DefaultTech");
}


//
// Overlay Effect
//
OverlayEffect::OverlayEffect( ID3D11Device* device, const std::wstring& filename ):
    Effect(device, filename)
{
    OverlayTech = mFX->GetTechniqueByName("OverlayTech");
    OverlayTexTech = mFX->GetTechniqueByName("OverlayTexTech");

    mfxDiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

void OverlayEffect::setDiffuseMap( ID3D11ShaderResourceView* tex )
{
    assert(tex);
    mfxDiffuseMap->SetResource(tex);
}
