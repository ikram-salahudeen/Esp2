#include "mbed.h"
#include "bluetooth.h"
#include "QEI.h"
#include "C12832.h"
#include "pid.h"

#define PWM_FREQUENCY 30000
#define WHEEL_DIAMETER 75

#define SPEED_KP 1
#define SPEED_KI 0
#define SPEED_CONTROL_PERIOD_MS 200

enum Mode {
    Bipolar,
    Unipolar
};



class Wheel {
    /*
    power = -1.00 (max power in reverse)
    power = 0.00 (no power)
    power = 1.00 (max power forward)
    */
public:
    float power;
    Mode mode;
    float frequency;
    
    float targetSpeed;
    float currentSpeed;
    Ticker speedTicker;
    bool useSpeedControl;
    PID pid;

    PwmOut pwm;
    DigitalOut direction;
    DigitalOut bipolar;
    

     QEI encoder;


    Wheel (
        PinName pwmPin, PinName directionPin, PinName bipolarPin,
        PinName encoder1, PinName encoder2,
        float frequency = 10000):
        pwm(pwmPin), direction(directionPin), bipolar(bipolarPin), frequency(frequency),
        encoder(encoder1, encoder2, NC, 256, QEI::X4_ENCODING), power(0), useSpeedControl(false),
        pid(&bt.params["Kp"], &bt.params["Ki"], &bt.params["Kd"], &targetSpeed, &currentSpeed,
            &power, -1, 1)
    {
        pwm.period(1/frequency);
        
        bt.params["Ts"] = 0.01;
        bt.params["Kp"] = 0;
        bt.params["Ki"] = 0;
        bt.params["Kd"] = 0;

        
    };

    
    void setPower(float newPower) {
        power = newPower;
        useSpeedControl = false;
        pid.stop();
        update();
    }

    float getPower() { return power; };

    void setMode(Mode newMode){
        mode = newMode;
        update();
    }
    Mode getMode() { return mode;};

    void setFrequency(float newFreq) {
        if (newFreq != frequency) {
            frequency = newFreq;
            pwm.period(1/frequency);
        }
    }
    float getFreqeuncy() { return frequency; };

    int initial_pulses;
    int initial_time_us;
    void calculateSpeed() {
        const float wheel_diameter = 0.08; // defined for testing
        const float PI = 3.141592653589793f;
        const float gearbox_ratio = 16.0;

        int pulses_count = 0; // count for change in pulses

        pulses_count = encoder.getPulses() - initial_pulses;
        initial_pulses = encoder.getPulses();

        int time_us = 0;

        time_us = timer.read_us() - initial_time_us;
        initial_time_us = timer.read_us();


        currentSpeed = (pulses_count / 1024.0f) * PI * wheel_diameter * (1000000.0f / time_us);//(PI * (rpm * 60.0) * wheel_diameter) / gearbox_ratio;
    }

    float speed() {
        return currentSpeed;
    }

    void setSpeed(float speed) {
        targetSpeed = speed;
        if (useSpeedControl == false) {
            //pid.start();
        }
    }



    void afterPid() {
        
    }

    void update() {
        if (mode == Bipolar) {
            bipolar.write(1);
            pwm.write(0.5*power + 0.5);
        } else {
            bipolar.write(0);
            direction.write(power > 0 ? 1 : 0);
            pwm.write(abs(power));
        }

    }
};