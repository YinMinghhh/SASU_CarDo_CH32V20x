#include "gpio.h"
#include "ch32v20x_misc.h"
/*
********************************************************************************************************
*                                               示例代码
*                                             EXAMPLE  CODE                                             
*
*                             (c) Copyright 2021; SaiShu.Lcc.; Leo
*                                 版权所属[北京赛曙科技有限公司]
*
*               The code is for internal use only, not for commercial transactions(开源学习,请勿商用).
*               The code ADAPTS the corresponding hardware circuit board(代码使用CarDo智控板), 
*               the specific details consult the professional(欢迎联系我们).
*********************************************************************************************************
*/

GpioStruct gpioStr;
BuzzerStruct buzzerStr;

/**
* @brief        GPIO外设初始化
* @param        
* @ref          
* @author       Leo
* @note         外设：蜂鸣器 x1  状态LED x1  按键输入 x1
**/
void GPIO_Initialize(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
	
    //蜂鸣器IO初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_3);

    //LED灯IO初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    
    //按键IO初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);
    //使能按键外部中断
    EXTI_InitStructure.EXTI_Line    = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    //BuzzerInit
    buzzerStr.Counter = 0;
    buzzerStr.Cut = 0;
    buzzerStr.Enable = false;
    buzzerStr.Times = 0;
}


//----------------------------------------------[UNIT-按键及数字输入中断]----------------------------------------------------------

/**
* @brief        按键A中断响应函数
* @param        
* @ref          
* @author       Leo
* @note         
**/
void EXTI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2)==1)	 		
    {				 
        if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))	//按键按下
        {
            gpioStr.KeyPress = true;
            GPIO_BuzzerEnable(BuzzerDing);
        }
        else	//按键弹起
        {
            gpioStr.KeyPress = false;
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line2);
}

//------------------------------------------------[END]-------------------------------------------------------------




/**
* @brief        GPIO线程控制器
* @param        
* @ref          
* @author       Leo
* @note         
**/
void GPIO_Timer(void)
{
    //蜂鸣器控制
    if(buzzerStr.Enable)
    {
        buzzerStr.Counter++;
        
        if(buzzerStr.Cut<buzzerStr.Counter)
            buzzerStr.Counter = buzzerStr.Cut;
    }
    
    //LED闪烁
    gpioStr.CounterLed++;
}



/**
* @brief        GPIO逻辑处理函数
* @param        
* @ref          
* @author       Leo
* @note         
**/
void GPIO_Handle(void)
{
    //蜂鸣器控制
    if(buzzerStr.Enable && !buzzerStr.Silent)
    {
        if(buzzerStr.Times<=0)
        {
            BUZZER_OFF;
            buzzerStr.Enable = false;
            return;
        }
        else if(buzzerStr.Cut<=buzzerStr.Counter)
        {
            BUZZER_REV;
            buzzerStr.Times--;          
            buzzerStr.Counter = 0;
        }
    }
    else
        BUZZER_OFF;
    
    //LED控制
    if(gpioStr.CounterLed > 100)	    //100ms
    {
        LED_REV;
        gpioStr.CounterLed = 0;
    }
}


/**
* @brief        蜂鸣器使能
* @param        buzzer：蜂鸣器工作模式
* @ref          
* @author       Leo
* @note         
**/
void GPIO_BuzzerEnable(BuzzerEnum buzzer)
{
	switch(buzzer)
	{
		case BuzzerOk:
			buzzerStr.Cut = 70;		    //70ms
			buzzerStr.Enable = true;
			buzzerStr.Times = 4;
			break;
		
		case BuzzerWarnning:
			buzzerStr.Cut = 100;		//100ms
			buzzerStr.Enable = true;
			buzzerStr.Times = 10;
			break;
		
		case BuzzerSysStart:
			buzzerStr.Cut = 60;			//60ms
			buzzerStr.Enable = true;
			buzzerStr.Times = 6;  
			break;
        
        case BuzzerDing:
			buzzerStr.Cut = 30;			//30ms
			buzzerStr.Enable = true;
			buzzerStr.Times = 2;  
			break;
        
        case BuzzerFinish:
			buzzerStr.Cut = 200;		//200ms
			buzzerStr.Enable = true;
			buzzerStr.Times = 6;  
			break;
	}
	
    buzzerStr.Counter = 0;
}


