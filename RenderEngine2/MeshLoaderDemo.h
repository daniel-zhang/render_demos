/*
1. load mesh from simple text file
*/
#ifndef MESH_LOADER_DEMO_H
#define MESH_LOADER_DEMO_H

#include "RenderCore.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"

class MeshLoaderDemo : public RenderCore
{
public:
	MeshLoaderDemo();
	~MeshLoaderDemo();

	virtual bool init();
	virtual void onResize();
	virtual void updateScene(float dt);
	virtual void drawScene();

	virtual void onMouseDown(WPARAM btnState, int x, int y);
	virtual void onMouseUp(WPARAM btnState, int x, int y);
	virtual void onMouseMove(WPARAM btnState, int x, int y);

private:
	void buildGeometryBuffers();
	void buildEffect();
	void buildVertexLayout();

private:
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11InputLayout* mInputLayout;

	ID3D11RasterizerState* mWireframeRS;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSkullWorld;

	UINT mSkullIndexCount;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

#endif