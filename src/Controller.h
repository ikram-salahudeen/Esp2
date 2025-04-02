#include "mbed.h"
#include "wheel.h"
#include "pins.h"
#include "rtos.h"
#include "bluetooth.h"




//extern const DigitalOut lineLeds[5];

struct Controller {
    /*
        Status variables
    */

    float desiredSpeed;

    /*
        Line sensor
    */
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

    float readings[5];
    float lineLocation;  // Where it thinks the line is from the centre (-2 to +2)
    bool lineDetected;   // false if no line detected

    /*
        Direction PID
    */
    PID steerPid;
    float lineLocationSetpoint; // always 0
    float direction; // output of PID
    float directionMin;
    float directionMax;

    /*
        Motors
    */

    Wheel L, R;
    DigitalOut enable;

    Controller():
        steerPid(&bt.params["DKp"], &bt.params["DKi"], &bt.params["DKd"], &lineLocationSetpoint, &lineLocation, &direction, &directionMin, &directionMin),
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
        lineLed4(LINE_LED_4),
        lineDetected(false)
        {
        bt.params["Ts"] = 0.01;
        bt.params["L"] = 0;
        bt.params["R"] = 0;
        
    }
    
    void start() {
    }

    void led_sample() {
        float sample_time = bt.params["Ts"] / 5;

        // Turn on the leds one at a time, wait a bit and read the value
        // This will be run 5 times as fast as the other functions
        // Sensor 0 sampling
        lineLed0 = 1;             // Turn on sensor 0 IR emitter
        wait_us(sample_time);             // Delay 100μs for stabilization
        readings[0] = lineSense0.read();  // Read analog value from sensor 0
        lineLed0 = 0;             // Turn off IR emitter

        // Sensor 1 sampling
        lineLed1 = 1;
        wait_us(sample_time);
        readings[1] = lineSense1.read();
        lineLed1 = 0;

        // Sensor 2 sampling
        lineLed2 = 1;
        wait_us(sample_time);
        readings[2] = lineSense2.read();
        lineLed2 = 0;

        // Sensor 3 sampling
        lineLed3 = 1;
        wait_us(sample_time);
        readings[3] = lineSense3.read();
        lineLed3 = 0;

        // Sensor 4 sampling
        lineLed4 = 1;
        wait_us(sample_time);
        readings[4] = lineSense4.read();
        lineLed4 = 0;
    }

    void process_line() {
        // Subtract minumum
        float min = *std::min_element(&readings[0], &readings[5]);

        for (int i = 0; i < 5; i++) {
            readings[i] = (readings[i] - min);
        }

        
        

        // Convert to digital
        const float threshold = 0.03;
        float numberOf1s = 0; // Number of sensors with readings > threshold.
        
        for (int i = 0; i < 5; i++) {
            if (readings[i] > threshold) {
                readings[i] = 1;
                numberOf1s++;
            } else {
                 readings[i] = 0;
            }
        }

        if (numberOf1s == 0) {
            // No line detected
            lineDetected = false;
            bt.outputs["line"] = -10;
            return;
        }

        lineDetected = true;
        // Average location of '1's
        float sum = 0;
         for (int i = 0; i < 5; i++) {
            if (readings[i] == 1) {
                sum += i - 2;
            }
        }

        float lineLocation = sum / numberOf1s;
        
        bt.outputs["line"] = lineLocation;
    }

    void follow() {
        // Stop if needed
    }

    void pid_update() {
        /*
            Direction control loop
        */

        if (lineDetected) { // Only do it if a line is dectected
            steerPid.update();
            // Now we have direction, set the speed of each motor
            // Assume direction is the angular velocity in radians
            // v_l = v - (L * ω) / 2
            // v_r = v + (L * ω) / 2

            L.setSpeed(desiredSpeed + (WHEEL_DIAMETER + direction) / 2)
            R.setSpeed(desiredSpeed - (WHEEL_DIAMETER + direction) / 2)
        };


        /*
            Speed control loop
        */

        L.calculateSpeed();            // Measure wheel velocity
        L.pid.update();                // Do PID calculations 
        L.update();                    // Update motor pwm

        R.calculateSpeed();
        R.pid.update();
        R.update();

        //bt.outputs["LSpeed"] = L.speed();
        //bt.outputs["LPower"] = L.getPower();
        //bt.outputs["LSetpoint"] = L.targetSpeed;
    }

    void loop() {
        led_sample();
        process_line();
        pid_update();

        
        // Set enable pin
        if (bt.params["En"] == 1) {
            enable = 1;
        } else {
            enable = 0;
        }

        
        //bt.outputs["-1"] = -1;
        //bt.outputs["1"] = 1;

        static unsigned n= 0;
        if (n%10 == 0) {
            bt.report();
        }
        n++;
    }

};
