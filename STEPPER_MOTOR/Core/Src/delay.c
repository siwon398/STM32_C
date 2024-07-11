#include "delay.h"

void delay_us(uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&htim11, 0);
  HAL_TIM_Base_Start(&htim11);
  while(__HAL_TIM_GET_COUNTER(&htim11)<us);
  HAL_TIM_Base_Stop(&htim11);
}
