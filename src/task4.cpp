#include "td1.h"
#include "pins.h"
#include "C12832.h"
#include <cstdio>



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

void turn(int dir /* 1 right, -1 left */, float modifier) {
    float distance =  3 * (3.14f / 2) * TURNING_RADIUS;
    if (dir == 1) { 
        R.setPower(0.6);
    
        int pulses = modifier *  1.32 * bt.params["turn"] * distance * 256.0f / (WHEEL_DIAMETER * 3.14f);
        int targetR = R.encoder.getPulses() + dir * pulses;

        lcd.locate(0, 0);
        lcd.printf("pulses %d %d\n %d %d\n", L.encoder.getPulses(), R.encoder.getPulses(), 0, targetR);

        while (dir * R.encoder.getPulses() < dir * targetR) {
            if (dir * R.encoder.getPulses() >= dir * targetR ) R.setPower(0);
        }
    } else {
        L.setPower(0.6);
        dir *= -1;
    
        int pulses = modifier *  1.32 * bt.params["turn"] * distance * 256.0f / (WHEEL_DIAMETER * 3.14f);
        int targetL = L.encoder.getPulses() + dir * pulses;

        lcd.locate(0, 0);
        lcd.printf("pulses %d %d\n %d %d\n", L.encoder.getPulses(), R.encoder.getPulses(), 0, 0);

        while (dir * L.encoder.getPulses() < dir * targetL) {
            if (dir * L.encoder.getPulses() >= dir * targetL ) L.setPower(0);
        }
    }

    
    wait(0.2);

    L.setPower(0);
    R.setPower(0);
}


void forward(float distance /* in mm */) {
    L.setPower(0.5);
    R.setPower(0.5);
    // distance = (pulses / (256*15)) * (WHEEL_DIAMETER * 3.14f)
    int pulses = distance * 15 * 256.0f / (WHEEL_DIAMETER * 3.14f);
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


void rev(float distance /* in mm */) {
    L.setPower(-0.5);
    R.setPower(-0.5);
    // distance = (pulses / (256*15)) * (WHEEL_DIAMETER * 3.14f)
    int pulses = distance * 15 * 256.0f / (WHEEL_DIAMETER * 3.14f);
    int targetL = L.encoder.getPulses() - pulses;
    int targetR = R.encoder.getPulses() - pulses;
    
    while (L.encoder.getPulses() > targetL || R.encoder.getPulses() > targetR) {
        if (L.encoder.getPulses() <= targetL ) L.setPower(0);
        if (R.encoder.getPulses() <= targetR ) R.setPower(0);
        lcd.locate(8,20);
        lcd.printf("L: %02.1d  R: %02.1d", L.encoder.getPulses(), R.encoder.getPulses());
    }

    L.setPower(0);
    R.setPower(0);

    wait(0.2);
}

void square() {
    float distance =  3 * (3.14f / 2) * TURNING_RADIUS;
    wait(2);
    //rev(50);

    // Clockwise
    
        forward(100 * bt.params["1"]);

        wait(1);
        turn(1, 1);
        wait(1);

        forward(100 * bt.params["fwd"]);

        wait(1);
        turn(1, 1);
        wait(1);

        forward(100 * bt.params["fwd"]);

        wait(1);
        turn(1, 1);
        wait(1);

        forward(100 * bt.params["fwd"]);

        wait(1);
        turn(1, 1);
        wait(1);
    
    
    wait(1);
    rev(50);
    turn(-1,1);
    wait(1);

    // anti-Clockwise
    forward(100 * bt.params["1"]);

        wait(1);
        turn(-1, 1);
        wait(1);

        forward(100 * bt.params["fwd"]);

        wait(1);
        turn(-1, 1);
        wait(1);

        forward(100 * bt.params["fwd"]);

        wait(1);
        turn(-1, 1);
        wait(1);

        forward(100 * bt.params["fwd"]);

        wait(1);
        turn(-1, 1);
        wait(1);

}


void task4()
{
   
    

    DigitalOut en (ENABLE);
    en.write(1);


    L.setMode(Bipolar);
    L.setPower(0);

    R.setMode(Bipolar);
    R.setPower(0);
    
    

         square();

    
    

}
