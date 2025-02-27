#include "mbed.h"
#include "pins.h"

class Controller {
    AnalogIn lineSensors[5];
    DigitalOut lineLeds[5];

    Controller(): lineSensors(LINE_SENSE_PINS), lineLeds(LINE_LED_PINS) {};

    void loop();

}