//#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "ControlTask.h"
//#include "PID.h"
#include "multi_button/multi_button.h"
#include "rtc.h"
#include "lvgl/lvgl.h"
#include "lowpower/lpmode.h"

uint8_t buzzerEN=0;
uint8_t get_time_flag=0;
extern uint8_t bme280ready;
extern uint8_t bt_update;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t Time1_ms = 0;
		static uint16_t Time2_ms = 0;
		static uint16_t Time3_ms = 0;
		static uint16_t Time4_ms = 0;
	
		static uint16_t cnt1_buzzer=0;
	
    if (htim == (&htim10))//ÖÐ¶ÏÆµÂÊÎª10kHz
    {
			Time1_ms++;			// Ã¿0.1msÔöÒ»
			Time2_ms++;			// Ã¿0.1msÔöÒ»
			Time3_ms++;			// æ¯0.1msè‡ªå¢žä¸€
			Time4_ms++;			// æ¯0.1msè‡ªå¢žä¸€
			
			
			if(Time1_ms == 4 && buzzerEN)//ÈÎÎñ1,·äÃùÆ÷Èí¼þPWM£¬ÖÜÆÚÎª0.4ms(2500Hz)
			{
				HAL_GPIO_TogglePin(BUZZER1_EN_GPIO_Port,BUZZER1_EN_Pin);
				if(cnt1_buzzer++>200){
					BuzzerClr();
					buzzerEN=0;
					cnt1_buzzer=0;
				}
			}
			if(Time2_ms == 10000)//ÈÎÎñ2,¶¨Ê±¶ÁÈ¡bme280±êÖ¾Î»£¬ÖÜÆÚÎª2s
			{
				bme280ready=1;
				bt_update=1;
			}
			if(Time3_ms == 50){
				button_ticks();
			}
			if(Time4_ms == 1000)//
			{
				get_time_flag=1;
			}
			
	

			
			if(Time1_ms >= 4){
				Time1_ms = 0;
			}	
			if(Time2_ms >= 10000){
				Time2_ms = 0;
			}	
			if(Time3_ms >= 50){
				Time3_ms = 0;
			}
			if(Time4_ms >= 1000){
				Time4_ms = 0;
			}			
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//	buzzerEN=1;

//	DialPlateSetup();
}
