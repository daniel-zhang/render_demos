#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include "Util.h"

#include <vector>
#include <map>
#include <unordered_map>

#include "InputLayoutMgr.h"
#include "EffectMgr.h"

#include "IRenderable2D.h"
#include "Math2D.h"
#include "QuadBuffer.h"

// Render queue is sorted by texture into multiple RenderQueueSubsets
class RenderQueueSubset
{
public:
    RenderQueueSubset(){}
    RenderQueueSubset(int sortKey) 
    {
        mSortKey = sortKey;

        mIndexStart = 0;
        mIndexNum = 0;
        mMaxLayerDepth = 0;

        if (sortKey == -1)
        {
            mEffect = EffectMgr::OverlayFX;
            mTech = EffectMgr::OverlayFX->OverlayTech;
            mPassIndex = 0;
        }
        else
        {
            mEffect = EffectMgr::OverlayFX;
            mTech = EffectMgr::OverlayFX->OverlayTexTech;
            mPassIndex = 0;
        }
    }

    void clear()
    {
        mIndexStart = 0;
        mIndexNum = 0;
        mMaxLayerDepth = 0;
        mBucket.clear();
    }

    void add(IRenderable2D* sprite)
    {
        if (mMaxLayerDepth < sprite->mLayerDepth)
        {
            mMaxLayerDepth = sprite->mLayerDepth;
        }
        mBucket.push_back(sprite);
    }
    
    int mSortKey;

    UINT mIndexStart;
    UINT mIndexNum;
    UINT mMaxLayerDepth;

    std::wstring mTextureName;

    // TODO: generalize effect type, like this: 
    // Effect* mEffect;
    OverlayEffect* mEffect;
    ID3DX11EffectTechnique* mTech;
    UINT mPassIndex;

    std::vector<IRenderable2D*> mBucket;
};

class GUIRenderer
{
public:
    GUIRenderer();
    ~GUIRenderer();

    void draw(std::vector<IRenderable2D*>& spriteBatch);
    
    bool init(ID3D11Device* device, ID3D11DeviceContext* context);

    void beginBatch();
    void batch(IRenderable2D* sprite);
    void batch(std::vector<IRenderable2D*>& sprites);
    void sortedBatch(IRenderable2D* sprite);
    void endBatch();

    void draw();

    // Viewport size is used to transform from screen space to NDC space. Thus the
    // renderer is required to be informed when viewport size is changed.
    void onViewportResize(Area2D& newArea);

private:
    float getDepth(int layerIndex);
    void sortRenderQueueByTextureOrder();
    void sortRenderQueueByTextureOrder_debug();
    void sortRenderQueueByTextureOrder_hashmap();

    void updateQuadBuffer();
    void updateQuadBuffer2();
private:

    // GUI renderer places all its sprites in a depth range specified as: [0, mZBoudary]
    float mZBoundary;
    UINT mMaxLayerDepth;
    Area2D mViewport;

    std::vector<IRenderable2D*> mRenderQueue;
    std::vector<RenderQueueSubset> mSubsets;

    std::unordered_map<int, RenderQueueSubset> mSortSubsets;

    // The d3d11 VB/IB wrapper
    QuadBuffer<Vertex::OverlayVertex> mQuadBuffer;
};

#endif