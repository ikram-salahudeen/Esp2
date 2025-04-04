#include "mbed.h"
#include "wheel.h"
#include "pins.h"
#include "rtos.h"
#include "bluetooth.h"
#include <cmath>

enum State {
    Go = 0, // Normal steering
    Stop = 1, // Brake
    UturnCW = 2, // Turn clockwise on the spot until the line is not detected, then go to 'findLineCW'
    UturnCCW = 3,
    findLineCW = 4, // Turn clockwise on the spot until it's on the line, then go to 'nextState'
    findLineCCW = 5
};

struct Controller {
    /*
        Status variables
    */
    State state;
    State nextState;
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
        steerPid(&bt.params["DKp"], &bt.params["DKi"], &bt.params["DKd"], &lineLocationSetpoint, &lineLocation, &direction, directionMin, directionMin),
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
        lineLocation(0),
        lineDetected(false),
        state(Go),
        nextState(Stop),
        desiredSpeed(0),
        directionMax(1),
        directionMin(-1)
        
        {
        bt.params["Ts"] = 0.01;
        bt.params["L"] = 0;
        bt.params["R"] = 0;
        L.setMode(Bipolar);
        R.setMode(Bipolar);
    }
    
    void start() {
    }

    void led_sample() {
        float sample_time = 1000000.0f * bt.params["Ts"] / 5.0f;

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

        if (bt.params["log"] == 0) {
            bt.outputs["Sense0"] = readings[0];
            bt.outputs["Sense1"] = readings[1];
            bt.outputs["Sense2"] = readings[2];
            bt.outputs["Sense3"] = readings[3];
            bt.outputs["Sense4"] = readings[4];
        }
        
        float weightedAverage = 0;
        for (int i = 0; i < 5; i++) { 
            weightedAverage += (i-2) * readings[i];
        }
        weightedAverage /= 5;
        bt.outputs["WA"] = weightedAverage;

        // Convert to digital
        const float threshold = 0.01;
        float numberOf1s = 0; // Number of sensors with readings > threshold.
        
        for (int i = 0; i < 5; i++) {
            if (readings[i] > threshold) {
                readings[i] = 1;
                numberOf1s++;
            } else {
                 readings[i] = 0;
            }
        }

        if (bt.params["log"] == 1) {
            bt.outputs["DSense0"] = readings[0];
            bt.outputs["DSense1"] = readings[1];
            bt.outputs["DSense2"] = readings[2];
            bt.outputs["DSense3"] = readings[3];
            bt.outputs["DSense4"] = readings[4];
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

        // stop the buggy if no line detected
        // 1. get sensor values
        // 2. IF ALL sensors < threshold value, start timer
        // 3. exit & get sensor values again
        // 4. IF ALL sensors < threshold value, do nothing, ELSE reset timer
        // 5. repeat 3 & 4, if timer reaches 2 seconds, interrupt -> stop motors and control loop
        
       static float startTime = 0;
       const float timeout = 0.5;

        if (lineDetected) {
            startTime = timer.read();
        }
        float elapsedTime = timer.read() - startTime;

        if (elapsedTime >= timeout) {
            state = Stop;
        }
    }

    

    void pid_update() {
        /*
            Direction control loop
        */
        if (lineDetected) { // Update direction if a line is dectected
            steerPid.update();
            bt.outputs["direction"] = direction;

            // Assume direction is the angular velocity in radians
            // v_l = v - (L * ω) / 2
            // v_r = v + (L * ω) / 2

            L.setSpeed(desiredSpeed + (WHEEL_DIAMETER + direction) / 2);
            R.setSpeed(desiredSpeed - (WHEEL_DIAMETER + direction) / 2);
        };

        L.setSpeed(bt.params["L"]);
        R.setSpeed(bt.params["L"]);

        /*
            Speed control loop
        */

        L.calculateSpeed();            // Measure wheel velocity
        L.pid.update();                // Do PID calculations 
        L.update();                    // Update motor pwm

        R.calculateSpeed();
        R.pid.update();
        R.update();

        if (bt.params["log"] == 3) {
            bt.outputs["LSpeed"] = L.speed();
            bt.outputs["RSpeed"] = R.speed();
            bt.outputs["LPower"] = L.getPower();
            bt.outputs["RPower"] = R.speed();
            bt.outputs["Renc"] = R.encoder.getPulses();
            bt.outputs["LSetpoint"] = L.targetSpeed;
        }
    }

    void loop() {
        switch (state) {
        case (Stop): {
            L.setPower(0);
            R.setPower(0);
        }
           
        break;
        case (Go): {
            led_sample();
            process_line();
            pid_update();
        }
        break;

        case(UturnCW): { // Rotate CW on the spot until the line is not detected, then go to findLineCW
            led_sample();
            process_line();

            if (lineDetected == false) state = findLineCW;

            L.setSpeed(0.5);
            R.setSpeed(-0.5);

            lineDetected = false; // Stop pid_update from changing wheel speed
            pid_update();
        }
        break;
        case(UturnCCW): { // Rotate CCW on the spot until the line is not detected, then go to findLineCCw 
            led_sample();
            process_line();

            if (lineDetected == false) state = findLineCCW;

            L.setSpeed(-0.5);
            R.setSpeed(0.5);

            lineDetected = false; // Stop pid_update from changing wheel speed
            pid_update();
        }
        break;
        case(findLineCW): { // Rotate CW on the spot until its on the line, then go to nextState
            led_sample();
            process_line();

            if (lineDetected == true && lineLocation <= 0.5 && lineLocation >= -0.5) state = nextState;

            L.setSpeed(-0.5);
            R.setSpeed(0.5);

            lineDetected = false; // Stop pid_update from changing wheel speed
            pid_update();
        }
        break;

        case(findLineCCW): { // Rotate on the spot until its on the line, then go
            led_sample();
            process_line();

            if (lineDetected == true && lineLocation > -0.5) state = nextState;

            L.setSpeed(-0.5);
            R.setSpeed(0.5);

            lineDetected = false; // Stop pid_update from changing wheel speed
            pid_update();
        }
        break;

        }
        
        //Update the states from bluetooth
        int btState = (int) bt.params["s"];
        if (btState >= 0 && btState <= 5) state = static_cast<State>(btState);

        btState = (int) bt.params["n"];
        if (btState >= 0 && btState <= 5) nextState = static_cast<State>(btState);
        
        // Set enable pin
        if (bt.params["En"] == 1) {
            enable = 1;
        } else {
            enable = 0;
        }

        static int n = 0;
        if (n%10 == 0) {

        bt.report();
        }
        n++;
    }

};
