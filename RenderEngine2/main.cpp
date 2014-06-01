#include "EmptyDemo.h"
#include "BoxDemo.h"
#include "GridDemo.h"
#include "SceneDemo.h"
#include "MeshLoaderDemo.h"
#include "WaveDemo.h"

#include "LightingDemo.h"
#include "LightingDemo2.h"

#include "ObjLoaderDemo.h"
#include "BillboardDemo.h"

#include "DemoBasic.h"
#include "SpriteDemo.h"

#include "GuiTest.h"
#include "SerializationTest.h"

//
// TODO: Add a sprite based GUI system that supports in-game selection of demos 
//

int WINAPI wWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPWSTR lpCmdLine, __in int nShowCmd )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    SpriteDemo demo;
    //GUITest demo;
    //SerializationTest demo;
    //LightingDemo2 demo;
    if(!demo.init()) return 0;

    return demo.run();
}