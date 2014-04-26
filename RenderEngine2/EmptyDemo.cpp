#include "EmptyDemo.h"

bool EmptyDemo::init()
{
	return RenderCore::init();
}

void EmptyDemo::drawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	HR(mSwapChain->Present(0, 0));
}

EmptyDemo::EmptyDemo()
{

}

EmptyDemo::~EmptyDemo()
{

}
