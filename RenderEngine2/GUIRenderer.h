#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include <vector>
#include "InputLayoutMgr.h"
#include "Widget.h"
#include "Math2D.h"
#include "QuadBuffer.h"
#include "Util.h"

class LayoutSolver;

class GUIRenderer
{
public:
    GUIRenderer();
    ~GUIRenderer();

    bool init(ID3D11Device* device, ID3D11DeviceContext* context);
    void draw(std::vector<Widget*>& widgets);

    void setUpdate();

    void onViewportResize(Area2D& newArea);
private:
    UINT findMaxLayerNum(std::vector<Widget*>& widgets);
    float getDepth(int layerIndex, int maxLayerNum, float zBoundary);

private:
    // Boolean flag that is externally set to inform the renderer to update its VB
    bool mUpdateBuffer;
    float mZBoundary;
    Area2D mViewport;
    QuadBuffer<Vertex::OverlayVertex> mQuadBuffer;
};

#endif