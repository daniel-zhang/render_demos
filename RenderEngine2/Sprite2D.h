#ifndef SPRITE2D_H
#define SPRITE2D_H

#include "Math2D.h"
#include <d3dx11.h>
#include <string>
#include "EffectMgr.h"
#include "InputLayoutMgr.h"
#include "Color.h"

class SpriteRenderer;
struct D3DEnv;

class Sprite2D
{
public:
    friend SpriteRenderer;

    Sprite2D();
    ~Sprite2D();

    bool init(D3DEnv& env, OverlayEffect* effect, ID3DX11EffectTechnique* tech);

    void move(Vector2D& movement);
    void resize(Area2D& newSize);

    void setTexture(ID3D11ShaderResourceView* srv, FBox2D& srcBox);
    void setColor(const RGBA& color);
    void setRSState(ID3D11RasterizerState* state){mRSState = state;}

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

    ID3D11RasterizerState* mRSState;
    ID3D11RasterizerState* mRSCachedState;

    OverlayEffect* mEffect;
    ID3DX11EffectTechnique* mTech;
};



#endif
