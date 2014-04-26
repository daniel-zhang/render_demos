#ifndef OBJ_LOADER_DEMO_H
#define OBJ_LOADER_DEMO_H

#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "Camera.h"
#include "LightHelper.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "Model.h"

//#include "ObjLoader.h"

class ObjLoaderDemo: public RenderCore
{
public:
    ObjLoaderDemo();
    ~ObjLoaderDemo();

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
    //
    // Model
    //
    Model* mpModel;

    //
    // Light 
    //
    DirectionalLight mDirLights[3];
    UINT mLightCount;
    XMFLOAT3 mEyePosWorld;

    //
    // RenderOptions
    //
    RenderOptions mRenderOptions;

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