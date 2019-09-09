#include "Copter.h"

#if MODE_FLIPAFTERCRASH_ENABLED == ENABLED

#define FLIP_ROLL_RIGHT      1      // used to set flip_dir
#define FLIP_ROLL_LEFT      -1      // used to set flip_dir

#define FLIP_PITCH_BACK      1      // used to set flip_dir
#define FLIP_PITCH_FORWARD  -1      // used to set flip_dir
#define MOTOR_TEST_PWM_MIN              800     // min pwm value accepted by the test
#define MOTOR_TEST_PWM_MAX              2200    // max pwm value accepted by the test


bool ModeFlipAfterCrash::init(bool ignore_checks) {
#if FRAME_CONFIG == HELI_FRAME
    // do not allow helis to flip after crash, this will not work
    return false;
#endif

    // do not enter the mode when already armed or when flying
    if (motors->armed()) {
        return false;
    }

    if (!motors->armed()) {
        // init_rc_out();
        // enable_motor_output();
        motors->armed(true);
    }

    // disable throttle and gps failsafe
    g.failsafe_throttle = FS_THR_DISABLED;
    g.failsafe_gcs = FS_GCS_DISABLED;
    g.fs_ekf_action = 0;

    gcs().send_text(MAV_SEVERITY_INFO, "starting flip after crash mode");

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

// should be called at 100hz or more
void ModeFlipAfterCrash::run() {

    int16_t pwm = 0;   // pwm that will be output to the motors

    pwm = channel_throttle->get_radio_in();

    // sanity check throttle values
    if (pwm >= MOTOR_TEST_PWM_MIN && pwm <= MOTOR_TEST_PWM_MAX) {
        // turn on motor to specified pwm value
        motors->output_test_seq(1, -pwm);
    } else {
        motors->output_min();
    }
}

void ModeFlipAfterCrash::stop() {

    gcs().send_text(MAV_SEVERITY_INFO, "finished flip after crash mode");

    // disarm motors
    motors->armed(false);

    // re-enable failsafes
    g.failsafe_throttle.load();
    g.failsafe_gcs.load();
    g.fs_ekf_action.load();


    // turn off notify leds
    AP_Notify::flags.esc_calibration = false;

}

#endif
