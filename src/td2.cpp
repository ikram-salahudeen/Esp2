#include "td1.h"
#include "rtos.h"
#include "RtosTimer.h"
#include "Controller.h"

const AnalogIn lineSensors[5] = {LINE_SENSE_PINS};
const DigitalOut lineLeds[5] = {LINE_SENSE_PINS};


Controller ctrl;

void loop() {

}

int main() {
    RtosTimer timer (&loop);

    ctrl.init();

    timer.start(10);
    
}