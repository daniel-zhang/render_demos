#ifndef GUI_TEST_H
#define GUI_TEST_H

#include "DemoBasic.h"
#include <string>
#include "SpriteRenderer.h"
#include "Sprite2D.h"

class GUITest: public DemoBasic
{
public:
    GUITest();
    ~GUITest();

    virtual bool init();
    virtual void drawScene();
    virtual void onResize();
    virtual void onMouseMove(WPARAM btnState, int x, int y);
    virtual void onMouseDown(WPARAM btnState, int x, int y);
    virtual void onMouseUp(WPARAM btnState, int x, int y);
    virtual void onMouseWheel(WPARAM btnState, int x, int y);

    SpriteRenderer mRenderer;
    std::vector<Sprite2D*> mSprites;
    Box2D mClipBox;
    Point2D mLasMousePos;
    bool mIsBtnDown;
};
#endif
