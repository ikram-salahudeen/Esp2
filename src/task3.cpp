/*#include "td1.h"

int L_pulses_count = 0; // count for change in pulses
int R_pulses_count = 0;
int L_initial_pulses = 0;
int R_initial_pulses = 0;
const float rpm_update_time = 0.5; // how often rpm is calculated
const float wheel_diameter = 0.08; // defined for testing
const float PI = 3.141592653589793f;
const float gearbox_ratio = 16.0;
Ticker ticker;

void timerISR() { // called to calculate the change in pulses over defind period rpm_update_time
    L_pulses_count = L.encoder.getPulses() - L_initial_pulses;
    L_initial_pulses = L.encoder.getPulses();

    R_pulses_count = R.encoder.getPulses() - R_initial_pulses;
    R_initial_pulses = R.encoder.getPulses();
}

void setup_timer() {
    ticker.attach(&timerISR, rpm_update_time);  // call ISR every rpm_update_time
}

void task3() {
    setup_timer();  // initialize the repeating timer
    while (1) {
        float L_rpm = (L_pulses_count * (60.0 / rpm_update_time)) / 256.0; // rpm calculation
        float R_rpm = (R_pulses_count * (60.0 / rpm_update_time)) / 256.0;
        float L_velocity = (PI * (L_rpm * 60.0) * wheel_diameter) / gearbox_ratio;
        float R_velocity = (PI * (R_rpm * 60.0) * wheel_diameter) / gearbox_ratio;
        lcd.locate(8,4); 
        lcd.printf("L: %02d   R: %02d", L.encoder.getPulses(), R.encoder.getPulses()); // print total pulses
        // lcd.locate(8,12);
        // lcd.printf("L: %02d   R: %02d", L_pulses_count, R_pulses_count); // print total pulses counted over past time
        lcd.locate(8,12);
        lcd.printf("L vel: %02.2f   R vel: %02.2f", L_velocity, R_velocity); // print velocity
        lcd.locate(8,20);
        lcd.printf("L RPM: %02.2f   R RPM: %02.2f", L_rpm, R_rpm); // print rpm
    }
}*/