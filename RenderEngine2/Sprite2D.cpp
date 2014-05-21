#include "Sprite2D.h"
#include "SpriteRenderer.h"
#include "RenderStateMgr.h"
//#include "Util.h"

Sprite2D::Sprite2D() :
    mIsClipped(false), mIsSynced(false), mTextureManagedExternally(false),
    mSRV(0), mVB(0), mIB(0), 
    mEffect(0), mTech(0)
{
    mVertices.reserve(4);
}

Sprite2D::~Sprite2D()
{
    clearGeometryBuffer();
}

bool Sprite2D::init( D3DEnv& env )
{
    return createGeometryBuffer(env);
}

void Sprite2D::move( Vector2D& movement )
{
    mDstBox.move(movement);

    mIsSynced = false;
}

void Sprite2D::resize( Area2D& newSize )
{
    mDstBox.resize(newSize);
    mIsSynced = false;
}

void Sprite2D::setEffect( OverlayEffect* effect )
{
    mEffect = effect;
    mIsSynced = false;
}

void Sprite2D::setTechique( ID3DX11EffectTechnique* tech )
{
    mTech = tech;
    mIsSynced = false;
}

void Sprite2D::setDstBox( Box2D& box )
{
    mDstBox = box;
    mIsSynced = false;
}

void Sprite2D::setTexture( ID3D11ShaderResourceView* srv, FBox2D& srcBox )
{
    mSRV = srv;
    mSrcBox = srcBox;
}

void Sprite2D::setClipBox( Box2D& clipBox )
{
    mClipBox = clipBox;
}

void Sprite2D::enableClip()
{
    mIsClipped = true;
    mIsSynced = false;
}
void Sprite2D::disableClip()
{
    mIsClipped = false;
    mIsSynced = false;
}

void Sprite2D::setColor( const RGBA& color )
{
    mVertexColor = color; 
    mIsSynced = false;
}

bool Sprite2D::createGeometryBuffer(D3DEnv& env)
{
    //
    // Create dynamic VB
    //
    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof (Vertex::OverlayVertex) * 4;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    vbd.Usage = D3D11_USAGE_DYNAMIC; // make dynamic
    HR(env.device->CreateBuffer(&vbd, 0, &mVB));

    //
    // Create static IB
    //
    UINT spriteNum = 1;
    std::vector<UINT> indices(spriteNum * 6, 0);
    for (UINT i = 0; i < spriteNum; ++i)
    {
        indices[i*6+0] = i*4+0;
        indices[i*6+1] = i*4+1;
        indices[i*6+2] = i*4+2;
        indices[i*6+3] = i*4+0;
        indices[i*6+4] = i*4+2;
        indices[i*6+5] = i*4+3;
    }
    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(UINT) * spriteNum * 6;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA idata;
    idata.pSysMem = &indices[0];
    HR(env.device->CreateBuffer(&ibd, &idata, &mIB));

    return true;
}

void Sprite2D::clearGeometryBuffer()
{
    safe_release(&mVB);
    safe_release(&mIB);
}

void Sprite2D::syncGeomtryBuffer( Area2D& vpSize, D3DEnv& env)
{
    if (!mIsSynced)
    {
        // Transform screen boxes to NDC space and update local vertices
        NdcBox2D ndcDstBox(mDstBox, vpSize);
        mVertices.clear();
        for (UINT i = 0; i < 4; ++i)
        {
            Vertex::OverlayVertex v;
            v.Pos.x = ndcDstBox.point[i].x;
            v.Pos.y = ndcDstBox.point[i].y;
            v.Pos.z = 0.1f;

            v.Tex.x = mSrcBox.point[i].x;
            v.Tex.y = mSrcBox.point[i].y;

            v.Color = mVertexColor.normalize();

            mVertices.push_back(v);
        }

        // Map local vertices to VB
        D3D11_MAPPED_SUBRESOURCE mappedData;
        env.context->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
        Vertex::OverlayVertex* vStart = reinterpret_cast<Vertex::OverlayVertex*>(mappedData.pData);
        for (UINT i = 0; i < mVertices.size(); ++i)
        {
            vStart[i] = mVertices[i];
        }
        env.context->Unmap(mVB, 0);

        mIsSynced = true;
    }
}

void Sprite2D::beforeDraw( D3DEnv& env )
{
    // An optimization for text rendering:
    // A group of text sprites generally share the same  texture(font alas). Instead of 
    // managing texture by the sprite itself, which brings overhead that is linear to the 
    // size of the sprite group, the shared texture is set and cleared externally by some 
    // manager through out the rendering of the sprite group.
    if (!mTextureManagedExternally && mSRV)
    {
        mEffect->setDiffuseMap(mSRV);
    }
    mTech->GetPassByIndex(0)->Apply(0, env.context);

    if (mIsClipped)
    {
        env.context->RSSetState(RenderStateMgr::ScissorRS);

        D3D11_RECT clip;
        clip.left   = static_cast<LONG>(mClipBox.getLeft());
        clip.top    = static_cast<LONG>(mClipBox.getTop());
        clip.right  = static_cast<LONG>(mClipBox.getRight());
        clip.bottom = static_cast<LONG>(mClipBox.getBottom());
        env.context->RSSetScissorRects(1, &clip);
    }
    float blendFactor[4] = {1.f};
    //env.context->OMSetBlendState(RenderStateMgr::AlphaToCoverageBS, blendFactor, 0xffffffff);
    env.context->OMSetBlendState(RenderStateMgr::TransparentBS, blendFactor, 0xffffffff);
}

void Sprite2D::afterDraw( D3DEnv& env )
{
    if (mIsClipped)
    {
        env.context->RSSetState(0);
    }

    float blendFactor[4] = {1.f};
    env.context->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void Sprite2D::enableExternalTextureManagement()
{
    mTextureManagedExternally = true;
}

void Sprite2D::disableExternalTextureManagement()
{
    mTextureManagedExternally = false;
}

Box2D& Sprite2D::getDstBox()
{
    return mDstBox;
}


