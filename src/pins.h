#include "mbed.h"

#define MOTOR_L_BIPOLAR PC_3
#define MOTOR_L_DIR PC_1
#define MOTOR_L_PWM PA_15

#define MOTOR_R_BIPOLAR PC_2
#define MOTOR_R_DIR PB_0
#define MOTOR_R_PWM PB_7

#define ENABLE PB_2

#define MICROLAN

#define MOTOR_L_CUR_SENSE1
#define MOTOR_L_CUR_SENSE2
#define MOTOR_R_CUR_SENSE2
#define MOTOR_R_CUR_SENSE1


#define ENCODER_L_A PB_15
#define ENCODER_L_B PB_4

#define ENCODER_R_A PB_13
#define ENCODER_R_B PB_3

#define LINE_SENSE_PINS {NC, NC, NC, NC, NC}

#define LINE_LED_PINS {NC, NC, NC, NC, NC}