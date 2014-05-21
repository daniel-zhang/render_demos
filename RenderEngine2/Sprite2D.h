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

    bool init(D3DEnv& env);

    void move(Vector2D& movement);
    void resize(Area2D& newSize);
    void setDstBox(Box2D& box);
    Box2D& getDstBox();

    void setEffect(OverlayEffect* effect);
    void setTechique(ID3DX11EffectTechnique* tech);
    void setTexture(ID3D11ShaderResourceView* srv, FBox2D& srcBox);
    void setColor(const RGBA& color);
    void setClipBox(Box2D& clipBox);

    void enableClip();
    void disableClip();

    void enableExternalTextureManagement();
    void disableExternalTextureManagement();

private:
    bool createGeometryBuffer(D3DEnv& env);
    void clearGeometryBuffer();
    void syncGeomtryBuffer(Area2D& vpSize, D3DEnv& env);

    void beforeDraw(D3DEnv& env);
    void afterDraw(D3DEnv& env);

private:
    Box2D mDstBox;
    Box2D mClipBox;
    FBox2D mSrcBox;
    RGBA mVertexColor;

    bool mIsClipped;
    bool mIsSynced;
    bool mTextureManagedExternally;

    // Keep a CPU copy of vertices, which also saves overhead of mem alloc
    // and makes vector::push_back() slightly faster.
    std::vector<Vertex::OverlayVertex> mVertices;
    ID3D11ShaderResourceView* mSRV;
    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;

    OverlayEffect* mEffect;
    ID3DX11EffectTechnique* mTech;
};



#endif
