/*
Demo features
---------------------------------------------------
1. Merge multiple objects vertex/index buffers into one

2. Enable wire-frame mode in RS stage

3. GeometryGenerator that creates cylinders and spheres

4. One obj can have multiple instances with different world matrices

---------------------------------------------------
Triangle A's vertex buffer: {Va, Vb, Vc}, index buffer{0, 1, 2}
The winding order of A is Va->Vb->Vc

Triangle B's vertex buffer: {Vd, Ve, Vf}, index buffer{2, 1, 0}
The winding order of B is Vf->Ve->Vd

The two vertex buffers can be concentrated into:
{Va, Vb, Vc, Vd, Ve, Vf}

As for the index buffer, triangle A's index buffer can remain the same, while triangle 
B's index buffer should be increased by 3:
[0---1---2---5---4---3-]

However it is tedious to re-compute index buffers. We can pass the index buffer as it is:
[0---1---2---2---1---0-]
as long as an offset array is available:
{A_offset: 0, B_offset:3}

So the graphic hardware can compensate the index buffers:
[0---1---2---2---1---0-]
+
[0---0---0---3---3---3-]
=
[0---1---2---5---4---3-]
*/
#ifndef SCENE_DEMO_H
#define SCENE_DEMO_H

#include "RenderCore.h"
#include "d3dx11effect.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"

class SceneDemo : public RenderCore
{
public:
	SceneDemo();
	~SceneDemo();

	virtual PCWSTR getWindowCaption() const;

	virtual bool init();
	virtual void onResize();
	virtual void updateScene(float dt);
	virtual void drawScene();

	virtual void onMouseDown(WPARAM btnState, int x, int y);
	virtual void onMouseUp(WPARAM btnState, int x, int y);
	virtual void onMouseMove(WPARAM btnState, int x, int y);

private:
	void buildGeometryBuffers();	// Build the geometries to be drawn
	void buildFX();					// Build the D3D Effect framework
	void buildVertexLayout();		//

private:	
	// Defines the position of the virtual camera
	float							mTheta, mPhi, mRadius;
	POINT							mLastMousePos;

	// A single vertex and index buffer that stores all the objects in the scene
	ID3D11Buffer*					mVertexBuffer;
	ID3D11Buffer*					mIndexBuffer;

	ID3DX11Effect*					mFX;				// D3D Effects interface
	ID3DX11EffectTechnique*			mTech;				// Technique defined in the FX file
	ID3DX11EffectMatrixVariable*	mfxWorldViewProj;	// The combined transform matrix

	ID3D11InputLayout*				mInputLayout;		// describes the input vertex buffer and shader signature

	ID3D11RasterizerState*			mWireframeRS;

	// Define transformations from local spaces to world space.
	// Each object in the scene has its own world matrix
	// Note that spheres and cylinders are instanced.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mCenterSphere;

	// The view and projection matrices are the same
	XMFLOAT4X4						mView;
	XMFLOAT4X4						mProj;

	// The start position of each object in the vertex buffer  
	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	// The start position of each object in the index buffer
	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	// Number of indices of each object
	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;


};
#endif
