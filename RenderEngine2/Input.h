#ifndef INPUT_H
#define INPUT_H

class Mouse
{
public:
    Mouse():mPosX(0), mPosY(0){}
    void move(int x, int y);
    int mPosX, mPosY;
};

#endif