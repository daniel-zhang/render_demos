/*
Notes
1. represent a quad defined in screen space
2. transform that quad into dynamic vertex buffer so it can be updated
3. add a shader to render quad 
*/
#include "SpriteDemo.h"
#include "EffectMgr.h"
#include "RenderStateMgr.h"
#include "Color.h"


SpriteDemo::SpriteDemo()
{
}

SpriteDemo::~SpriteDemo()
{
}

bool SpriteDemo::init()
{
    if (DemoBasic::init() == false)
    {
        return false;
    }

    FontEngine fontEngine;
    mFontSheet.mFontName = L"Arial Unicode MS";
    //mFontSheet.mFontName = L"Arial";
    mFontSheet.mFontStyle = Gdiplus::FontStyleRegular;
    mFontSheet.mFontSizeInPixel = 40;
    fontEngine.createFontSheet(md3dDevice, mFontSheet, L"debug");

    mWidgetMgr.init(&mInput, md3dDevice, md3dImmediateContext);

    Container* window1 = new Container(
        mWidgetMgr.getRenderEnv(),
        Point2D(100, 100), Area2D(200, 200), RGBAColor::Red,
        PixelPadding(), PixelMargin(), 
        WIDGET_LAYOUT_STATIC);

    Container* window2 = new Container(
        mWidgetMgr.getRenderEnv(),
        Point2D(200, 50), Area2D(150, 200), RGBAColor::Green,
        PixelPadding(), PixelMargin(), 
        WIDGET_LAYOUT_STATIC);

    Container* child1 = new Container(
        mWidgetMgr.getRenderEnv(),
        Point2D(110, 110), Area2D(150, 50), RGBAColor::Background,
        PixelPadding(), PixelMargin(), 
        WIDGET_LAYOUT_STATIC);

    /*
    mInfoBox = new Text(
        mWidgetMgr.getRenderEnv(),
        Point2D(210, 60), Area2D(250, 50), 
        24, &mFontSheet);
        */

    Button::Attributes buttonAttr;
    buttonAttr.position = Point2D(200, 50);
    buttonAttr.size = Area2D(150, 20);
    buttonAttr.label_font_sheet = &mFontSheet;
    buttonAttr.label_font_size = 20;
    buttonAttr.label_string =  L"Button 1";
    buttonAttr.info_font_sheet = &mFontSheet;
    buttonAttr.info_string = L"This is button 1's info string.\nThis is button 1's info string.";
    buttonAttr.info_frame_size = Area2D(230, 120);
    buttonAttr.info_font_size = 18;

    Button* button1 = new Button(mWidgetMgr.getRenderEnv(), buttonAttr);
    buttonAttr.position = Point2D(200, 73);
    buttonAttr.label_string =  L"Button 2";
    buttonAttr.info_string = L"This is button 2's info string.\nThis is button 2's info string.";
    Button* button2 = new Button(mWidgetMgr.getRenderEnv(), buttonAttr);

    List::Attributes listAttr;
    listAttr.font_sheet = &mFontSheet;
    listAttr.font_size = 20;
    listAttr.background_color = RGBAColor::Blue;
    listAttr.active_background_color = RGBAColor::Yellow;
    listAttr.item_desc.push_back(List::ItemDesc(L"BillboardDemo", 
        L"Focused on:\n"
        L"How to build sprites in GS\n"
        L"How to use the texture array\n"
        L"Alpha-to-coverage: specify a 2D object's silhouette by alpha map\n"
        L"so MXAA can be applied to 2d stuff" ));
    listAttr.item_desc.push_back(List::ItemDesc(L"MeshLoaderDemo", 
        L"Info..."));
    listAttr.item_desc.push_back(List::ItemDesc(L"BoxDemo", L"Info..."));
    listAttr.item_desc.push_back(List::ItemDesc(L"EmptyDemo", L"Info..."));
    listAttr.item_desc.push_back(List::ItemDesc(L"LightingDemo", L"Info..."));
    listAttr.item_desc.push_back(List::ItemDesc(L"LightingDemo2", L"Info..."));
    List* lst = new List(mWidgetMgr.getRenderEnv(), listAttr);

    mWidgetMgr.addWidget(window1);
    mWidgetMgr.addWidget(window2);
    mWidgetMgr.addWidget(child1, window1);
    // mWidgetMgr.addWidget(mInfoBox);
    mWidgetMgr.addWidget(button1, window2);
    mWidgetMgr.addWidget(button2, window2);
    mWidgetMgr.addWidget(lst);
 
    return true;
}

void SpriteDemo::drawScene()
{
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    md3dImmediateContext->IASetInputLayout(InputLayoutMgr::PosNormalTex);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set per frame constants.
    EffectMgr::BasicFX->setDirLights(mDirLights);
    EffectMgr::BasicFX->setEyePosW(mEyePosWorld);
    EffectMgr::BasicFX->setFogColor(Colors::Silver);
    EffectMgr::BasicFX->setFogStart(15.0f);
    EffectMgr::BasicFX->setFogRange(175.0f);

    //
    // Draw grid
    //
    // Update Cam viewproj to shader program
    XMFLOAT4X4 vp;
    XMStoreFloat4x4(&vp, mCam.ViewProj());
    // TODO: let model to choose tech based on the model to be rendered.
    ID3DX11EffectTechnique* tech = 0;
    md3dImmediateContext->RSSetState(RenderStateMgr::WireframeRS);
    mpModel->draw(tech, vp);
    md3dImmediateContext->RSSetState(0);

    /*
    std::wostringstream out;
    float t = mTimer.totalTime();
    out << L"Hello World!!\n" <<  mTimer.totalTime()  << L"\n12345678";
    mInfoBox->setText(out.str());
    */

    mWidgetMgr.draw();

    HR(mSwapChain->Present(0, 0));
}

void SpriteDemo::onResize()
{
    DemoBasic::onResize();
    
    UINT vpNum = 1;
    D3D11_VIEWPORT vp;
    md3dImmediateContext->RSGetViewports(&vpNum, &vp);

    mInput.EventViewportResize.fire(static_cast<int>(vp.Width), static_cast<int>(vp.Height));
}

void SpriteDemo::onMouseMove( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseMove(btnState, x, y);

//    mMouse.EventMove.fire(x, y);

    mInput.EventMove.fire(x, y);
}

void SpriteDemo::onMouseDown( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseDown(btnState, x, y);

    if( (btnState & MK_LBUTTON) != 0 )
    {

        mInput.EventLBtnDown.fire(x, y);
    }
    else if ( (btnState & MK_RBUTTON) != 0)
    {
    }
    else
    {
    }
}

void SpriteDemo::onMouseUp( WPARAM btnState, int x, int y )
{
    DemoBasic::onMouseUp(btnState, x, y);

    mInput.EventLBtnUp.fire(x, y);

    if( (btnState & MK_LBUTTON) != 0 )
    {
    }
    else if ( (btnState & MK_RBUTTON) != 0)
    {
    }
    else
    {
    }
}

void SpriteDemo::onMouseWheel( WPARAM btnState, int x, int y )
{
    int zDelta = GET_WHEEL_DELTA_WPARAM(btnState);
    if (zDelta < 0)
    {
        mInput.EventMouseWheelUp.fire(x, y);
    }
    else
    {
        mInput.EventMouseWheelDown.fire(x, y);
    }
}

