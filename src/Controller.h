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
    AnalogIn lineSense4;

    DigitalOut lineLed0;
    DigitalOut lineLed1;
    DigitalOut lineLed2;
    DigitalOut lineLed3;
    DigitalOut lineLed4;

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
        lineSense0(LINE_SENSE_0),
        lineSense1(LINE_SENSE_1),
        lineSense2(LINE_SENSE_2),
        lineSense3(LINE_SENSE_3),
        lineSense4(LINE_SENSE_4),
        lineLed0(LINE_LED_0),
        lineLed1(LINE_LED_1),
        lineLed2(LINE_LED_2),
        lineLed3(LINE_LED_3),
        lineLed4(LINE_LED_4)
        {
        bt.params["Ts"] = 0.01;
        bt.params["L"] = 0;
        bt.params["R"] = 0;
        
    }
    
    void start() {
    }

    void led_sample() {
        // Turn on the leds one at a time, wait a bit and read the value
        // This will be run 5 times as fast as the other functions
    }

    void process_line() {
        // Weighted average, immunity to ambient light
    }

    void follow() {
        // Stop if needed
    }

    void pid_update() {
        // Do the calculations for the two control loops:
        // 1) Adjust the direction to try to minimise distance to line
        // 2) Adjust motor power to regulate spped
    }



    void line() {
        // Read sensor values (normalized: 0.0 - 1.0)
        float sensor1_reading = lineSense0.read();
        float sensor2_reading = lineSense1.read();
        float sensor3_reading = lineSense2.read();
        float sensor4_reading = lineSense3.read();
        float sensor5_reading = lineSense4.read();

        // Apply thresholding: Set values below 0.5 to 0
        if (sensor1_reading < 0.1) sensor1_reading = 0;
        if (sensor2_reading < 0.1) sensor2_reading = 0;
        if (sensor3_reading < 0.1) sensor3_reading = 0;
        if (sensor4_reading < 0.1) sensor4_reading = 0;
        if (sensor5_reading < 0.1) sensor5_reading = 0;

        // Compute weighted average
        float sum = sensor1_reading + sensor2_reading + sensor3_reading + sensor4_reading + sensor5_reading;
        float weighted_average = 0;

        if (sum != 0) {
            weighted_average = (-2 * sensor1_reading + -1 * sensor2_reading + 0 * sensor3_reading + 
                                1 * sensor4_reading + 2 * sensor5_reading) / sum;
        }

        if (bt.params["Log"] == 1) {
            bt.outputs["Sensor 1"] = sensor1_reading;
            bt.outputs["Sensor 2"] = sensor2_reading;
            bt.outputs["Sensor 3"] = sensor3_reading;
            bt.outputs["Sensor 4"] = sensor4_reading;
            bt.outputs["Sensor 5"] = sensor5_reading;

            bt.outputs["WA"] = weighted_average;
        }
    }

    void loop() {

        //line();
        //bt.serial.printf("Line: %f %f %f %f %f", readings[0], readings[1], readings[2], readings[3], readings[4]);

       

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

        static unsigned n= 0;
        if (n%10 == 0) {
            bt.report();
        }
        n++;
    }

};
