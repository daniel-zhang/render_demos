#include "BoxDemo.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

BoxDemo::BoxDemo():
	mBoxVB(0),
	mBoxIB(0),
	mFX(0),
	mTech(0),
	mfxWorldViewProj(0),
	mInputLayout(0),
	mTheta(1.5f * MathHelper::Pi),
	mPhi(0.25f * MathHelper::Pi),
	mRadius(5.f)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX identityMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, identityMatrix);
	XMStoreFloat4x4(&mView, identityMatrix);
	XMStoreFloat4x4(&mProj, identityMatrix);
}

BoxDemo::~BoxDemo()
{
	safe_release(&mBoxVB);
	safe_release(&mBoxIB);
	safe_release(&mFX);
	safe_release(&mInputLayout);
}

PCWSTR BoxDemo::getWindowCaption() const
{
	return L"Box Demo";
}

bool BoxDemo::init()
{
	if (!RenderCore::init())
		return false;

	buildGeometryBuffers();
	buildFX();
	buildVertexLayout();

	return true;
}

void BoxDemo::onResize()
{
	RenderCore::onResize();

	// If window is resized, then aspect ratio and projection matrix should be updated
	XMMATRIX projMatrix= XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio(), 1.f, 1000.f);
	XMStoreFloat4x4(&mProj, projMatrix);
}

void BoxDemo::updateScene(float dt)
{
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.f);		// Cam position
	XMVECTOR target = XMVectorZero();				// Looking at
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);	// Upward

	XMMATRIX viewMatrix = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, viewMatrix);
}

void BoxDemo::drawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	// Set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX world_view_proj = world * view	* proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&world_view_proj));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}
	HR(mSwapChain->Present(0, 0));

}

void BoxDemo::onMouseDown( WPARAM btnState, int x, int y )
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mHwnd);
}

void BoxDemo::onMouseUp( WPARAM btnState, int x, int y )
{
	ReleaseCapture();
}

void BoxDemo::onMouseMove( WPARAM btnState, int x, int y )
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(mLastMousePos.x - x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(mLastMousePos.y - y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxDemo::buildGeometryBuffers()
{
	// Create vertex buffer
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}

void BoxDemo::buildFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"../FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if( compilationMsgs != 0 )
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		safe_release(&compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, md3dDevice, &mFX));

	// Done with compiled shader.
	safe_release(&compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void BoxDemo::buildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout, note that the signature is obtained from mTech.pass(0)
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
}
