#include "WaveDemo.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

WaveDemo::WaveDemo():
	mEffect(0),
	mTech(0),
	mfxWVP(0),

	mInputLayout(0),
	mWireframeRS(0),

	mTerrainVertexBuffer(0),
	mTerrainIndexBuffer(0),
	mWavesVertexBuffer(0),
	mWavesIndexBuffer(0),

	mTerrainIndexCount(0),
	mTheta(1.5f*MathHelper::Pi), 
	mPhi(0.1f*MathHelper::Pi), 
	mRadius(200.0f)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mTerrainWorld, I);
	XMStoreFloat4x4(&mWavesWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}

WaveDemo::~WaveDemo()
{
	safe_release(&mTerrainVertexBuffer);
	safe_release(&mTerrainIndexBuffer);
	safe_release(&mWavesVertexBuffer);
	safe_release(&mWavesIndexBuffer);
	safe_release(&mEffect);
	safe_release(&mInputLayout);
	safe_release(&mWireframeRS);
}

PCWSTR WaveDemo::getClassName() const
{
	return L"WaveDemo";
}

PCWSTR WaveDemo::getWindowCaption() const
{
	return L"Waves Demo";
}

bool WaveDemo::init() 
{
	if (RenderCore::init() == false)
	{
		return false;
	}

	buildTerrainBuffers();

	mWaves.init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);
	buildWavesBuffers();

	buildEffect();
	buildVertexLayout();

	//
	// Create RasterizerState that enables wire-frame rendering
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;
	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

	return true;
}

void WaveDemo::onResize()
{
	RenderCore::onResize();

	//
	// Recompute the projection matrix according to the updated size of window
	//
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, aspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void WaveDemo::updateScene( float dt )
{
	//
	// Recompute the view matrix according the updated position of camera
	//

	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	//
	// Every quarter second, generate a random wave.
	//
	static float t_base = 0.0f;
	if( (mTimer.totalTime() - t_base) >= 0.25f )
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		float r = MathHelper::RandF(1.0f, 2.0f);

		mWaves.disturb(i, j, r);
	}

	mWaves.update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	// mWavesVertexBuffer keeps track of the vertex buffer located in GPU memory.
	// However we are not allowed to directly modify it. Instead D3D11 provides 
	// the DeviceContext::Map(...) method to access a GPU mem chunk.
	// 
	// There's a race condition problem here:
	// GPU needs to read the vertex buffer to do rendering work, and at the same time we instruct CPU
	// to write data into the vertex buffer. To avoid hanging GPU read operation, D3D11 defined three MapType:
	//
	// D3D11_MAP_WRITE_DISCARD
	// GPU would allocate a new memory chunk into which CPU will write, while at the same time the old
	// buffer is still available to GPU(to read from). Then GPU will swap to the newly created buffer.
	//
	// D3D11_MAP_WRITE_NO_OVERWRITE
	// CPU will limit its write operation to the empty part of the GPU buffer, so GPU can safely read the 
	// used part of the buffer.

	// D3D11_MAP_READ
	// All CPU want is to allocate a copy of GPU buffer in system memory.

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(md3dImmediateContext->Map(mWavesVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for(UINT i = 0; i < mWaves.vertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	md3dImmediateContext->Unmap(mWavesVertexBuffer, 0);
}

void WaveDemo::drawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view  = XMLoadFloat4x4(&mView);
	XMMATRIX proj  = XMLoadFloat4x4(&mProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//
		// Draw the terrain
		//
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mTerrainVertexBuffer, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mTerrainIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&mTerrainWorld);
		XMMATRIX wvp = world * view * proj; 
		mfxWVP->SetMatrix(reinterpret_cast<float *>(&wvp));
		mTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mTerrainIndexCount, 0, 0);

		//
		// Draw the waves
		//

		// Turn on wireframe mode
		md3dImmediateContext->RSSetState(mWireframeRS);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWavesVertexBuffer, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWavesIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		world = XMLoadFloat4x4(&mWavesWorld);
		wvp = world * view * proj;
		mfxWVP->SetMatrix(reinterpret_cast<float*>(&wvp));

		mTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mWaves.triangleCount()*3, 0, 0);

		// Turn off wireframe mode
		md3dImmediateContext->RSSetState(0);
	}
	HR(mSwapChain->Present(0, 0));
}

void WaveDemo::onMouseDown( WPARAM btnState, int x, int y )
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mHwnd);
}

void WaveDemo::onMouseUp( WPARAM btnState, int x, int y )
{
	ReleaseCapture();
}

void WaveDemo::onMouseMove( WPARAM btnState, int x, int y )
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
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.01f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 200.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

float WaveDemo::getTerrainHeight( float x, float z ) const
{
	return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

void WaveDemo::buildTerrainBuffers()
{
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mTerrainIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  In addition, color the vertices based on their height so we have
	// sandy looking beaches, grassy low hills, and snow mountain peaks.
	//

	std::vector<Vertex> vertices(grid.Vertices.size());
	for(size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = getTerrainHeight(p.x, p.z);

		vertices[i].Pos   = p;

		// Color the vertex based on its height.
		if( p.y < -10.0f )
		{
			// Sandy beach color.
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if( p.y < 5.0f )
		{
			// Light yellow-green.
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if( p.y < 12.0f )
		{
			// Dark yellow-green.
			vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if( p.y < 20.0f )
		{
			// Dark brown.
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mTerrainVertexBuffer));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mTerrainIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mTerrainIndexBuffer));
}

void WaveDemo::buildWavesBuffers()
{
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mWaves.vertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVertexBuffer));


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<UINT> indices(3*mWaves.triangleCount()); // 3 indices per face

	// Iterate over each quad.
	UINT m = mWaves.rowCount();
	UINT n = mWaves.columnCount();
	int k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(DWORD j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWavesIndexBuffer));

}

void WaveDemo::buildEffect()
{
	std::ifstream fin("../fx/color.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, md3dDevice, &mEffect));

	mTech    = mEffect->GetTechniqueByName("ColorTech");
	mfxWVP = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void WaveDemo::buildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
}


