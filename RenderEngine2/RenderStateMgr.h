#ifndef RENDER_STATE_MGR_H
#define RENDER_STATE_MGR_H

#include "Util.h"

class RenderStateMgr
{
public:
    static void initAll(ID3D11Device* device);
    static void destroyAll();

    static ID3D11RasterizerState* WireframeRS;
    static ID3D11RasterizerState* NoCullRS;
    static ID3D11RasterizerState* ScissorRS;

    static ID3D11DepthStencilState* NoDepthTestDS;

    static ID3D11BlendState* AlphaToCoverageBS;
    static ID3D11BlendState* TransparentBS;
};

#endif
