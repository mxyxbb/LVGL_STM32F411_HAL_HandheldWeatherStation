/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPMODE_H__
#define __LPMODE_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "st7735/st7735.h"

// STOP模式唤醒, 配置时钟
void module_pwr_stop_mode_wake(void);
// 进入STOP模式,靠外部中断唤醒
void module_pwr_enter_stop_mode(void);

#endif /* __LPMODE_H__ */