#ifndef SPRITE_DEMO_H
#define SPRITE_DEMO_H

#include "DemoBasic.h"
#include "Sprite.h"
#include "FontEngine.h"

class SpriteDemo : public DemoBasic
{
public:
    SpriteDemo();
    ~SpriteDemo();

    virtual bool init();
    virtual void drawScene();

private:
    ID3D11Buffer* mSpriteVB;
    ID3D11Buffer* mSpriteIB;
    SpriteGenertaor* mSpriteGen;
    FontEngine mFont;
};


#endif