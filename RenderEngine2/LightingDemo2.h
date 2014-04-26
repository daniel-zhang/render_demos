// 
//
#ifndef LIGHTING_DEMO_2_H
#define LIGHTING_DEMO_2_H

#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Waves.h"
#include "Camera.h"

class LightingDemo2 : public RenderCore
{
public:
    LightingDemo2();
    ~LightingDemo2();

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
    void buildSceneBuffers();

    void buildSkull();
    void buildBox();
    void buildLand();
    void buildWave();
    void buildCigar();

    float getHillHeight(float x, float z)const;
    XMFLOAT3 getHillNormal(float x, float z)const;

private:
    // Skull 
    // Mesh:    disk file
    // Mat:     yes
    // Tex:     no 
    ID3D11Buffer* mSkullVB;
    ID3D11Buffer* mSkullIB;
    UINT mSkullIndexCount;
    XMFLOAT4X4 mSkullWorld;
    Material mSkullMaterial;

    // CigarPack
    // Mesh: tex-coord is manually tweaked
    // Mat: yes
    // Tex: yes
    ID3D11Buffer* mCigVB;
    ID3D11Buffer* mCigIB;
    int mCigVertexOffset;
    UINT mCigIndexOffset;
    UINT mCigIndexCount;
    XMFLOAT4X4 mCigWorld;
    Material mCigMaterial; 
    ID3D11ShaderResourceView* mCigMapSRV;

    // Box
    // Mesh:    computationally constructed
    // Mat:     yes
    // Tex:     yes
    ID3D11Buffer* mBoxVB;
    ID3D11Buffer* mBoxIB;
    int mBoxVertexOffset;
    UINT mBoxIndexOffset;
    UINT mBoxIndexCount;
    XMFLOAT4X4 mBoxWorld;
    Material mBoxMaterial;
    ID3D11ShaderResourceView* mBoxMapSRV;

    // Terrain
    // Mesh:    computationally constructed
    // Mat:     yes
    // Tex:     TODO - transformed?
    ID3D11Buffer* mLandVB;
    ID3D11Buffer* mLandIB;
    UINT mLandIndexCount;
    XMFLOAT4X4 mLandWorld;
    Material mLandMaterial;
    ID3D11ShaderResourceView* mGrassMapSRV;
    XMFLOAT4X4 mGrassTexTransform;

    // Waves
    // Mesh:    computationally constructed
    // Mat:     yes
    // Tex:     TODO
    Waves mWaves;
    ID3D11Buffer* mWavesVB;
    ID3D11Buffer* mWavesIB;
    XMFLOAT4X4 mWavesWorld;
    Material mWavesMaterial;
    ID3D11ShaderResourceView* mWavesMapSRV;
    XMFLOAT2 mWaterTexOffset;
    XMFLOAT4X4 mWaterTexTransform;

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
    //XMFLOAT4X4 mView;
    //XMFLOAT4X4 mProj;
    //float mTheta;
    //float mPhi;
    //float mRadius;

    Camera mCam;

    //
    // User input
    //
    POINT mLastMousePos;
};

#endif