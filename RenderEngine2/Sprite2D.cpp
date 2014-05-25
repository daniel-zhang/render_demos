#include "Sprite2D.h"
#include "SpriteRenderer.h"
#include "RenderStateMgr.h"
//#include "Util.h"

Sprite2D::Sprite2D() :
    mIsClipped(false), mTextureManagedExternally(false),
    mSRV(0), mVB(0), mIB(0), mEnv(0), mClipBox(0),
    mEffect(0), mTech(0)
{
    mVertices.reserve(4);
    mDynamic = false;

    XMStoreFloat4x4(&mTranslation, XMMatrixTranslation(0.f, 0.f, 0.f));
    XMStoreFloat4x4(&mScale, XMMatrixScaling(1.f, 1.f, 1.f));
}

Sprite2D::~Sprite2D()
{
    clearGeometryBuffer();
}

bool Sprite2D::init( D3DEnv* env, Point2D& pos, Area2D& size, const RGBA& color, OverlayEffect* effect, ID3DX11EffectTechnique* tech, bool useDynamicVB /*= false*/, ID3D11ShaderResourceView* srv /*= NULL*/, FBox2D& srcBox /*= FBox2D() */ )
{
    mEnv = env;
    mVertexColor = color; 
    mEffect = effect;
    mTech = tech;

    ////////////////////////////////////
    // Only required by static sprite //
    getViewportSize(mInitialVP);
    mReferenceRect.zero();
    mReferenceRect.buildFromScreenArea(size, mInitialVP);

    mCacheRect.zero();
    mCacheRect.resize(size);
    mCacheRect.moveTo(pos);
    
    mSRV = srv;
    mSrcBox = srcBox; 
    // Only required by static sprite //
    ////////////////////////////////////

    mDynamic = useDynamicVB;
    if(mDynamic)
    {
        createDynamicGeometryBuffer();
        syncDynamicGeomtryBuffer();
    }
    else
    {
        createStaticGeometryBuffer();
    }
    return true;
}


void Sprite2D::move( Vector2D& movement )
{
    mCacheRect.move(movement);
}

void Sprite2D::moveTo( Point2D& pos )
{
    Vector2D movement = pos - mCacheRect.point[0];
    move(movement);
}

void Sprite2D::resize( Area2D& newSize )
{
    mCacheRect.resize(newSize);
}

void Sprite2D::setEffect( OverlayEffect* effect )
{
    mEffect = effect;
}

void Sprite2D::setTechique( ID3DX11EffectTechnique* tech )
{
    mTech = tech;
}

void Sprite2D::setTexture( ID3D11ShaderResourceView* srv )
{
    mSRV = srv;
}

void Sprite2D::setColor( const RGBA& color )
{
    mVertexColor = color; 
}

void Sprite2D::setClipBox( Box2D* clipBox )
{
    mClipBox = clipBox;
}

void Sprite2D::enableClip()
{
    mIsClipped = true;
}
void Sprite2D::disableClip()
{
    mIsClipped = false;
}

bool Sprite2D::createDynamicGeometryBuffer()
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
    HR(mEnv->device->CreateBuffer(&vbd, 0, &mVB));

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
    HR(mEnv->device->CreateBuffer(&ibd, &idata, &mIB));

    return true;
}

bool Sprite2D::createStaticGeometryBuffer()
{
    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof (Vertex::OverlayVertex) * 4;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    vbd.Usage = D3D11_USAGE_IMMUTABLE; // make dynamic
    D3D11_SUBRESOURCE_DATA vdata;
    mVertices.clear();
    for (UINT i = 0; i < 4; ++i)
    {
        Vertex::OverlayVertex v;
        v.Pos.x = mReferenceRect.point[i].x;
        v.Pos.y = mReferenceRect.point[i].y;
        v.Pos.z = 0.0f;

        v.Tex.x = mSrcBox.point[i].x;
        v.Tex.y = mSrcBox.point[i].y;

        v.Color = mVertexColor.normalize();

        mVertices.push_back(v);
    }
    vdata.pSysMem = &mVertices[0];

    HR(mEnv->device->CreateBuffer(&vbd, &vdata, &mVB));

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
    HR(mEnv->device->CreateBuffer(&ibd, &idata, &mIB));

    return true;
}

void Sprite2D::clearGeometryBuffer()
{
    safe_release(&mVB);
    safe_release(&mIB);
}

void Sprite2D::syncDynamicGeomtryBuffer()
{
    // Transform screen boxes to NDC space and update local vertices
    mVertices.clear();
    for (UINT i = 0; i < 4; ++i)
    {
        Vertex::OverlayVertex v;
        v.Pos.x = mReferenceRect.point[i].x;
        v.Pos.y = mReferenceRect.point[i].y;
        v.Pos.z = 0.0f;

        v.Tex.x = mSrcBox.point[i].x;
        v.Tex.y = mSrcBox.point[i].y;

        v.Color = mVertexColor.normalize();

        mVertices.push_back(v);
    }

    // Map local vertices to VB
    D3D11_MAPPED_SUBRESOURCE mappedData;
    mEnv->context->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    Vertex::OverlayVertex* vStart = reinterpret_cast<Vertex::OverlayVertex*>(mappedData.pData);
    for (UINT i = 0; i < mVertices.size(); ++i)
    {
        vStart[i] = mVertices[i];
    }
    mEnv->context->Unmap(mVB, 0);
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

    if (mIsClipped && mClipBox)
    {
        env.context->RSSetState(RenderStateMgr::ScissorRS);

        D3D11_RECT clip;
        clip.left   = static_cast<LONG>(mClipBox->getLeft());
        clip.top    = static_cast<LONG>(mClipBox->getTop());
        clip.right  = static_cast<LONG>(mClipBox->getRight());
        clip.bottom = static_cast<LONG>(mClipBox->getBottom());
        env.context->RSSetScissorRects(1, &clip);
    }

    // Default: transparency enabled
    float blendFactor[4] = {1.f};
    env.context->OMSetBlendState(RenderStateMgr::TransparentBS, blendFactor, 0xffffffff);

    syncTransformation();
    mEffect->setScale(XMLoadFloat4x4(&mScale));
    mEffect->setTranslation(XMLoadFloat4x4(&mTranslation));
    mEffect->setColor(XMLoadFloat4(&mVertexColor.normalize()));

    mTech->GetPassByIndex(0)->Apply(0, env.context);
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

void Sprite2D::getViewportSize( Area2D& vpSize )
{
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    mEnv->context->RSGetViewports(&vpNum, &vp);
    vpSize.width =  static_cast<int>(vp.Width);
    vpSize.height = static_cast<int>(vp.Height);
}

void Sprite2D::syncTransformation()
{
    Area2D vpSize;
    getViewportSize(vpSize);

    // Compute a compensate scale to keep sprite size independent of viewport size
    float vpScaleX = static_cast<float>(mInitialVP.width)/vpSize.width;
    float vpScaleY = static_cast<float>(mInitialVP.height)/vpSize.height;

    float modelScaleX = static_cast<float>(mCacheRect.getWidth())/mReferenceRect.getScreenWidth(mInitialVP);
    float modelScaleY = static_cast<float>(mCacheRect.getHeight())/mReferenceRect.getScreenHeight(mInitialVP);

    XMStoreFloat4x4(&mScale, XMMatrixScaling(vpScaleX * modelScaleX, vpScaleY * modelScaleY, 1.f));

    FPoint2D ndcVec = mCacheRect.point[0].toNDC(vpSize);
    XMStoreFloat4x4(&mTranslation, XMMatrixTranslation(ndcVec.x, ndcVec.y, 0.f));
}

Box2D* Sprite2D::getCacheBox()
{
    return &mCacheRect;
}

void Sprite2D::dynamicSync( Point2D& pos, Area2D& size, FBox2D& srcBox, ID3D11ShaderResourceView* srv )
{
    getViewportSize(mInitialVP);
    mReferenceRect.zero();
    mReferenceRect.buildFromScreenArea(size, mInitialVP);

    mCacheRect.zero();
    mCacheRect.resize(size);
    mCacheRect.moveTo(pos);

    mSRV = srv;
    mSrcBox = srcBox;

    syncDynamicGeomtryBuffer();
}




