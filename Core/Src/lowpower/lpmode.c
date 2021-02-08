#include "lpmode.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "dma.h"

// 进入低功耗模式  -如果是中断就先执行中断的程序,然后执行上次停下的代码
void module_pwr_enter_sleep_mode(void)
{
	user_main_printf("begin sleep mode\r\n");
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
}
 
 
// 进入STOP模式,靠外部中断唤醒
void module_pwr_enter_stop_mode(void)
{
	user_main_printf("begin stop mode\r\n");
	

	LCD_BLK_Clr();
	BuzzerClr();
	BTClr();

//	HAL_ADC_MspDeInit(&hadc1);
//	HAL_I2C_MspDeInit(&hi2c1);
////	HAL_SPI_MspDeInit(&hspi1);
//	HAL_TIM_Base_MspDeInit(&htim10);
//	HAL_UART_MspDeInit(&huart1);
//	HAL_UART_MspDeInit(&huart2);
////	__HAL_RCC_GPIOA_CLK_DISABLE();
//	__HAL_RCC_GPIOB_CLK_DISABLE();
//	__HAL_RCC_GPIOC_CLK_DISABLE();
//	__HAL_RCC_GPIOH_CLK_DISABLE();
//	__HAL_RCC_DMA1_CLK_DISABLE();
//	__HAL_RCC_DMA2_CLK_DISABLE();
	HAL_SuspendTick();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);   
}
 
// 进入待机模式
void module_pwr_enter_standby_mode(void)
{
	 user_main_printf("begin standby mode\r\n");
   
 
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
 
  /* Clear all related wakeup flags*/
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Enable WakeUp Pin PWR_WAKEUP_PIN1 connected to PA.00 使能PA00的唤醒功能*/
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
 
  /* Enter the Standby mode进入待机模式 */
  HAL_PWR_EnterSTANDBYMode();
	
	
}
 
extern void SystemClock_Config(void);
// STOP模式唤醒, 配置时钟
void module_pwr_stop_mode_wake(void)
{
//	HAL_Init();
	// 配置系统时钟
	SystemClock_Config();
	// 恢复系统时钟
	HAL_ResumeTick();
////	__HAL_RCC_GPIOB_CLK_ENABLE();
////	__HAL_RCC_GPIOC_CLK_ENABLE();
////	__HAL_RCC_GPIOH_CLK_ENABLE();
////	__HAL_RCC_DMA1_CLK_ENABLE();
////	__HAL_RCC_DMA2_CLK_ENABLE();
//	MX_GPIO_Init();
//  MX_DMA_Init();
//  MX_ADC1_Init();
//  MX_I2C1_Init();
////  MX_SPI1_Init();
//  MX_TIM10_Init();
//  MX_USART1_UART_Init();
//  MX_USART2_UART_Init();
//	HAL_TIM_Base_Start_IT(&htim10);
	BTSet();
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  LCD_BLK_Set();
	
}