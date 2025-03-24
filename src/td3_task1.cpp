/*\#include "td1.h"
#include "pins.h"
#include "C12832.h"
#include <cstdio>

class SamplingVoltage: public Potentiometer{
    private:
        float samplingFrequency, samplingPeriod;
        Ticker sampler;

    public:
        SamplingVoltage(PinName pin, float v, float time): Potentiometer(pin, v), samplingFrequency(time){
            samplingPeriod = 1/samplingFrequency;
            sampler.attach(callback(this, &Potentiometer::sample), samplingPeriod);
        };
        

};

int main() {
    SamplingVoltage sensor_1(A0, 3.3, 100);
    SamplingVoltage sensor_2(A1, 3.3, 100);
    SamplingVoltage sensor_3(A2, 3.3, 100);
    SamplingVoltage sensor_4(A3, 3.3, 100);
    SamplingVoltage sensor_5(A4, 3.3, 100);

    float sensor1_reading, sensor2_reading, sensor3_reading, sensor4_reading, sensor5_reading;

    while (1) {
        sensor1_reading = sensor_1.getCurrentSampleNorm();
        sensor2_reading = sensor_2.getCurrentSampleNorm();
        sensor3_reading = sensor_3.getCurrentSampleNorm();
        sensor4_reading = sensor_4.getCurrentSampleNorm();
        sensor5_reading = sensor_5.getCurrentSampleNorm();

        if (sensor1_reading < 0.5) sensor1_reading = 0;
        if (sensor2_reading < 0.5) sensor2_reading = 0;
        if (sensor3_reading < 0.5) sensor3_reading = 0;
        if (sensor4_reading < 0.5) sensor4_reading = 0;
        if (sensor5_reading < 0.5) sensor5_reading = 0;

        float sum = sensor1_reading + sensor2_reading + sensor3_reading + sensor4_reading + sensor5_reading;
        float weighted_average = 0;

        if (sum != 0) {
            weighted_average = (-2 * sensor1_reading + -1 * sensor2_reading + 0 * sensor3_reading + 
                                1 * sensor4_reading + 2 * sensor5_reading) / sum;
        } else {
            weighted_average = 0; 
        }

        printf("Sensor Readings: %.2f %.2f %.2f %.2f %.2f\n", sensor1_reading, sensor2_reading, 
               sensor3_reading, sensor4_reading, sensor5_reading);
        printf("Weighted Average: %.2f\n", weighted_average);

        wait(0.01);
    }
}
*/