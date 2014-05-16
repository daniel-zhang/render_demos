#ifndef WIDGET_MGR_DEP_H
#define WIDGET_MGR_DEP_H

#include <vector>
#include "WidgetBase.h"
#include "Util.h"
#include "Mouse.h"

#include "InputLayoutMgr.h"
#include "FontEngine.h"

class WidgetMgr_deprecated
{
public:
    WidgetMgr_deprecated();
    ~WidgetMgr_deprecated();
    /*
    Pseudo:
    1. construct dynamic VB and IB
    2. hook event handlers into MouseEvents
    3. init font
    */
    bool init(ID3D11Device* device, ID3D11DeviceContext* context, Mouse* pMouse);

    // Set another mouse... does it make any sense?
    void setMouse(Mouse* pMouse);

    void drawUI();

    void batchBegin();
    void batchEnd();
    WidgetBase* createWidget( float posX, float poxY, int width, int height, std::wstring& labelText, std::wstring widgetType = L"Widget/Button");

    //
    // Input handlers
    //
    void onMouseMove(int x, int y);
    void onMouseLBtnDown(int x, int y);
    void onMouseLBtnUp(int x, int y);
    void onMouseRBtnDown(int x, int y);
    void onMouseRBtnUp(int x, int y);
    void onMouseWheelUp(int x, int y);
    void onMouseWheelDown(int x, int y);
    void onViewportResize(int width, int height);

    //
    // UI logic
    //
protected:
    Mouse* mpMouse;
    std::vector<WidgetBase*> mWidgets;

    //
    // Graphics Details
    //
protected:
    void createGraphicsBuffers();
    void clearGraphicsBuffers();
    // TODO: reconstruct the widget's vertices according to its pos and size
    void updateGraphicsBuffers();

    void createTextBuffers();
    void clearTextBuffers();
    void updateTextBuffers();

    void drawText();

    FontSheet mFontSheet;
    FontEngine mFontEngine;
    ID3D11ShaderResourceView* mFontAtlas;

    ID3D11Device* mDevice;
    ID3D11DeviceContext* mCtx;

    ID3D11Buffer* mGraphicsVB;
    ID3D11Buffer* mGraphicsIB;

    UINT mTextQuadNum;
    ID3D11Buffer* mTextVB;
    ID3D11Buffer* mTextIB;
};



#endif