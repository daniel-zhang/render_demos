#ifndef DEMO_BASIC_H
#define DEMO_BASIC_H

#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Camera.h"
#include "Model.h"

class DemoBasic : public RenderCore
{
public:
    DemoBasic();
    ~DemoBasic();

    virtual PCWSTR getClassName() const;
    virtual PCWSTR getWindowCaption() const;

    virtual bool init();
    virtual void buildGird();
    virtual void updateScene(float dt);
    virtual void drawScene(); 

    virtual void onResize();
    virtual void onMouseDown(WPARAM btnState, int x, int y);
    virtual void onMouseUp(WPARAM btnState, int x, int y);
    virtual void onMouseMove(WPARAM btnState, int x, int y);
protected:
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