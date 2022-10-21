/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "car.h"
#include "cmd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t RX_raspi[];
extern uint16_t recv_end_flag;
extern uint16_t rx_len;
extern uint16_t BUFFER_SIZE;
extern uint8_t uart1_rx_buffer[];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uartcall(void);
//void Usart_Cont1(void);
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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Transmit(&huart1,(uint8_t *)"hello",6,1000);
	printf("STM32-Raspi-ROSrebotcar");
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	//HAL_UART_Receive_DMA(&huart1,(uint8_t *)&uart1_rx_buffer,BUFFER_SIZE);
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&RX_raspi,sizeof((uint8_t *)&RX_raspi));;	

	/*
  //初始�? 字符串命令和对应函数
  CMD_Name_Func car_cmds[] =
   {
        {"go", LeftMotor_Go},                       // �?个结构体变量大小�? 12 (字符串大�?10 + 函数名大�?2)
        {"back",LeftMotor_Back },                     // �?个结构体变量大小�? 12
        {"stop", LeftMotor_Stop},
   };
 
    //将命令添加到列表�?
    register_cmds( car_cmds, ARRAY_SIZE(car_cmds));	// ARRAY_SIZE 用来计算结构体数组中，数组的个数。个�?=结构体�?�长�?/单个数组长度
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		HAL_Delay(200);
		uartcall();
		//Usart_Cont1();
		//match_cmd((char*)RX_raspi);
		
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void uartcall()
{
	uint8_t value1[3]={0};
	uint8_t i;
	for(i=0;i<2;i++)
	{
		while(!(USART1->SR & (1<<5))){};
		value1[i]=USART1->DR;
	}
	value1[2]='\0';
	if(!(strcmp((const char *)value1,"go")))
	{
			MotorControl(0,200);
	}
	else if(!(strcmp((const char *)value1,"bk")))
	{
			MotorControl(1,200);
	}
	else if(!(strcmp((const char *)value1,"st")))
	{
			MotorControl(2,0);
	}
}
/*void Usart_Cont1(void)
{
	uint8_t value[2];
	uint8_t i;
	for(i=0;i<2;i++)
	{
		while(!(USART1->SR & (1<<5))){};
		value[i]=USART1->DR;
	}
	switch(value[0])
	{
		case '0':
			if(value[1]=='1')
			{
				MotorControl(0,200);
			}
			else if(value[1]=='2')
			{
				MotorControl(1,200);
			}
			else if(value[1]=='3')
			{
				MotorControl(0,0);
			}	
			break;
		case '1':
			if(value[1]=='1')
			{
				MotorControl(1,8000);
			}
				
			break;
	}
}*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&RX_raspi,sizeof((uint8_t *)&RX_raspi),100);	
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&RX_raspi,sizeof((uint8_t *)&RX_raspi));	
}


/* USER CODE END 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
