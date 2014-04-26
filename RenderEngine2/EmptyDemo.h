/*
Demo features
---------------------------------------------------
Minimal code that derives from RenderCore
*/
#ifndef EMPTY_DEMO_H
#define EMPTY_DEMO_H
#include "RenderCore.h"

class EmptyDemo : public RenderCore
{
public:
	EmptyDemo();
	~EmptyDemo();

	bool init();

	void drawScene();
};

#endif