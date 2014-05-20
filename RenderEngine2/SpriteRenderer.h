#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Sprite2D.h"

struct D3DEnv 
{
    D3DEnv(){ZeroMemory(this, sizeof D3DEnv);}
    ID3D11Device* device;
    ID3D11DeviceContext* context;
};

class SpriteRenderer
{
public:
    SpriteRenderer();
    ~SpriteRenderer();

    bool init(ID3D11Device* device, ID3D11DeviceContext* ctx);
    void draw(Sprite2D& sprite);
    void onViewportResize(Area2D& newArea);
    D3DEnv& getRenderEnv();

protected:
    D3DEnv mEnv;
    Area2D mVpSize;
};

#endif