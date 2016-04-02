#include "DumbDll.h"

namespace dumb
{
    LonelyDLL::LonelyDLL()
    {
        mInnerPeaceful = true;
    }

    LonelyDLL::~LonelyDLL()
    {

    }

    std::string LonelyDLL::whoAmI()
    {
        return mInnerPeaceful ? 
            "A lonely traveler": 
            "Someone who is very angry with almost everything";
    }

}
