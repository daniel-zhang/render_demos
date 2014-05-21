#include "SpriteRenderer.h"
#include "InputLayoutMgr.h"
#include "RenderStateMgr.h"

SpriteRenderer::SpriteRenderer()
{

}

SpriteRenderer::~SpriteRenderer()
{

}

bool SpriteRenderer::init( ID3D11Device* device, ID3D11DeviceContext* ctx )
{
    mEnv.context = ctx;
    mEnv.device = device;

    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    mEnv.context->RSGetViewports(&vpNum, &vp);
    mVpSize = Area2D(static_cast<int>(vp.Width), static_cast<int>(vp.Height));

    return true;
}

void SpriteRenderer::draw( Sprite2D& sprite )
{
    _beforeDraw();
    _draw(sprite);
    _afterDraw();
}

void SpriteRenderer::onViewportResize( Area2D& newArea )
{
    mVpSize = newArea;
}

D3DEnv& SpriteRenderer::getRenderEnv() 
{
    return mEnv;
}

void SpriteRenderer::_beforeDraw()
{
    mEnv.context->OMSetDepthStencilState(RenderStateMgr::NoDepthTestDS, 0);
}

void SpriteRenderer::_draw( Sprite2D& sprite )
{
    mEnv.context->IASetInputLayout(InputLayoutMgr::OverlayVertex);
    mEnv.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex::OverlayVertex);
    UINT offset = 0;
    mEnv.context->IASetVertexBuffers(0, 1, &(sprite.mVB), &stride, &offset);
    mEnv.context->IASetIndexBuffer(sprite.mIB, DXGI_FORMAT_R32_UINT, 0);

    sprite.syncGeomtryBuffer(mVpSize, mEnv);
    sprite.beforeDraw(mEnv);
    mEnv.context->DrawIndexed(6, 0, 0);
    sprite.afterDraw(mEnv);
}

void SpriteRenderer::_afterDraw()
{
    mEnv.context->OMSetDepthStencilState(0, 0);
}
