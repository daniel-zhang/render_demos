/*
Demo features
---------------------------------------------------
1. Dynamic vertex buffer
2. 

*/
#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "WavesSimple.h"

class WaveDemo : public RenderCore
{
public:
	WaveDemo();
	~WaveDemo();

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
	float getTerrainHeight(float x, float z)const;
	void buildTerrainBuffers();
	void buildWavesBuffers();
	void buildEffect();
	void buildVertexLayout();

private:
	// Effect framework
	ID3DX11Effect* mEffect;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWVP;

	// Pipeline specific
	ID3D11InputLayout* mInputLayout;
	ID3D11RasterizerState* mWireframeRS;

	// Terrain and waves mesh
	ID3D11Buffer* mTerrainVertexBuffer;
	ID3D11Buffer* mTerrainIndexBuffer;

	ID3D11Buffer* mWavesVertexBuffer;
	ID3D11Buffer* mWavesIndexBuffer;

	UINT mTerrainIndexCount;	// Waves' index count is cached in mWaves

	// Waves generator
	WavesSimple mWaves;

	// World location of terrain and waves
	XMFLOAT4X4	mTerrainWorld, mWavesWorld;

	// Camera
	XMFLOAT4X4 mView, mProj;
	float mTheta, mPhi, mRadius;

	// Mouse input
	POINT mLastMousePos;
};