#include "car.h"
#include "tim.h"
#include "usart.h"

void LeftMotor_Go(void) //左电机正转 IN1输出低电平 IN2输出高电平
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
		printf("LeftMotor is go\r\n");
}


void LeftMotor_Back(void)  //左电机反转	IN1输出高电平 IN2输出低电平
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);

		printf("LeftMotor is back\r\n");
}


void LeftMotor_Stop(void)  //左电机停止 IN1和IN2输出相同电平
{
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
		printf("LeftMotor is stop\r\n");
}





void MotorControl(char motorDirection,int LeftMotorPWM)  //左电机调速 ENA的PWM控制
{
    switch(motorDirection){
        case 0:
							LeftMotor_Go();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							printf("LeftMontor is change speed\r\n");

				break;
				case 1:
							LeftMotor_Back();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							printf("LeftMontor is change speed\r\n");
				break;
				case 2:
							LeftMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
							printf("LeftMontor is change speed\r\n");

				break;
			default:
				break;
		}

}
