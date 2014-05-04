#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include <boost/signals2.hpp>

class MouseEvent
{
public:
    typedef boost::signals2::signal<void(int, int)> SIGNAL_TYPE;

public:
    void fire(int x, int y);
    boost::signals2::connection hook(SIGNAL_TYPE::slot_type &handler);

private:
    SIGNAL_TYPE mSig;
};

#endif

