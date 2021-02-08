#ifndef _UART234_H
#define _UART234_H

#include "stm32f4xx_hal.h"

#define Ov3Mode_QrCode 0
#define Ov3Mode_ColorBlock 1

extern uint8_t Color[6]; //保存接收到的颜色的数组
extern uint8_t Ov3Mode;				//要进行的识别模式,默认二维码
extern uint32_t Catch1Time[6]; //机械臂动作时间-原料区

extern void led_shan(void);



void Uart2_servoCtr(uint8_t CMD);
void Uart3_readQRcode(void);
void Uart3_readColor(void);

#endif
