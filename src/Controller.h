#include "mbed.h"
#include "pins.h"
#include "td1.h"
#include "rtos.h"


extern const AnalogIn lineSensors[5];
extern const DigitalOut lineLeds[5];

struct Controller {
    /*
        Line sensor
    */
    float readings[5];


    /*
        Motors
    */

    Wheel L, R;

    Controller():
        L(MOTOR_L_PWM, MOTOR_L_DIR, MOTOR_L_BIPOLAR, ENCODER_L_A, ENCODER_L_B, PWM_FREQUENCY),
        R(MOTOR_R_PWM, MOTOR_R_DIR, MOTOR_R_BIPOLAR, ENCODER_R_A, ENCODER_R_B, PWM_FREQUENCY)
        {

        }
    

    void init() {
    
    }

    void loop() {
        bt.serial.printf("Line: %f %f %f %f %f", readings[0], readings[1], readings[2], readings[3], readings[4], readings[5]);

        for (int i = 0; i < 5; i++) {
            if (readings[i] > bt.params["LT"]) {
                lcd.fillrect(1 + i*7, 1, 5, 5, 1);
            } else {
                lcd.rect(1 + i*7, 1, 5, 5, 1);
            }
        }
        printf("Speed %f %f", L.speed(), R.speed());
    }

};