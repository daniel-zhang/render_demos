#ifndef WIDGET_BASE2_H
#define WIDGET_BASE2_H

#include <string>
#include "Util.h"
#include "InputLayoutMgr.h"

const float gDefaultWidgetDepth = 0.002f;
const float gDefaultWidgetDepth2 = 0.001f;

struct RectFloat
{
    RectFloat()
    {
        ZeroMemory(this, sizeof(RectFloat));
    }
    RectFloat(float l, float t, float r, float b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    float left, top, right, bottom;
};

struct Viewport
{
    Viewport() {ZeroMemory(this, sizeof(Viewport));}
    Viewport(int w, int h):width(w), height(h){}
    Viewport(float w, float h):width(static_cast<int>(w)), height(static_cast<int>(h)){}

    int width;
    int height;
};

class WidgetBase
{
public:
    // Pos refers to the upper-left vertex in NDC space.
    // Size is given in screen pixel, then turned into NDC space internally.
    WidgetBase(float posX, float posY, int width, int height, Viewport& vp);
    virtual ~WidgetBase();
    virtual void setLabel(std::wstring& labelText);

    //
    // Action
    //
    virtual void move(int x, int y, int lastX, int lastY);

    //
    // UI logic
    //
    virtual void onMouseEnter(int x, int y);
    virtual void onMouseLeave(int x, int y);
    virtual void onMouseMove(int x, int y, int lastX, int lastY);
    virtual void onLBtnDown(int x, int y);
    virtual void onLBtnUp(int x, int y);
    virtual void onRBtnDown(int x, int y);
    virtual void onRBtnUp(int x, int y);
    virtual void onResize(Viewport& vp);
    virtual void onWheelUp(int x, int y);
    virtual void onWheelDown(int x, int y);

    virtual bool hasFocus();

    //
    // UI graphics
    //
    virtual bool isPointInside(int x, int y);
    virtual Vertex::OverlayVertex* getVerticesInNDC();
    virtual void setColor(XMFLOAT4 color);
    virtual void setTextures();

    // Text
    void setText(std::wstring& text);
    const std::wstring& getText();
    void getTextArea(RectFloat& rect);
    float getFontSize();
    const Viewport& getViewPort();

protected:
    enum WidgetState
    {
        NORMAL = 0,
        ON_HOVER,
        PRESSED_DOWN,
        DISABLED,
        STATE_NUMBER
    };

    bool mHasFocus;
    //
    // UI logic
    //
    WidgetState mCurrState;
    WidgetState mPrevState;

    WidgetBase* mParent;
    std::vector<WidgetBase*> mChildren;

    //
    // UI graphics
    //
    void widgetResizeInNDC();
    XMFLOAT2 screenToNDC(int x, int y);
    void updateVertices();

    Viewport mViewport;
    int mWidth, mHeight;
    int mPaddingX, mPaddingY;
    float mFontSizePixel;
    
    XMFLOAT2 mSizeNDC;
    XMFLOAT2 mPosNDC;
    XMFLOAT4 mDefaultColor;

    Vertex::OverlayVertex mVertices[4];
    std::wstring mText;

};
#endif
