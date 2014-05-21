#ifndef TEXT_ELEMENT_H
#define TEXT_ELEMENT_H
#include "Widget.h"

class FontSheet;
class TextElement : public Widget
{
public:
    TextElement(D3DEnv* env, wchar_t charactor, int fontSize, FontSheet* fontSheet );
    virtual bool init();

    void setChar(wchar_t charactor);
    wchar_t& getChar(){return mCharactor;}

    void setFontSize(int fontSize);
    int getFontSize(){return mFontSize;}

    void enableExternalTextureSrc();
    void disableExternalTextureSrc();

    void setColor(const RGBA& color);

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
    wchar_t mCharactor;
};
#endif