#ifndef SPRITE_DEMO_H
#define SPRITE_DEMO_H

#include "DemoBasic.h"
#include "FontEngine.h"
#include "WidgetMgr.h"
#include "Input.h"
#include <string>
#include "AllWidgets.h"

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
    ID3D11ShaderResourceView* mSRV;
    WidgetMgr mWidgetMgr;

    FontSheet mFontSheet;
    Text* mInfoBox;
//    OverlayUI::OverlayGraphics mOverlayGraphics;
//   Animation2D mAnim;
//    std::vector<OverlayUI::Quad*> mQuads;
    Input mInput;
};

#endif