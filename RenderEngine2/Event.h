#ifndef EVENT_H
#define EVENT_H

#include <boost/signals2.hpp>

class MouseEvent
{
public:
    typedef boost::signals2::signal<void(int, int)> SIGNAL_TYPE;

public:
    void fire(int x, int y)
    {
        return mSig(x, y);
    }

    boost::signals2::connection hook(SIGNAL_TYPE::slot_type &handler)
    {
        return mSig.connect(handler);
    }

private:
    SIGNAL_TYPE mSig;
};

#endif

