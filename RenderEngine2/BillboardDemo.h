// Focus:
// How to build sprites in GS
// How to use the texture array
// Alpha-to-coverage: specify a 2D object's silhouette by alpha map
// so MXAA can be applied to 2d stuff
#ifndef BILLBOARD_DEMO_H
#define BILLBOARD_DEMO_H

#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Camera.h"
#include "Model.h"

class BillboardDemo : public RenderCore
{
public:
    BillboardDemo();
    ~BillboardDemo();
    virtual PCWSTR getClassName() const;
    virtual PCWSTR getWindowCaption() const;

    virtual bool init();
    virtual void onResize();
    virtual void updateScene(float dt);
    virtual void drawScene(); 

    virtual void onMouseDown(WPARAM btnState, int x, int y);
    virtual void onMouseUp(WPARAM btnState, int x, int y);
    virtual void onMouseMove(WPARAM btnState, int x, int y);

private:
    void buildGird();

private:
    //
    // Sprites
    //
    ID3D11Buffer* mTreeSpritesVB;
    ID3D11ShaderResourceView* mTreeTextureMapArraySRV;
    Material mTreeMat;
    bool mAlphaToCoverageOn;

    //
    // Grid 
    //
    Model* mpModel;

    //
    // Light 
    //
    DirectionalLight mDirLights[3];
    UINT mLightCount;
    XMFLOAT3 mEyePosWorld;

    //
    // Camera 
    //
    Camera mCam;

    //
    // User input
    //
    POINT mLastMousePos;
};
#endif