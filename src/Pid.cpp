#include "pid.h"



void PID::update(float Ts) {
    // Remember previous values
    static float u_tk = 0;
    static float u_tk_1 = 0;
    static float u_tk_2 = 0;

    static float e_tk = 0;
    static float e_tk_1 = 0;
    static float e_tk_2 = 0;

    // Error
    e_tk = *target - *current;

    // Equation for PID control in discrete time
    u_tk = u_tk_1 + e_tk * (*Kp + *Ki * Ts + *Kd / Ts) + e_tk_1 * (-*Kp - 2*(*Kd)/Ts) + (*Kd) / Ts + (*Kd) * e_tk_2 / Ts;
    

    // Clamp to min or max values
    if (u_tk > stimulusMax) u_tk = stimulusMax;
    if (u_tk < stimulusMin) u_tk = stimulusMin;

    *stimulus = u_tk;

    // Update for the next call
    e_tk_2 = e_tk_1;
    e_tk_1 = e_tk;

    u_tk_2 = u_tk_1;
    u_tk_1 = u_tk;
}