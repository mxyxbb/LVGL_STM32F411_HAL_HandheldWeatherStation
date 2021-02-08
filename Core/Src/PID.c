/*
 * PID.c
 *
 *  Created on: Jul 15, 2019
 *      Author: XinLiu
 */
#include "PID.h"
double middle = 8400; 
#define KP 100
#define KI 60
#define KD 10

#define UPPER  16799
#define LOWER  0
PID udPIDParameter0;
PID udPIDParameter1;
PID udPIDParameter2;
PID udPIDParameter3;


void User_PID_Init(PID *userPara)
{
	userPara->Kp = KP;
	userPara->Ki = KI;
	userPara->Kd = KD;
	userPara->SetValue = 0.0;
	userPara->error = 0.0;
	userPara->error_last = 0.0;
	userPara->error_pre = 0.0;
	userPara->integral = 0.0;
	userPara->pid_Calc_out = 0.0;
}


float User_PID_Calc(PID *uPID,float TargetVal,float CurrentVal)
{
    uPID->SetValue = TargetVal;

    uPID->error = uPID->SetValue - CurrentVal;
    uPID->integral += uPID->error;
    uPID->pid_Calc_out = middle + (uPID->Kp * uPID->error) + (uPID->Ki * uPID->integral)
    					+ uPID->Kd * (uPID->error - uPID->error_last);
    uPID->error_last = uPID->error;

    if(uPID->pid_Calc_out > UPPER)	uPID->pid_Calc_out = UPPER;
    else if(uPID->pid_Calc_out < LOWER) uPID->pid_Calc_out = LOWER;

    return (uPID->pid_Calc_out * 1.0);
}


float User_PID_ADJ(PID *uPID,float TargetVal)
{
    float Out;
    uPID->SetValue = TargetVal;
    Out = (uPID->Kp * (uPID->error - uPID->error_last))  + (uPID->Ki * uPID->error)
    			+ uPID->Kd * (uPID->error-2 * uPID->error_last + uPID->error_pre);
    uPID->error_pre = uPID->error_last;
    uPID->error_last = uPID->error;

    uPID->pid_Calc_out += Out;
    return (uPID->pid_Calc_out);
}

