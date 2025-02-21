#include "td1.h"
#include "pins.h"
#include "C12832.h"



float TURNING_RADIUS = 100;
float FORWARD_POWER = 0.5;
float TURNING_POWER = 0.2;



void test() {

    //printf("Enc");
    while(true) {
         //printf("%i %i\n", (int) L.encoder.getPulses(), (int) R.encoder.getPulses());
         wait(1);
    };
}

void turn(int dir /* 1 right, -1 left */) {
    float distance = (3.14f / 2) * TURNING_RADIUS;

    L.setPower(-0.5 * dir);
    R.setPower( 0.5 * dir);

    int pulses = distance * (256.0f / (WHEEL_DIAMETER * 3.14f));
    int targetL = L.encoder.getPulses() - dir * pulses;
    int targetR = R.encoder.getPulses() + dir * pulses;

    while (dir * L.encoder.getPulses() > dir * targetL || dir * R.encoder.getPulses() < dir * targetR) {
        if (dir * L.encoder.getPulses() <= dir * targetL ) L.setPower(0);
        if (dir * R.encoder.getPulses() >= dir * targetR ) R.setPower(0);
    }
    wait(0.2);

    L.setPower(0);
    R.setPower(0);
}

void forward(float distance /* in mm */) {
    L.setPower(0.5);
    R.setPower(0.5);

    int pulses = distance * (256.0f / (WHEEL_DIAMETER * 3.14f));
    int targetL = L.encoder.getPulses() + pulses;
    int targetR = R.encoder.getPulses() + pulses;
    
    while (L.encoder.getPulses() < targetL || R.encoder.getPulses() < targetR) {
        if (L.encoder.getPulses() >= targetL ) L.setPower(0);
        if (R.encoder.getPulses() >= targetR ) R.setPower(0);
        lcd.locate(8,20);
        lcd.printf("L: %02.1d  R: %02.1d", L.encoder.getPulses(), R.encoder.getPulses());
    }

    L.setPower(0);
    R.setPower(0);

    wait(0.2);
}

void square() {
    wait(2);

    printf("2");
    forward(1000);
    printf("3");
    turn(1);
    
    printf("4");
    forward(100);
    turn(1);
    
    forward(100);
    turn(1);

    forward(100);
    turn(1);

}


void task4()
{
   
    

    DigitalOut en (ENABLE);
    en.write(1);


    L.setMode(Bipolar);
    L.setPower(0);

    R.setMode(Bipolar);
    R.setPower(0);
   
   
   lcd.locate(8,10);
    
         square();
    

}
