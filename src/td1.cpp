#include "td1.h"
#include "pins.h"


Wheel L(MOTOR_L_PWM, MOTOR_L_DIR, MOTOR_L_BIPOLAR, ENCODER_L_A, ENCODER_L_B, PWM_FREQUENCY);
Wheel R(MOTOR_R_PWM, MOTOR_R_DIR, MOTOR_R_BIPOLAR, ENCODER_R_A, ENCODER_R_B, PWM_FREQUENCY);

C12832 lcd(D11, D13, D12, D7, D10);
//Bluetooth bt(PA_11, PA_12, 9600)
Bluetooth bt(PA_11, PA_12, 9600);

Timer timer;

SamplingPotentiometer potLeft(A0, 3.3, 10);
SamplingPotentiometer potRight(A1, 3.3, 10);

char buffer[256];
int buffer_length = 0;


void td1() {
    timer.start();

    DigitalIn up(A2);
    DigitalIn down(A3);
    DigitalIn left(A4);
    DigitalIn right(A5);

    lcd.locate(0, 0);
    lcd.printf("Task 2(Up), Task 3(Down)");
    lcd.locate(0, 15);
    lcd.printf("Task 4(right), Test(Left)");

    bt.params["fwd"] = 0.5;
    bt.params["turn"] = 1.75;
    bt.params["t2"] = 1;
    bt.params["1"] = 0.9;

    while(true) {
        if (up.read() == 1) {
            lcd.cls();
            task2();
        }
        if (down.read() == 1) {
            lcd.cls();
            task3();
        }
        if (right.read() == 1) {
            lcd.cls();
            task4();
        }
    }
}