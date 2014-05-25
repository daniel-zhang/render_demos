#ifndef SPRITE2D_H
#define SPRITE2D_H

#include "Math2D.h"
#include <d3dx11.h>
#include <string>
#include "EffectMgr.h"
#include "InputLayoutMgr.h"
#include "Color.h"

class SpriteRenderer;

struct D3DEnv 
{
    D3DEnv(){ZeroMemory(this, sizeof D3DEnv);}
    ID3D11Device* device;
    ID3D11DeviceContext* context;
};

class Sprite2D
{
public:
    friend class SpriteRenderer;

    Sprite2D();
    ~Sprite2D();

    bool init(
        D3DEnv* env, Point2D& pos, Area2D& size, const RGBA& color, 
        OverlayEffect* effect, ID3DX11EffectTechnique* tech,
        bool useDynamicVB = false,
        ID3D11ShaderResourceView* srv = NULL,
        FBox2D& srcBox = FBox2D()
        );

    void move(Vector2D& movement);
    void moveTo(Point2D& pos);
    void resize(Area2D& newSize);

    // setTexutre(...) only works for static sprite
    void setTexture(ID3D11ShaderResourceView* srv);
    void setEffect(OverlayEffect* effect);
    void setTechique(ID3DX11EffectTechnique* tech);
    void setColor(const RGBA& color);

    void dynamicSync(Point2D& pos, Area2D& size, FBox2D& srcBox, ID3D11ShaderResourceView* srv);

    // D3D pipeline receives clip box in screen space.
    void setClipBox(Box2D* clipBox);
    void enableClip();
    void disableClip();

    void enableExternalTextureManagement();
    void disableExternalTextureManagement();

    Box2D* getCacheBox();

private:
    bool createStaticGeometryBuffer();
    bool createDynamicGeometryBuffer();
    void syncDynamicGeomtryBuffer();

    void clearGeometryBuffer();

    void getViewportSize(Area2D& vpSize);

    void syncTransformation();

    void beforeDraw(D3DEnv& env);
    void afterDraw(D3DEnv& env);

private:
    // Cache Rect is used to expose screen space interface to user code.
    // It stores the sprite's current position and size in screen space, and is something like a 
    // command cache for user operations(like move and resize). CacheRect is synced to transform
    // matrices in shader at each frame. This way the VB can be static if texcoords are not changed.
    Box2D mCacheRect;

    // Reference rect and intialVP are immutable through out the life time of the sprite.
    // RefereceRect is directly mapped to VB 
    // initialViewport is stored so sprite size can remain unchanged and independent 
    // of viewport size.
    NdcBox2D mReferenceRect;
    Area2D mInitialVP;

    Box2D* mClipBox;
    FBox2D mSrcBox;

    RGBA mVertexColor;
    XMFLOAT4X4 mTranslation;
    XMFLOAT4X4 mScale;

    bool mIsClipped;
    bool mTextureManagedExternally;
    bool mDynamic;

    // Keep a CPU copy of vertices, which also saves overhead of mem alloc
    // and makes vector::push_back() slightly faster.
    std::vector<Vertex::OverlayVertex> mVertices;
    ID3D11ShaderResourceView* mSRV;
    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;

    OverlayEffect* mEffect;
    ID3DX11EffectTechnique* mTech;
    D3DEnv* mEnv;
};

#endif
