#include "Copter.h"

#if MODE_FLIPAFTERCRASH_ENABLED == ENABLED

bool ModeFlipAfterCrash::init(bool ignore_checks)
{
#if FRAME_CONFIG == HELI_FRAME
    // do not allow helis to flip after crash, this will not work
    return false;
#endif

    // do not enter the mode when already armed or when flying
    if (motors->armed()) {
        return false;
    }

    return true;
}

// runs the throw to start controller
// should be called at 100hz or more
void ModeFlipAfterCrash::run()
{


}
#endif
