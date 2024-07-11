

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_

#include "delay.h"

#define STEPS_PER_REVOLUTION  4096
#define DIR_CW                0   // 시계 방향
#define DIR_CCW               1   // 반시계 방향

#define IN1_PIN   GPIO_PIN_1
#define IN2_PIN   GPIO_PIN_15
#define IN3_PIN   GPIO_PIN_14
#define IN4_PIN   GPIO_PIN_13

#define IN1_GPIO_PORT   GPIOB
#define IN2_GPIO_PORT   GPIOB
#define IN3_GPIO_PORT   GPIOB
#define IN4_GPIO_PORT   GPIOB

static const uint8_t HALF_STEP_SEQ[8][4]={
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

void stepMotor(uint8_t step);
void rotateStep(uint16_t steps, uint8_t direction); // 4096의 step -> 16bit
void rotateDegree(uint16_t degree, uint8_t direction);


#endif /* INC_STEPPER_H_ */
