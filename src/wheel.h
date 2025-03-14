#include "mbed.h"
#include "bluetooth.h"
#include "QEI.h"
#include "C12832.h"
#include "td1.h"
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

extern Timer timer;

class Wheel {
    /*
    power = -1.00 (max power in reverse)
    power = 0.00 (no power)
    power = 1.00 (max power forward)
    */
    float power;
    Mode mode;
    float frequency;
    
    float targetSpeed;
    float currentSpeed;
    Ticker speedTicker;
    bool useSpeedControl;
    float Ts;
    PID pid;

public:
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
        pid(&bt.params["Kp"], &bt.params["Ki"], &bt.params["Kd"], &bt.params["Ts"], &targetSpeed, &currentSpeed,
            &power, -1, 1, callback(this, &Wheel::calculateSpeed),callback(this, &Wheel::update)), Ts(0.01)
    {
        pwm.period(1/frequency);
        
        bt.params["Ts"] = 0.01;
        bt.params["Kp"] = 0;
        bt.params["Ki"] = 0;
        bt.params["Kd"] = 0;
    };

    
    void setPower(float newPower) {
        power = newPower;
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

    void calculateSpeed() {
        int pulses_count = 0; // count for change in pulses
        int initial_pulses = 0;
        const float wheel_diameter = 0.08; // defined for testing
        const float PI = 3.141592653589793f;
        const float gearbox_ratio = 16.0;

        pulses_count = encoder.getPulses() - initial_pulses;
        initial_pulses = encoder.getPulses();

        float rpm = (pulses_count * (60.0 / bt.params["Ts"])) / 256.0; // rpm calculation
        currentSpeed = (PI * (rpm * 60.0) * wheel_diameter) / gearbox_ratio;
    }

    float speed() {
        return currentSpeed;
    }

private:

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