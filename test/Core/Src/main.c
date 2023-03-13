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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "car.h"
#include "cmd.h"
#include "esp.h"
#include "ir.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define REMOTE_ID 0

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	uint8_t uart2_rx[]={0};	
	uint8_t value1[100]={0};

	uint32_t temp = 0;//用于接收数据

	uint16_t i =0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uartcall(void);
uint8_t Remote_Scan(void);
	


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
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  //HAL_UART_Transmit(&huart1,(uint8_t *)"hello",6,1000);
	//HAL_UART_Transmit(&huart2,(uint8_t *)"hello",6,1000);
	printf("STM32rebotcar");

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
	
		HAL_TIM_Base_Start_IT(&htim3); 
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1); 
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE); 
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	HAL_UART_Receive_DMA(&huart1,uart1_rx_buffer,BUFFER_SIZE);

	
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); 
	HAL_UART_Receive_DMA(&huart2,uart2_rx_buffer,BUFFER_SIZE);
	
	
				int pwm_val=1000;
                                                                                              


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	

		if (IR_Scanf(&temp))
		{
			printf("%#X\r\n", temp);
		}
		switch(temp)	{													
			case ir1:
				MotorControl(3,1000,1000);
				temp=0;
			break;
			
			case ir2:
				MotorControl(0,1000,1000);
			temp=0;
			break;
			
			case ir3:
				MotorControl(2,1000,1000);
			temp=0;
			break;
			
			case ir5:
				MotorControl(4,1000,1000);
			temp=0;
			break;
			
			case ir7:
				MotorControl(5,1000,1000);
			temp=0;
			break;
			
			case ir8:
				MotorControl(1,1000,1000);
			temp=0;
			break;
			
			case ir9:
				MotorControl(6,1000,1000);
			temp=0;
			break;
			
			case ir4:
        pwm_val=pwm_val-100;
				MotorControl(2,pwm_val,pwm_val);
			temp=0;
			break;
			case ir6:
        pwm_val=pwm_val+100;
				MotorControl(3,pwm_val,pwm_val);
			temp=0;
			break;
			default:
				break;
		}
	
		
	/*
										
		for(i=0;i<1;i++)
	{
		while(!(USART1->SR & (1<<5))){};
		value1[i]=USART1->DR;
	}
			printf("%s",value1);
      if(value1[0] == 0x67)																//g
      {
								HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
				MotorControl(0,1000,1000);
			}
			else if(value1[0] == 0x62)									//b
       {
								HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
					MotorControl(1,1000,1000);
        }
       else if(value1[0] == 0x72)									//r
       {				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
					MotorControl(2,1000,1000);
        }
			 else if(value1[0] == 0x6c)									//l
       {				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
					MotorControl(3,1000,1000);
        }
			 else if(value1[0] == 0x73)									//s
       {				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
					MotorControl(4,0,0);
        }
			 memset(value1, 0, sizeof(value1));
 */

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
/*void uartcall()
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
			MotorControl(0,200,200);
	}
	else if(!(strcmp((const char *)value1,"bk")))
	{
			MotorControl(1,200,200);
	}
		else if(!(strcmp((const char *)value1,"rg")))
	{
			MotorControl(2,200,0);
	}
		else if(!(strcmp((const char *)value1,"lg")))
	{
			MotorControl(3,0,200);
	}
	else if(!(strcmp((const char *)value1,"st")))
	{
			MotorControl(4,0,0);
	}
}
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获回调函数
{
	if (TIM3 == htim->Instance)
	{
		IR_CaptureCallback();
	}
}

/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit(&huart1, uart2_rx,sizeof( uart2_rx),100);	
	HAL_UART_Receive_IT(&huart2, uart2_rx,sizeof( uart2_rx));	
}*/

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
