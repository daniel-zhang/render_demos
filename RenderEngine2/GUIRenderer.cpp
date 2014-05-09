#include "GUIRenderer.h"
#include "EffectMgr.h"
#include "LayoutSolver.h"

GUIRenderer::GUIRenderer()
{
    mZBoundary = 0.01f;
    mUpdateBuffer = true;
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

void GUIRenderer::draw( std::vector<Widget*>& widgets)
{
    if (mUpdateBuffer)
    {
        mUpdateBuffer = false;
        std::vector<Vertex::OverlayVertex> vertices;
        Box2D widgetQuad;
        int widgetDepth = 0;
        RGBA color;

        UINT maxLayerNum = findMaxLayerNum(widgets); 

        for (UINT i = 0; i < widgets.size(); ++i)
        {
            widgets[i]->getRenderInfo(widgetQuad, widgetDepth, color);

            NdcBox2D ndcBox(widgetQuad, mViewport);
            for (UINT j = 0; j < 4; ++j)
            {
                Vertex::OverlayVertex v;
                v.Pos.x = ndcBox.point[j].x;
                v.Pos.y = ndcBox.point[j].y;
                v.Pos.z = getDepth(widgetDepth, maxLayerNum, mZBoundary);
                v.Color = color.normalize();
                vertices.push_back(v);
            }
        }
        mQuadBuffer.update(vertices);
    }

    mQuadBuffer.setPipeline();

    EffectMgr::OverlayFX->OverlayTech->GetPassByIndex(0)->Apply(0, mQuadBuffer.mCtx);

    // TODO: blending code not working, why?
    //float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    //mCtx->OMSetBlendState(RenderStateMgr::TransparentBS, blendFactor, 0xffffffff);
    mQuadBuffer.mCtx->DrawIndexed(mQuadBuffer.getIndexNum(), 0, 0);
}

void GUIRenderer::onViewportResize( Area2D& newArea )
{
    mViewport = newArea;
}

float GUIRenderer::getDepth( int layerIndex, int maxLayerNum, float zBoundary )
{
    return zBoundary * ( 1.f - static_cast<float>(layerIndex)/maxLayerNum);
}

UINT GUIRenderer::findMaxLayerNum( std::vector<Widget*>& widgets )
{
    UINT maxLayerNum = 0;
    for (UINT i = 0; i < widgets.size(); ++i)
    {
        UINT currDepth = widgets[i]->getLayerDepth();
        if ( currDepth > maxLayerNum)
            maxLayerNum = currDepth;
    }
    return maxLayerNum;
}

void GUIRenderer::setUpdate()
{
    mUpdateBuffer = true;
}
