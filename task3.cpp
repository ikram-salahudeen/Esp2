#include "td1.h"

void task3() {
    while (true) {

        lcd.locate(8,20);
        lcd.printf("L: %02.1d  R: %02.1d", L.encoder.getPulses(), R.encoder.getPulses());

   }
}