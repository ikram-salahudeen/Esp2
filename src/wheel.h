#include "mbed.h"
#include "QEI.h"
#include "C12832.h"

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
        speedTicker.attach(callback(this, &Wheel::speedControlISR), SPEED_CONTROL_PERIOD_MS / 1000.0f);
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
        if (useSpeedControl == false) return;

        static int previousPulses = 0;
        static float previousTime = 0;

        float newTime = timer.read();
        int newPulses = encoder.getPulses();

        previousPulses = newPulses;
        previousTime = newTime;


        float calculatedSpeed = ((newPulses - previousPulses) * WHEEL_DIAMETER * 3.14f) / (256 * (newTime - previousTime));
        float speedError = targetSpeed - calculatedSpeed;

        static float integral = 0;
        integral += speedError * (SPEED_CONTROL_PERIOD_MS / 1000.0f);

        float stimulus = SPEED_KP * speedError + SPEED_KI * integral;

        setPower(stimulus);
    }

    void setSpeed(float speed) {
        useSpeedControl = true;
        targetSpeed = speed;
    }

private:
    float targetSpeed;
    Ticker speedTicker;
    bool useSpeedControl;

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