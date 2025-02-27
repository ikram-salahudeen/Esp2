#include "mbed.h"
#include "C12832.h"
#include "td1.h"
#include "pins.h"


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

