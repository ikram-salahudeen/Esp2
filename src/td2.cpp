#include "td1.h"
#include "Controller.h"

const AnalogIn lineSensors[5] = {LINE_SENSE_PINS};
const DigitalOut lineLeds[5] = {LINE_SENSE_PINS};



int main() {
    Controller ctrl;

    ctrl.init();

    while(true) {
        ctrl.loop();
    }
}