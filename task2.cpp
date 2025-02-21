#include "mbed.h"
#include "C12832.h"
#include "td1.h"
#include "pins.h"


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

    // get minutes
    int getPercentage() {
        return static_cast<int>(currentValue);
    }
 
};

SamplingPotentiometer potLeft(A0, 3.3, 10);
SamplingPotentiometer potRight(A1, 3.3, 10);

//PwmOut motor_pwm_left(PB_13);  // PB13 作为 PWM 输出
float leftValue = 0.8f, rightValue = 0;



void task2()
{                  
    //motor_pwm_left.period(1.0 / 10000.0);  // 设置 PWM 频率为 10kHz（周期 = 1/10000 秒）
    
    
    DigitalOut en (ENABLE);
    en.write(1);
    
    while(true) {
       leftValue = potLeft.getPercentage();
        rightValue = potRight.getPercentage();
        float duty_cycle_left = leftValue / 100.0;  // 计算占空比 (0.0 - 1.0)
        float duty_cycle_right = rightValue / 100.0;  // 计算占空比 (0.0 - 1.0)
        lcd.rect(0, 0, 25, 25, 0);  // 画黑色矩形覆盖上次显示的文字
        lcd.locate(10, 5);
        lcd.printf("L Pot: %d %% | %.2f", leftValue, duty_cycle_left);
        lcd.locate(10, 15);
        lcd.printf("R Pot: %d %% | %.2f", rightValue, duty_cycle_right);


        printf("%f\n", leftValue);

        L.direction.write(1);
        L.bipolar.write(1);
        L.pwm.write(duty_cycle_left);
        R.direction.write(1);
        R.bipolar.write(1);
        R.pwm.write(duty_cycle_right);



        wait_us(100000);  // 100ms 延迟
    }
}

