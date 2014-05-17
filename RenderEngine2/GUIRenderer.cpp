#include "GUIRenderer.h"
#include "TextureMgr.h"

GUIRenderer::GUIRenderer()
{
    mZBoundary = 0.01f;
}

GUIRenderer::~GUIRenderer()
{

}

bool GUIRenderer::init( ID3D11Device* device, ID3D11DeviceContext* context )
{
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    context->RSGetViewports(&vpNum, &vp);
    mViewport = Area2D(static_cast<int>(vp.Width), static_cast<int>(vp.Height));

    return mQuadBuffer.init(device, context);
}

void GUIRenderer::onViewportResize( Area2D& newArea )
{
    mViewport = newArea;
}

float GUIRenderer::getDepth( int layerIndex )
{
    return mZBoundary* ( 1.f - static_cast<float>(layerIndex)/(mMaxLayerDepth+1));
}

void GUIRenderer::beginBatch()
{
    mSubsets.clear();
    mRenderQueue.clear();
    mMaxLayerDepth = 0;
    
    std::unordered_map<int, RenderQueueSubset>::iterator it = mSortSubsets.begin();
    for (; it != mSortSubsets.end(); ++it)
    {
        it->second.clear();
    }
}

void GUIRenderer::sortedBatch( IRenderable2D* sprite )
{
    // Discard invisible sprite
    if (!sprite->mVisible) return;

    // Keep track of layer number
    if (mMaxLayerDepth < sprite->mLayerDepth)
    {
        mMaxLayerDepth = sprite->mLayerDepth;
    }

    std::unordered_map<int, RenderQueueSubset>::iterator it = mSortSubsets.find(sprite->mSortKey);
    if (it == mSortSubsets.end())
    {
        RenderQueueSubset subset(sprite->mSortKey);
        subset.add(sprite);
        mSortSubsets.insert(std::pair<int, RenderQueueSubset>(sprite->mSortKey, subset));
    }
    else
    {
        it->second.add(sprite);
    }
}

void GUIRenderer::batch( IRenderable2D* sprite )
{
    // Discard invisible sprite
    if (!sprite->mVisible) return;
    mRenderQueue.push_back(sprite);
}

void GUIRenderer::batch( std::vector<IRenderable2D*>& sprites)
{
    for (UINT i = 0; i < sprites.size(); ++i)
    {
        // Discard invisible sprite
        if (!sprites[i]->mVisible) continue;
        mRenderQueue.push_back(sprites[i]);
    }
}

void GUIRenderer::endBatch()
{
    //sortRenderQueueByTextureOrder_hashmap();
    //sortRenderQueueByTextureOrder();
    //sortRenderQueueByTextureOrder_debug();
    //updateQuadBuffer();

    updateQuadBuffer2();
}

// Sort the render queue by texture order
void GUIRenderer::sortRenderQueueByTextureOrder()
{
    if (mRenderQueue.empty())
    {
        return;
    }

    // Insert the un-sorted render queue into a std::multimap<..>
    std::multimap<int, IRenderable2D*> sorter;
    for (UINT i = 0; i < mRenderQueue.size(); ++i)
    {
        IRenderable2D* sprite = mRenderQueue[i];
        sorter.insert(std::pair<int, IRenderable2D*>(sprite->mSortKey, sprite));

        // Additionally, find mMaxLayerDepth here
        UINT currDepth = sprite->mLayerDepth;
        if ( currDepth > mMaxLayerDepth)
            mMaxLayerDepth = currDepth;
    }

    //
    // mSubsets are populated in a 'multi-pass' manner:
    // first pass: mTextureName + Effect/Tech/Pass setting
    // second pass: index range
    //

    // first pass
    // Iterate the multimap to store all its unique keys into mSubsets;
    typedef std::multimap<int, IRenderable2D*>::iterator MapIter;
    for (MapIter it = sorter.begin(); it != sorter.end(); it = sorter.upper_bound(it->first))
    {
        RenderQueueSubset subset;
        subset.mSortKey = (it)->first;
        // Construct Effect/Tech/Pass setting. 
        // TODO: sprites with the same texture have only one Effect/Tech/Pass setting, fix this.
        if (it->second->mHasTexture)
        {
            subset.mEffect = EffectMgr::OverlayFX;
            subset.mTech = EffectMgr::OverlayFX->OverlayTexTech;
            subset.mPassIndex = 0;
            subset.mTextureName = (it)->second->mTextureName;
        }
        else
        {
            subset.mEffect = EffectMgr::OverlayFX;
            subset.mTech = EffectMgr::OverlayFX->OverlayTech;
            subset.mPassIndex = 0;
        }

        mSubsets.push_back(subset);
    }

    // second pass
    UINT quadOffset = 0;
    mRenderQueue.clear();
    for (UINT i = 0; i < mSubsets.size(); ++i)
    {
        // Compute index range and populate mSubsets
        mSubsets[i].mIndexStart = quadOffset * 6;
        UINT quadNum = sorter.count(mSubsets[i].mSortKey);
        mSubsets[i].mIndexNum = quadNum * 6;
        quadOffset += quadNum;

        // Sort the render queue
        std::pair<MapIter, MapIter> ret = sorter.equal_range(mSubsets[i].mSortKey);
        for (MapIter it = ret.first; it != ret.second; ++it)
        {
            mRenderQueue.push_back(it->second);
        }
    }
}

void GUIRenderer::updateQuadBuffer2()
{
    mVertices.clear();
    //vertices.reserve(mRenderQueue.size()*4);

    UINT spriteOffset = 0;
    std::unordered_map<int, RenderQueueSubset>::iterator it = mSortSubsets.begin();
    for (; it != mSortSubsets.end(); ++it)
    {
        it->second.mIndexStart = spriteOffset * 6;
        UINT bucketSize = it->second.mBucket.size();
        it->second.mIndexNum = bucketSize * 6;

        for (UINT i = 0; i < bucketSize; ++i)
        {
            IRenderable2D* sprite = it->second.mBucket[i];
            NdcBox2D ndcBox(sprite->mVisibleRect, mViewport);
            for (UINT j = 0; j < 4; ++j)
            {
                Vertex::OverlayVertex v;
                v.Pos.x = ndcBox.point[j].x;
                v.Pos.y = ndcBox.point[j].y;
                v.Pos.z = getDepth(sprite->mLayerDepth);

                v.Tex.x = sprite->mTextureRect.point[j].x;
                v.Tex.y = sprite->mTextureRect.point[j].y;

                v.Color = sprite->mColor.normalize();

                mVertices.push_back(v);
            }
            spriteOffset += 1;
        }
    }
    mQuadBuffer.update(mVertices);
}

void GUIRenderer::updateQuadBuffer()
{
    std::vector<Vertex::OverlayVertex> vertices;
    vertices.reserve(mRenderQueue.size()*4);

    // By here, render queue should be already sorted 
    for (UINT i = 0; i < mRenderQueue.size(); ++i)
    {
        // Transform quad form screen space to NDC space
        NdcBox2D widgetNdcQuad(mRenderQueue[i]->mVisibleRect, mViewport);

        for (UINT j = 0; j < 4; ++j)
        {
            Vertex::OverlayVertex v;
            v.Pos.x = widgetNdcQuad.point[j].x;
            v.Pos.y = widgetNdcQuad.point[j].y;
            v.Pos.z = getDepth(mRenderQueue[i]->mLayerDepth);

            v.Tex.x = mRenderQueue[i]->mTextureRect.point[j].x;
            v.Tex.y = mRenderQueue[i]->mTextureRect.point[j].y;

            v.Color = mRenderQueue[i]->mColor.normalize();

            vertices.push_back(v);
        }
    }
    mQuadBuffer.update(vertices);
}

void GUIRenderer::draw()
{
    mQuadBuffer.setPipeline();
    std::unordered_map<int, RenderQueueSubset>::iterator it = mSortSubsets.begin();
    for (; it != mSortSubsets.end(); ++it)
    {
        RenderQueueSubset*  bucket = &(it->second);
        if (bucket->mSortKey >= 0)
        {
            it->second.mEffect->setDiffuseMap(TextureMgr::getTexture(bucket->mSortKey));
        }
        bucket->mTech->GetPassByIndex(bucket->mPassIndex)->Apply(0, mQuadBuffer.mCtx);
        mQuadBuffer.mCtx->DrawIndexed(bucket->mIndexNum, bucket->mIndexStart, 0);
    }
}

void GUIRenderer::sortRenderQueueByTextureOrder_debug()
{
    if (mRenderQueue.empty())
    {
        return;
    }
    RenderQueueSubset subset;
    subset.mEffect = EffectMgr::OverlayFX;
    subset.mTech = EffectMgr::OverlayFX->OverlayTech;
    subset.mPassIndex = 0;
    subset.mSortKey = mRenderQueue[0]->mSortKey;
    subset.mIndexStart = 0;
    subset.mIndexNum = mRenderQueue.size() * 6;

    mSubsets.push_back(subset);
    for (UINT i = 0; i < mRenderQueue.size(); ++i)
    {
        // Additionally, find mMaxLayerDepth here
        UINT currDepth = mRenderQueue[i]->mLayerDepth;
        if ( currDepth > mMaxLayerDepth)
            mMaxLayerDepth = currDepth;
    }

}

void GUIRenderer::sortRenderQueueByTextureOrder_hashmap()
{
    if (mRenderQueue.empty())
    {
        return;
    }

    // Insert the un-sorted render queue into a std::multimap<..>
    std::unordered_multimap<int, IRenderable2D*> sorter;
    for (UINT i = 0; i < mRenderQueue.size(); ++i)
    {
        IRenderable2D* sprite = mRenderQueue[i];
        sorter.insert(std::pair<int, IRenderable2D*>(sprite->mSortKey, sprite));

        // Additionally, find mMaxLayerDepth here
        UINT currDepth = sprite->mLayerDepth;
        if ( currDepth > mMaxLayerDepth)
            mMaxLayerDepth = currDepth;
    }

    //
    // mSubsets are populated in a 'multi-pass' manner:
    // first pass: mTextureName + Effect/Tech/Pass setting
    // second pass: index range
    //

    // first pass
    // Iterate the multimap to store all its unique keys into mSubsets;
    typedef std::unordered_multimap<int, IRenderable2D*>::iterator MapIter;
    for (MapIter it = sorter.begin(); it != sorter.end(); it = sorter.upper_bound(it->first))
    {
        RenderQueueSubset subset;
        subset.mSortKey = (it)->first;
        // Construct Effect/Tech/Pass setting. 
        // TODO: sprites with the same texture have only one Effect/Tech/Pass setting, fix this.
        if (it->second->mHasTexture)
        {
            subset.mEffect = EffectMgr::OverlayFX;
            subset.mTech = EffectMgr::OverlayFX->OverlayTexTech;
            subset.mPassIndex = 0;
            subset.mTextureName = (it)->second->mTextureName;
        }
        else
        {
            subset.mEffect = EffectMgr::OverlayFX;
            subset.mTech = EffectMgr::OverlayFX->OverlayTech;
            subset.mPassIndex = 0;
        }

        mSubsets.push_back(subset);
    }

    // second pass
    UINT quadOffset = 0;
    mRenderQueue.clear();
    for (UINT i = 0; i < mSubsets.size(); ++i)
    {
        // Compute index range and populate mSubsets
        mSubsets[i].mIndexStart = quadOffset * 6;
        UINT quadNum = sorter.count(mSubsets[i].mSortKey);
        mSubsets[i].mIndexNum = quadNum * 6;
        quadOffset += quadNum;

        // Sort the render queue
        std::pair<MapIter, MapIter> ret = sorter.equal_range(mSubsets[i].mSortKey);
        for (MapIter it = ret.first; it != ret.second; ++it)
        {
            mRenderQueue.push_back(it->second);
        }
    }
}
