#include "Copter.h"
#include <AP_Math/AP_Math.h>

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
    int8_t motor3 = 1;
    int8_t motor4 = 2;

    // choose direction based on pilot's roll and pitch sticks
    if (channel_pitch->get_control_in() > 100) {
        pwm = channel_pitch->get_radio_in();
        motor1 = 1;
        motor2 = 2;
        motor3 = 3;
        motor4 = 4;

    } else if (channel_pitch->get_control_in() < -100) {
        pwm = channel_pitch->get_radio_in();
        motor1 = 3;
        motor2 = 4;
        motor3 = 1;
        motor4 = 2;
    } else if (channel_roll->get_control_in() > 100) {
        pwm = channel_roll->get_radio_in();
        motor1 = 2;
        motor2 = 4;
        motor3 = 1;
        motor4 = 3;
    } else if (channel_roll->get_control_in() < -100) {
        pwm = channel_roll->get_radio_in();
        motor1 = 3;
        motor2 = 1;
        motor3 = 2;
        motor4 = 4;
    }

    pwm = constrain_int16(pwm, 1000, 2000);
    uint16_t value = 2 * (pwm - 1000);

    // this is a DShot-3D output, map so that 1500 PWM is zero throttle reversed
    if (value < 1000) {
        value = 2000 - value;
    } else if (value > 1000) {
        value = value - 1000;
    } else {
        // mid-throttle is off
        value = 0;
    }

    if (value != 0) {
        // dshot values are from 48 to 2047. Zero means off.
        value += 47;
    }

    if (pwm >= MOTOR_TEST_PWM_MIN && pwm <= MOTOR_TEST_PWM_MAX) {

        // turn on motor to specified pwm value
        motors->output_test_seq(motor1, value);
        motors->output_test_seq(motor2, value);
        motors->output_test_seq(motor3, 0);
        motors->output_test_seq(motor4, 0);
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
