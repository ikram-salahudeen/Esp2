#include "mbed.h"
#include "pins.h"
#include "td1.h"
#include "rtos.h"


extern const AnalogIn lineSensors[5];
extern const DigitalOut lineLeds[5];

struct Controller {
    /*
        Line sensor
    */
    float readings[5];


    /*
        Motors
    */

    Wheel L, R;
    



    void init() {
    
    }

    void loop() {
        bt.serial.printf("Line: %f %f %f %f %f", readings[0], readings[1], readings[2], readings[3], readings[4], readings[5]);

        for (int i = 0; i < 5; i++) {
            if (readings[i] > bt.params["LT"]) {
                lcd.fillrect(1 + i*7, 1, 5, 5, 1);
            } else {
                lcd.rect(1 + i*7, 1, 5, 5, 1);
            }
        }

        wait_us(10000000);
    }

};