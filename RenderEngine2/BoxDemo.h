/*
Demo features
---------------------------------------------------
1. The Effect interface is used to organize the shader code:
Effect file(.fx)
----technique 'normal_map'
|---pass0
|---pass1
---|technique 'displacement'
|---pass0
|---pass1
|---pass2

2. Device context states are set via Effect interface:
ID3DX11EffectPass::Apply(UINT flag, ID3D11DeviceContext ctx) 

3. Shaders are compiled at run-time 
*/
#ifndef BOX_DEMO_H
#define BOX_DEMO_H
#include "d3dx11effect.h"
#include "MathHelper.h"
#include "RenderCore.h"

class BoxDemo : public RenderCore
{
public:
	BoxDemo();
	~BoxDemo();

	virtual PCWSTR getWindowCaption() const;

	bool init();
	void onResize();
	void updateScene(float dt);
	void drawScene();

	void onMouseDown(WPARAM btnState, int x, int y);
	void onMouseUp(WPARAM btnState, int x, int y);
	void onMouseMove(WPARAM btnState, int x, int y);

private:
	void buildGeometryBuffers();
	void buildFX();					// Build effect
	void buildVertexLayout();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta, mPhi, mRadius;
	POINT mLastMousePos;
};

#endif