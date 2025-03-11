#include "mbed.h"
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
        encoder(encoder1, encoder2, NC, 256, QEI::X4_ENCODING), power(0), useSpeedControl(false) {
        pwm.period(1/frequency);
        speedTicker.attach(callback(this, &Wheel::speedControlISR), Ts);

        pid.Kp = &bt.params["Kp"];
        pid.Ki = &bt.params["Ki"];
        pid.Kd = &bt.params["Kd"];

        pid.current = &
        //pwm.suspend();
    };

    
    void setPower(float newPower) {
        power = newPower;
        useSpeedControl = false;
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
    float getFrequncy() { return frequency; };

    void speedControlISR() {
       pid.update(Ts)
    }

    void setSpeed(float speed) {
        useSpeedControl = true;
        targetSpeed = speed;
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