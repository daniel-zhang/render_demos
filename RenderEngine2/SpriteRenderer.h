#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Sprite2D.h"

class SpriteRenderer
{
public:
    SpriteRenderer();
    ~SpriteRenderer();

    bool init(ID3D11Device* device, ID3D11DeviceContext* ctx);

    void beforeDraw();
    void draw(Sprite2D& sprite);
    void afterDraw();

    void onViewportResize(Area2D& newArea);
    D3DEnv* getRenderEnv();

protected:
    D3DEnv mEnv;
    Area2D mVpSize;
};

#endif