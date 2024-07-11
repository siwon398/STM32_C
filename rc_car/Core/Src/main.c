/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay_us.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TRIG_PORT GPIOC
#define TRIG_PIN  GPIO_PIN_0

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Do 383
#define Re 340
#define Mi 304
#define Fa 287
#define Sol 255
#define Ra 227
#define qNote 150
#define wNote 1800

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int _write(int file, unsigned char* p, int len)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, p, len, 100);
    return (status == HAL_OK ? len : 0);
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rxData[1];
uint16_t IC_Value01=0;
uint16_t IC_Value02=0;
uint16_t IC_Value11=0;
uint16_t IC_Value12=0;
uint16_t IC_Value21=0;
uint16_t IC_Value22=0;
uint16_t echoTime0=0;
uint16_t echoTime1=0;
uint16_t echoTime2=0;
uint16_t captureFlag0=0;
uint16_t captureFlag1=0;
uint16_t captureFlag2=0;
uint16_t distance0=0;
uint16_t distance1=0;
uint16_t distance2=0;
uint8_t speedState=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART6)
  {
    HAL_UART_Transmit_IT(&huart2, rxData,  1);
    HAL_UART_Receive_IT(&huart2, rxData,  1);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    if(captureFlag0  ==0)
    {
      IC_Value01 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

      captureFlag0=1;

      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
    }
    else if(captureFlag0 ==1)
    {
      IC_Value02 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

      __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);


      if(IC_Value02 > IC_Value01)
      {
        echoTime0 = IC_Value02 - IC_Value01;
      }

      else if(IC_Value01 > IC_Value02)
      {
        echoTime0 = (0xffff - IC_Value01) + IC_Value02;
      }

      distance0 = echoTime0 / 58;

      captureFlag0 = 0;

      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
      __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);

    }
  }
  ///////////////////////////////////////////////////////////////////////
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      if(captureFlag1 ==0)
      {
        IC_Value11 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        captureFlag1=1;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
      }
      else if(captureFlag1==1)
      {
        IC_Value12 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
//        __HAL_TIM_SET_COUNTER(&htim4,0);
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0);

        if(IC_Value12 > IC_Value11)
        {
          echoTime1 = IC_Value12 - IC_Value11;
        }
        else if(IC_Value11 > IC_Value12)
        {
          echoTime1 = (0xffff - IC_Value11) + IC_Value12;
        }
        distance1 = echoTime1 / 58;
        captureFlag1 = 0;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
        __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC2);
      }
    }
  //////////////////////////////////////////////////////////////////////////
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
      {
        if(captureFlag2 ==0)
        {
          IC_Value21 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
          captureFlag2=1;
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if(captureFlag2==1)
        {
          IC_Value22 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);

          __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);

          if(IC_Value22 > IC_Value21)
          {
            echoTime2 = IC_Value22 - IC_Value21;
          }
          else if(IC_Value21 > IC_Value22)
          {
            echoTime2 = (0xffff - IC_Value21) + IC_Value22;
          }
          distance2 = echoTime2 / 58;
          captureFlag2 = 0;
          __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
          __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC3);
        }
      }


}
  /////////////////////////////////////////////////////////////////////////////


void HCS_SR04(void)
{
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
  delay_us(10);
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

  __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC1);
  __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC2);
  __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC3);

}

void speedLow(void)
{
  TIM3->CCR1 =400;
  TIM3->CCR2 =400;
}

void speedHigh(void)
{
  TIM3->CCR1 =1000;
  TIM3->CCR2 =1000;
}
void controlSpeed1(void)
{
  if(speedState == 0)
  {
    speedLow();
  }
  else if(speedState == 1)
  {
   speedHigh();
  }
}

void buzzer(void)
{
  uint16_t song[]={Do,Re, Mi, Re, Do, Re, Mi};
  uint16_t time[]={qNote,qNote,qNote,qNote,qNote,qNote,qNote};
  for(int i = 0;i<=sizeof(song);i++)
  {
    TIM5->PSC = song[i];
    HAL_Delay(time[i]);
    TIM5->CCR1 = 0;
    HAL_Delay(5);
    TIM5->CCR1 = 500;
  }
}


void autoMode(void)
{
  //controlSpeed1();//오른쪾이 1

  HAL_UART_Receive_IT(&huart6, rxData, 1);

  if ( distance1 <= 8)  //LEFT
      {
      TIM3->CCR1 =800;
      TIM3->CCR2 =800;
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
      }

  else if (distance2 <= 8)//RIGHT
      {
       TIM3->CCR1 =800;//strong
       TIM3->CCR2 =800;
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
      }


  else if ((distance0 <= 30) && (distance1 < distance2))  //LEFT
      {
      TIM3->CCR1 =800;
      TIM3->CCR2 =800;
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
      }


  else if ((distance0 <= 30) && (distance1 > distance2))//RIGHT
      {
       TIM3->CCR1 =800;//strong
       TIM3->CCR2 =800;
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);

      }
  else if ((distance0 <= 30) && (distance2 = distance1))//BACK
  {
    TIM3->CCR1 =400;
    TIM3->CCR2 =400;
    //speedLow();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);

  }
  else //FORWARD
  {
    TIM3->CCR1 =800;
    TIM3->CCR2 =800;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
  }


//  else if((distance0 > 30 ) && (distance2 <= 30) && (distance1 <= 30))//FORWARD( SIDE BLOCK )
//    {
//      TIM3->CCR1 =400;
//      TIM3->CCR2 =400;
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
//      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
//    }
//  else if((distance0 <= 30 ) && (distance1 > 30) && (distance2 > 30))//FORWARD(ONLY FORWARD BLOCK)
//    {
//    TIM3->CCR1 =400;
//        TIM3->CCR2 =400;
//        //speedLow();
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
//
//    }
//  else if((distance0 > 30 ) && (distance1 <= 30) && (distance2 > 30))//FORWARD(ONLY RIGHT BLOCK)
//      {
//        TIM3->CCR1 =400;
//        TIM3->CCR2 =400;
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
//      }
//  else if((distance0 > 30 ) && (distance1 > 30) && (distance2 <= 30))//FORWARD(ONLY LEFT BLOCK)
//       {
//         TIM3->CCR1 =400;
//         TIM3->CCR2 =400;
//         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
//         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
//         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
//         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
//       }
}

//////////////////////////////////////////////////////////////////////
void manualJoystick(void)
{
  HAL_UART_Receive_IT(&huart6, rxData, 1);
  if (rxData[0] == 'F')
      {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
      }
      else if (rxData[0] == 'B')
      {

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);      }
      else if (rxData[0] == 'L')
      {
        speedLow();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);//1001
      }
      else if (rxData[0] == 'R')
      {
        speedLow();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);//0110
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
      }

      else if (rxData[0] == 'X')
      {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);

      }else if (rxData[0] == '0')
      {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);

      }
      else if (rxData[0] =='T')
      {
        autoMode();
      }

      else if(rxData[0] == 'S')
       {
         speedState = !speedState;
         controlSpeed1();
       }

//      if (rxData[0] =='C')
//      {
//        buzzer();
//
//      }
}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
   HAL_TIM_Base_Start(&htim11);
   HAL_TIM_Base_Start(&htim4);
   HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
   HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
   HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);
   HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);

   HAL_Delay(1000);

   HAL_UART_Receive_IT(&huart6, rxData, 1);




  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//    printf("%d, %d, %d  \r\n", distance0, distance1,distance2);



//    manualJoystick();
    //controlSpeed1();
//    HAL_UART_Receive_IT(&huart6, rxData, 1);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
