#ifndef RENDER_CORE_H
#define RENDER_CORE_H

#include "WindowBase.h"
#include "Util.h"
#include "Timer.h"

class RenderCore : public WindowBase<RenderCore>
{
public:
	RenderCore();
	virtual ~RenderCore();

	float aspectRatio() const;
	int run();

	virtual bool init();
	virtual LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual PCWSTR getClassName() const;
	virtual PCWSTR getWindowCaption() const;
	virtual void onResize();

	virtual void updateScene(float dt){}
	virtual void drawScene(){}

	virtual void onMouseDown(WPARAM btnState, int x, int y){}
	virtual void onMouseUp(WPARAM btnState, int x, int y){}
	virtual void onMouseMove(WPARAM btnState, int x, int y){}
	virtual void onMouseWheel(WPARAM btnState, int x, int y){}

protected:
	bool initRenderWindow();
	bool initDirect3D();
	void calculateFPS();

protected:
	Timer mTimer;

	/**** General config ****/
	int				mClientWidth;
	int				mClientHeight;
	bool			mEnable4xMsaa;
	UINT			m4xMsaaQuality;
	D3D_DRIVER_TYPE	md3dDriverType;
	
	/**** Window states ****/
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;

	/**** Direct 3D interfaces ****/
	ID3D11Device*				md3dDevice;
	ID3D11DeviceContext*		md3dImmediateContext;
	IDXGISwapChain*				mSwapChain;
	ID3D11Texture2D*			mDepthStencilBuffer;
	ID3D11RenderTargetView*		mRenderTargetView;
	ID3D11DepthStencilView*		mDepthStencilView;
	D3D11_VIEWPORT				mScreenViewport;

};
#endif