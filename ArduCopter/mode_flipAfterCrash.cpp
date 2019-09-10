#include "Copter.h"

#if MODE_FLIPAFTERCRASH_ENABLED == ENABLED


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

    return true;
}

// should be called at 100hz or more
void ModeFlipAfterCrash::run() {

    int16_t pwm = 0;   // pwm that will be output to the motors
    int8_t motor1 = 1;
    int8_t motor2 = 2;


    // choose direction based on pilot's roll and pitch sticks
    if (channel_pitch->get_control_in() > 100)
    {
        pwm = channel_pitch->get_radio_in();
        motor1 = 1;
        motor2 = 2;

    }
    else if (channel_pitch->get_control_in() < -100)
    {
        pwm = channel_pitch->get_radio_in();
        motor1 = 3;
        motor2 = 4;
    }
    else if (channel_roll->get_control_in() > 100)
    {
        pwm = channel_roll->get_radio_in();
        motor1 = 2;
        motor2 = 4;
    }
    else if (channel_roll->get_control_in() < -100)
    {
        pwm = channel_roll->get_radio_in();
        motor1 = 3;
        motor2 = 1;
    }


    // sanity check throttle values
    if (pwm >= MOTOR_TEST_PWM_MIN && pwm <= MOTOR_TEST_PWM_MAX) {
        // turn on motor to specified pwm value
        motors->output_test_seq(motor1, pwm);
        motors->output_test_seq(motor2, pwm);
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
