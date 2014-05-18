#ifndef TEXT_H
#define TEXT_H

#include "Widget.h"
#include "TextElement.h"
#include <string>

class Text : public Widget
{
public:
    Text( Widget* parent, int fontSize,  FontSheet* fontSheet, Area2D& size, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType);
    Text( Widget* parent, int fontSize,  FontSheet* fontSheet, Point2D& pos, Area2D& size, PixelPadding& padding, PixelMargin& margin, LayoutType layoutType = WIDGET_LAYOUT_STATIC);

    void setText(std::wstring& s);
    void clearText();
    void setFontSize(int newSize);
    void setFontColor(RGBA& color);
    void setBgColor(RGBA& color);

    virtual void updateRenderable();

    //virtual void onLayoutChanged(GUIEvent& evt);
    virtual void onResize(GUIEvent& e);
    virtual void onLBtnDown(GUIEvent& e);
    virtual void onLBtnUp(GUIEvent& e);
    virtual void onMouseMove(GUIEvent& e);
    virtual void onMouseWheelUp(GUIEvent& e);
    virtual void onMouseWheelDown(GUIEvent& e);
    virtual void onMouseEnter(GUIEvent& e);
    virtual void onMouseLeave(GUIEvent& e);

    virtual void resize(Area2D& newSize);
    virtual void solveLayout();

protected:
    int mFontSize;
    FontSheet* mFontSheet;
    std::wstring mString;
    RGBA mBgColor;
    RGBA mFontColor;
    RGBA mActiveBgColor;
    RGBA mActiveFontColor;
};

#endif