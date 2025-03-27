#include "mbed.h"
#include "wheel.h"
#include "pins.h"
#include "rtos.h"
#include "bluetooth.h"




//extern const DigitalOut lineLeds[5];

struct Controller {
    AnalogIn lineSense0;
    AnalogIn lineSense1;
    AnalogIn lineSense2;
    AnalogIn lineSense3;
    /*
        Line sensor
    */
    float readings[5];


    /*
        Motors
    */

    Wheel L, R;
    DigitalOut enable;

    Controller():
        L(MOTOR_L_PWM, MOTOR_L_DIR, MOTOR_L_BIPOLAR, ENCODER_L_A, ENCODER_L_B, PWM_FREQUENCY),
        R(MOTOR_R_PWM, MOTOR_R_DIR, MOTOR_R_BIPOLAR, ENCODER_R_A, ENCODER_R_B, PWM_FREQUENCY),
        enable(ENABLE), 
        lineSense0(A0),
        lineSense1(A1),
        lineSense2(A2),
        lineSense3(PC_0)
        {
            bt.params["Ts"];
        }
    

    void init() {
        bt.params["L"] = 0;
        bt.params["R"] = 0;
        
    }

    void loop() {
        //bt.serial.printf("Line: %f %f %f %f %f", readings[0], readings[1], readings[2], readings[3], readings[4]);

       bt.outputs["Sensor 1"] = lineSense0.read();
       bt.outputs["Sensor 2"] = lineSense1.read();
       bt.outputs["Sensor 3"] = lineSense2.read();
       bt.outputs["Sensor 4"] = lineSense3.read();

        R.setPower(0);

        if (bt.params["En"] == 1) {
            enable = 1;
        } else {
            enable = 0;
        }

        L.setSpeed(bt.params["L"]);

        
        L.calculateSpeed();
        L.pid.update();
        L.update();

        bt.outputs["Speed"] = L.speed();
        bt.outputs["Power"] = L.getPower();
        bt.outputs["Setpoint"] = L.targetSpeed;
        
        bt.outputs["-1"] = -1;
        bt.outputs["1"] = 1;

        bt.report();
    }

};
