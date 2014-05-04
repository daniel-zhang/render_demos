#ifndef SPRITE_DEMO_H
#define SPRITE_DEMO_H

#include "DemoBasic.h"
#include "FontEngine.h"

//#include "OverlayGraphics.h"
#include "WidgetMgr.h"

class SpriteDemo : public DemoBasic
{
public:
    SpriteDemo();
    ~SpriteDemo();

    virtual bool init();
    virtual void drawScene();
    virtual void onResize();
    virtual void onMouseMove(WPARAM btnState, int x, int y);

private:
    FontEngine mFont;
    FontSheet mFontSheet;
//    OverlayUI::OverlayGraphics mOverlayGraphics;
//   Animation2D mAnim;
//    std::vector<OverlayUI::Quad*> mQuads;

    Mouse mMouse;
    WidgetMgr mWidgetMgr;
};

#endif