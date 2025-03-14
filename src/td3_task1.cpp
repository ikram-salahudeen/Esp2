/*#include "td1.h"
#include "pins.h"
#include "C12832.h"
#include <cstdio>

class Potentiometer                                 //Begin Potentiometer class definition
{
private:                                            //Private data member declaration
    AnalogIn inputSignal;                           //Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts; //Float variables to speficy the value of VDD and most recent samples

public:                                             // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}   //Constructor - user provided pin name assigned to AnalogIn...
                                                                        //VDD is also provided to determine maximum measurable voltage
    float amplitudeVolts(void)                      //Public member function to measure the amplitude in volts
    {
        return (inputSignal.read()*VDD);            //Scales the 0.0-1.0 value by VDD to read the input in volts
    }
    
    float amplitudeNorm(void)                       //Public member function to measure the normalised amplitude
    {
        return inputSignal.read();                  //Returns the ADC value normalised to range 0.0 - 1.0
    }
    
    void sample(void)                               //Public member function to sample an analogue voltage
    {
        currentSampleNorm = inputSignal.read();       //Stores the current ADC value to the class's data member for normalised values (0.0 - 1.0)
        currentSampleVolts = currentSampleNorm * VDD; //Converts the normalised value to the equivalent voltage (0.0 - 3.3 V) and stores this information
    }
    
    float getCurrentSampleVolts(void)               //Public member function to return the most recent sample from the potentiometer (in volts)
    {
        return currentSampleVolts;                  //Return the contents of the data member currentSampleVolts
    }
    
    float getCurrentSampleNorm(void)                //Public member function to return the most recent sample from the potentiometer (normalised)
    {
        return currentSampleNorm;                   //Return the contents of the data member currentSampleNorm  
    }

};

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
class SamplingPotentiometer : public Potentiometer { 
private:                                                                 
    float samplingFrequency, samplingPeriod; 
    Ticker sampler;
    float currentValue;

    void sampleAndMap() {
        this->sample();
        float normValue = getCurrentSampleNorm(); // get norm value (0.0 - 1.0)
        currentValue = normValue * 100; 
    }                                                      
 
public:                                                                  
    SamplingPotentiometer(PinName pin, float v, float fs)
        : Potentiometer(pin, v), samplingFrequency(fs) {
        samplingPeriod = 1.0f / samplingFrequency;
        sampler.attach(callback(this, &SamplingPotentiometer::sampleAndMap), samplingPeriod);
    }

    int getPercentage() {
        return static_cast<int>(currentValue);
    }
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
