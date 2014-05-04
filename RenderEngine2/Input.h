#ifndef INPUT_H
#define INPUT_H

class Mouse
{
public:
    Mouse():mPosX(0), mPosY(0){}
    void move(int x, int y);
    int mPosX, mPosY;
};

class InputMgr
{
public:
    void injectMouseLBtnDown(int x, int y);
    void injectMouseLBtnUp(int x, int y);
    void injectMouseMove(int x, int y);

    void hookHandler(MouseKeyEvent evt );
private:
    int mPrevMousePosX;
    int mPrevMousePosY;
    MouseKeyEvent mMouseEvent;

};
#endif