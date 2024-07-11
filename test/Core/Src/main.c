#include<stdint.h>

void delay_count(uint32_t timeCount)
{
  for(;timeCount >0; timeCount--);
}


int main()
{
//  volatile unsigned int *ptr;
//  ptr = 0x40023830;
//  *ptr |=0x01
  //GPIOA의 클럭을 설정
  (*(volatile unsigned *)0x40023830) |= 0x01;

  //GPIOA의 모드를 출력으로 설정
  (*(volatile unsigned *)0x40020000) &= ~(0x3<<10);
  (*(volatile unsigned *)0x40020000) |= (0x1<<10);

  while(1)
  {
    //GPIOA 5번 ON
    (*(volatile unsigned *)0x40020014) |= (1<<5);
    delay_count(0x250000);
    //GPIOA 5번 OFF
    (*(volatile unsigned *)0x40020014) &= ~(1<<5);
    delay_count(0x250000);
  }
}
