#ifndef __PWM_H__
#define __PWM_H__

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "main.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

#define  MOTOR_PWM_MAX   			1500		//OCR=95%,禁止满占空比输出，造成MOS损坏
#define  MOTOR_PWM_MIN			    -1500		//OCR=95%
#define  MOTOR_SPEED_MAX		    10.0f	 	//电机最大转速(m/s) (0.017,8.04)
#define  PI					        3.141593f   //π
#define  MOTOR_CONTROL_CYCLE	    0.01f    	//电机控制周期T：10ms

/**
* @brief    电机相关
**/
typedef struct 
{
	float ReductionRatio ;					    //电机减速比
	float EncoderLine ; 						//编码器线数=光栅数16*4
	signed int EncoderValue;				    //编码器实时速度
	float DiameterWheel;						//轮子直径：mm
	bool CloseLoop;							    //开环模式
	uint16_t Counter;							//线程计数器
}MotorStruct;


extern MotorStruct motorStr;


void MOTOR_Init(void);
void MOTOR_SetPwmValue(signed int pwm);
void MOTOR_ControlLoop(float speed);
void MOTOR_Timer(void);


//===========================================  End Of File  ===========================================//
#endif


