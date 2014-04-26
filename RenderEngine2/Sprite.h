#ifndef xxx_xxx
#define xxx_xxx
#include "Util.h"
#include "InputLayoutMgr.h"

struct Quad
{
    Quad()
    {
        ZeroMemory(this, sizeof(Quad));
    }
    XMFLOAT2 mSize;
    XMFLOAT3 mCenter;
};

class SpriteGenertaor
{
public:
    SpriteGenertaor(ID3D11Device* dv, ID3D11DeviceContext* ctx);
    ~SpriteGenertaor(){}
    void buildVBFromQuads(std::vector<Quad>& quads);
    void drawSprite();

protected:
    bool mSpriteUpdated;
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mCtx;

    XMFLOAT2 mVPSize;

    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;
    std::vector<Quad> mQuads;

protected:
    XMFLOAT3 screenToNDC(XMFLOAT3 srcPos);
};



#endif