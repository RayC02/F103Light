#ifndef __CAR_H__
#define __CAR_H__
#include "main.h"
void LeftMotor_Go(void);
void LeftMotor_Back(void);
void LeftMotor_Stop(void);
void MotorControl(char motorDirection,int leftMotorPWM/*, int rightMotorPWM*/);
#endif