/*
Demo features
---------------------------------------------------
1. Usage of GeometryGenerator to generate a grid
2. Shaders are compiled at compile-time and read via std::fin 
3. Shows how to setup customized build tool in VS2010
*/

#ifndef GRID_DEMO_H
#define GRID_DEMO_H

#include "RenderCore.h"
#include "d3dx11effect.h"
#include "MathHelper.h"

#include "GeometryGenerator.h"


class GridDemo : public RenderCore
{
public:
	GridDemo();
	~GridDemo();

	virtual PCWSTR getWindowCaption() const;

	bool init();
	void onResize();
	void updateScene(float dt);
	void drawScene();

	void onMouseDown(WPARAM btnState, int x, int y);
	void onMouseUp(WPARAM btnState, int x, int y);
	void onMouseMove(WPARAM btnState, int x, int y);

private:
	float getHeight(float x, float z)const;
	void buildGeometryBuffers();
	void buildFX();					// Build effect
	void buildVertexLayout();

private:	
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3D11InputLayout* mInputLayout;

	UINT mGridIndexCount;

	XMFLOAT4X4 mGridWorld; // Each obj has a unique world matrix
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta, mPhi, mRadius;
	POINT mLastMousePos;
};


#endif