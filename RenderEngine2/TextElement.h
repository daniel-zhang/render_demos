#ifndef TEXT_ELEMENT_H
#define TEXT_ELEMENT_H

#include "Widget.h"
#include "FontEngine.h"

class TextElement : public Widget
{
public:
    
    TextElement(Widget* parent, int fontSize, wchar_t charactor, FontSheet* fontSheet );

    wchar_t& getChar(){return mCharacter;}
    void setColor(RGBA& color);

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
    wchar_t mCharacter;
    RGBA mFontColor;
};
#endif