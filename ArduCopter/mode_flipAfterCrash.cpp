#include "Copter.h"

#if MODE_FLIPAFTERCRASH_ENABLED == ENABLED

#define FLIP_ROLL_RIGHT      1      // used to set flip_dir
#define FLIP_ROLL_LEFT      -1      // used to set flip_dir

#define FLIP_PITCH_BACK      1      // used to set flip_dir
#define FLIP_PITCH_FORWARD  -1      // used to set flip_dir

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
    roll_dir = pitch_dir = 0;

       // choose direction based on pilot's roll and pitch sticks
       if (channel_pitch->get_control_in() > 300) {
           pitch_dir = FLIP_PITCH_BACK;
       } else if (channel_pitch->get_control_in() < -300) {
           pitch_dir = FLIP_PITCH_FORWARD;
       } else if (channel_roll->get_control_in() >= 0) {
           roll_dir = FLIP_ROLL_RIGHT;
       } else {
           roll_dir = FLIP_ROLL_LEFT;
       }
    return true;
}

// runs the throw to start controller
// should be called at 100hz or more
void ModeFlipAfterCrash::run()
{


}
#endif
