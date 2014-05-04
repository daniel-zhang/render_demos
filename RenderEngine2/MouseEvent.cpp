#include "MouseEvent.h"
void MouseEvent::fire( int x, int y )
{
    return mSig(x, y);
}

boost::signals2::connection MouseEvent::hook( SIGNAL_TYPE::slot_type &handler )
{
    return mSig.connect(handler);
}
