#include "mbed.h"

struct PID {
    const float* Kp;
    const float* Ki;
    const float* Kd;

    const float* target;
    const float* current;

    float* stimulus;
    float stimulusMin, stimulusMax;

    void update(float t /* seconds*/ );
};