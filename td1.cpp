#include "td1.h"
#include "pins.h"


Wheel L(MOTOR_L_PWM, MOTOR_L_DIR, MOTOR_L_BIPOLAR, ENCODER_L_A, ENCODER_L_B, PWM_FREQUENCY);
Wheel R(MOTOR_R_PWM, MOTOR_R_DIR, MOTOR_R_BIPOLAR, ENCODER_R_A, ENCODER_R_B, PWM_FREQUENCY);

C12832 lcd(D11, D13, D12, D7, D10);

Timer timer;


int main() {
    timer.start();

    DigitalIn up(A2);
    DigitalIn down(A3);
    DigitalIn left(A4);
    DigitalIn right(A5);

    lcd.locate(0, 0);
    lcd.printf("Task 2(Up), Task 3(Down)");
    lcd.locate(0, 15);
    lcd.printf("Task 4(right), Test(Left)");
    while(true) {
        if (up.read() == 1) {
            task2();
        }
        if (down.read() == 1) {
            task3();
        }
        if (right.read() == 1) {
            task4();
        }
        if (left.read() == 1) {
            
        }
    }
}