#include "Controller.h"
#include "bluetooth.h"

//const AnalogIn lineSensors[5] = LINE_SENSE_PINS;
//const DigitalOut lineLeds[5] = LINE_LED_PINS;

Bluetooth bt(PA_11, PA_12, 9600);
Serial USB (USBTX, USBRX, 9600);
Controller ctrl;
Timer timer;


int main() {
    timer.start();

    while (true) {

        ctrl.loop();
        bt.process();
    }
    
}