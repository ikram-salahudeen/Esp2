#include "mbed.h"
#include "bluetooth.h"

class PID {
    Ticker ticker;
public:
    const float* Kp;
    const float* Ki;
    const float* Kd;
    
    const float* Ts_ptr;

    const float* target;
    const float* current;

    float* stimulus;
    float stimulusMin, stimulusMax;

    PID(const float* Kp, const float* Ki, const float* Kd, const float* Ts, const float* target, const float* current,
        float* stimulus, float stimulusMin, float stimulusMax):
        Kp(Kp), Ki(Ki), Kd(Kd), Ts_ptr(Ts), target(target), current(current),
        stimulus(stimulus), stimulusMin(stimulusMin), stimulusMax(stimulusMax)
        {}

    void start() {
        ticker.detach();
        //ticker.attach(callback(this, &PID::update), Ts);
    }

    void stop() {
        ticker.detach();
    }

    void update() {
        int time_us = 0;
        static int initial_time_us = 0;

        time_us = timer.read_us() - initial_time_us;
        initial_time_us = timer.read_us();

        float Ts = time_us / 1000000.0f; 

        // Remember previous values
        static float u_tk = 0;
        static float u_tk_1 = 0;
        static float u_tk_2 = 0;

        static float e_tk = 0;
        static float e_tk_1 = 0;
        static float e_tk_2 = 0;

        // Error
        e_tk = *current - *target;

        // Equation for PID control in discrete time
        u_tk = u_tk_1 + e_tk * ((*Kp) + (*Ki) * (Ts) + (*Kd) / (Ts)) + e_tk_1 * (-(*Kp) - 2*(*Kd) / (Ts)) + (*Kd) * e_tk_2 / (Ts);
        
        

        // Clamp to min or max values
        if (u_tk > stimulusMax) u_tk = stimulusMax;
        if (u_tk < stimulusMin) u_tk = stimulusMin;

        *stimulus = u_tk;

        // Update for the next call
        e_tk_2 = e_tk_1;
        e_tk_1 = e_tk;

        u_tk_2 = u_tk_1;
        u_tk_1 = u_tk;

        // If Ts has changed re-attach the ISR with new Ts
        //if (*Ts_ptr != Ts) start();
    }
};