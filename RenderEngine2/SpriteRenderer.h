#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Sprite2D.h"

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
    void _beforeDraw();
    void _draw(Sprite2D& sprite);
    void _afterDraw();
    D3DEnv mEnv;
    Area2D mVpSize;
};

#endif