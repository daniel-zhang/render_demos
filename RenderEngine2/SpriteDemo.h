#ifndef SPRITE_DEMO_H
#define SPRITE_DEMO_H

#include "DemoBasic.h"
#include "FontEngine.h"

//#include "OverlayGraphics.h"
#include "WidgetMgr.h"

#include "WidgetMgr2.h"
#include "GUIRenderer.h"
#include "Input.h"

class SpriteDemo : public DemoBasic
{
public:
    SpriteDemo();
    ~SpriteDemo();

    virtual bool init();
    virtual void drawScene();
    virtual void onResize();
    virtual void onMouseMove(WPARAM btnState, int x, int y);
    virtual void onMouseDown(WPARAM btnState, int x, int y);
    virtual void onMouseUp(WPARAM btnState, int x, int y);
    virtual void onMouseWheel(WPARAM btnState, int x, int y);

private:
    GUIRenderer mRenderer;
    WidgetMgr2 mWidgetMgr2;

private:
    FontSheet mFontSheet;
//    OverlayUI::OverlayGraphics mOverlayGraphics;
//   Animation2D mAnim;
//    std::vector<OverlayUI::Quad*> mQuads;
    Input mInput;
    Mouse mMouse;
    
    WidgetMgr mWidgetMgr;
};

#endif