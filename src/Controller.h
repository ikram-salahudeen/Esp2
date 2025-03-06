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
    float sample_sum = 0;
    int which_led = 0;
    int sample_count = 0;

    Ticker adc_ticker;

    /*
        Motors
    */

    Wheel L, R;
    


    void adc_isr() {
        sample_sum += lineSensors[which_led].read();
        sample_count++;

        if (sample_count == bt.params["S"]) {
            // Sampling for 1 led finished
            readings[which_led] = sample_sum/sample_count;

            lineLeds[which_led].write(0);

            sample_count = 0;
            which_led++;

            lineLeds[which_led].write(1);            

            if (which_led == 5) which_led = 0;
        }

    }



    void init() {
        bt.params["S"] = 1; // number of samples
        bt.params["SP"] = 0.00001; // sampling period
        bt.params["LT"] = 0.5; // threshold for led to be on
        adc_ticker.attach(callback(this, &Controller::adc_isr), bt.params["SP"]);

        for (int i = 0; i < 5; i ++) {
            lineLeds[i].write(0);
        }

        
    
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