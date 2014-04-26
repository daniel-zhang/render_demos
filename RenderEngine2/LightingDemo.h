// Contains a wave simulation that is lit by directional/spot/point light
// 
#ifndef LIGHTING_DEMO_H
#define LIGHTING_DEMO_H

#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Waves.h"

class LightingDemo : public RenderCore
{
public:
	LightingDemo();
	~LightingDemo();

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
	void buildWaveGeometryBuffers();
	void buildFX();
	void buildVertexLayout();

private:
	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mWavesIB;

	Waves mWaves;
	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;
	Material mWavesMat;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;
	ID3DX11EffectVectorVariable* mfxEyePosW;
	ID3DX11EffectVariable* mfxDirLight;
	ID3DX11EffectVariable* mfxPointLight;
	ID3DX11EffectVariable* mfxSpotLight;
	ID3DX11EffectVariable* mfxMaterial;

	ID3D11InputLayout* mInputLayout;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mWavesWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

#endif