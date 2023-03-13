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

void RightMotor_Go(void) //右电机正转 IN1输出低电平 IN2输出高电平
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
		printf("RightMotor is go\r\n");
}


void RightMotor_Back(void)  //右电机反转	IN1输出高电平 IN2输出低电平
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);

		printf("RightMotor is back\r\n");
}


void RightMotor_Stop(void)  //右电机停止 IN1和IN2输出相同电平
{
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
		printf("RightMotor is stop\r\n");
}




void MotorControl(char motorDirection,int LeftMotorPWM,int rightMotorPWM)  // ENA的PWM控制
{
    switch(motorDirection){
        case 0:
							LeftMotor_Go();
							RightMotor_Go();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rightMotorPWM);

				break;
				case 1:
							LeftMotor_Back();
							RightMotor_Back();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rightMotorPWM);
							
				break;
				case 2:																							//Right
							LeftMotor_Go();
							RightMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rightMotorPWM);
							HAL_Delay(500);
							LeftMotor_Stop();
							RightMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
							

				break;
				case 3:																							//Left
							LeftMotor_Stop();
							RightMotor_Go();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rightMotorPWM);
							HAL_Delay(500);
							LeftMotor_Stop();
							RightMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);

				break;
				case 4:
							LeftMotor_Stop();
							RightMotor_Stop();
							

				break;
				case 5:																									//left and back
							LeftMotor_Stop();
							RightMotor_Back();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rightMotorPWM);
							HAL_Delay(500);
							LeftMotor_Stop();
							RightMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
				break;
				case 6:																									//right and back
							LeftMotor_Back();
							RightMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,LeftMotorPWM);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rightMotorPWM);
							HAL_Delay(500);
							LeftMotor_Stop();
							RightMotor_Stop();
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
				break;
			default:
				break;
		}

}
