//***************************************************************************************
// SpritesAndText.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates drawing sprites and text with d3d11.  
//
//***************************************************************************************

#include "d3dApp.h"
#include "Effects.h"
#include "FontSheet.h"
#include "SpriteBatch.h"
#include "RenderStates.h"

class SpritesAndTextApp : public D3DApp
{
public:
	SpritesAndTextApp(HINSTANCE hInstance);
	~SpritesAndTextApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

private:
	FontSheet mFont;
	SpriteBatch mSpriteBatch;

	ID3D11ShaderResourceView* mImageSRV;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	SpritesAndTextApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

SpritesAndTextApp::SpritesAndTextApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
	mMainWndCaption = L"Sprites and Text Demo";
}

SpritesAndTextApp::~SpritesAndTextApp()
{
	Effects::DestroyAll();
	RenderStates::DestroyAll();

	ReleaseCOM(mImageSRV);
}

bool SpritesAndTextApp::Init()
{
	if(!D3DApp::Init())
		return false;

	Effects::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	HR(mFont.Initialize(md3dDevice, L"Times New Roman", 96.0f, FontSheet::FontStyleItalic, true));

	HR(mSpriteBatch.Initialize(md3dDevice));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/WoodCrate01.dds", 0, 0, &mImageSRV, 0 ));
	
	return true;
}

void SpritesAndTextApp::OnResize()
{
	D3DApp::OnResize();
}

void SpritesAndTextApp::UpdateScene(float dt)
{

}

void SpritesAndTextApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	mSpriteBatch.BeginBatch(mImageSRV);

	// Draw the sprite in each corner of the screen with a different tint.
	CD3D11_RECT r1(0, 0, 200, 200);
	CD3D11_RECT r2(mClientWidth - 200, 0, mClientWidth, 200);
	CD3D11_RECT r3(0, mClientHeight - 200, 200, mClientHeight);
	CD3D11_RECT r4(mClientWidth - 200, mClientHeight - 200, mClientWidth, mClientHeight);

	mSpriteBatch.Draw(r1, XMCOLOR(0xffffffff));
	mSpriteBatch.Draw(r2, XMCOLOR(0xffff0000));
	mSpriteBatch.Draw(r3, XMCOLOR(0xff00ff00));
	mSpriteBatch.Draw(r4, XMCOLOR(0xff00ffff));

	mSpriteBatch.EndBatch(md3dImmediateContext);

	float blendFactor[4] = {1.0f};
	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	std::wstring text = L"Hello Direct3D 11!";

	// Calculate the text width.
	int textWidth = 0;
	for(UINT i = 0; i < text.size(); ++i)
	{
		WCHAR character = text[i];
		if(character == ' ') 
		{
			textWidth += mFont.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = mFont.GetCharRect(text[i]);
			textWidth += (r.right - r.left + 1);
		}
	}

	// Center the text in the screen.
	POINT textPos;
	textPos.x = (mClientWidth - textWidth) / 2;
	textPos.y = (mClientHeight - mFont.GetCharHeight()) / 2 ;

	mSpriteBatch.DrawString(md3dImmediateContext, mFont, text, textPos, XMCOLOR(0xffffffff));

	// restore default
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	HR(mSwapChain->Present(0, 0));
}
