#include "multi_button/multi_button.h"
#include "main.h"
#include "rtc.h"
#include "usart.h"
//lvgl includes
#include "lvgl/lvgl.h"
#include "lowpower/lpmode.h"

uint8_t read_button1_GPIO();
uint8_t read_button2_GPIO();
void BTN1_LONG_PRESS_START_Handler(void* btn);
void BTN2_LONG_PRESS_START_Handler(void* btn);
void BTN1_DOUBLE_Click_Handler(void* btn);
void BTN2_DOUBLE_Click_Handler(void* btn);
void animSelect(uint8_t i);
void anim_cnt_lim(void);
	
struct Button button1;
struct Button button2;
extern uint8_t buzzerEN;
extern lv_obj_t * btn1;
extern lv_anim_t a;
uint8_t anim_cnt=0;
uint8_t my_stop=0;

void my_button_init()
{
	button_init(&button1, read_button1_GPIO, 0);//multibtn初始化
	button_init(&button2, read_button2_GPIO, 0);//multibtn初始化
	
//	button_attach(&button1, PRESS_DOWN,       BTN1_PRESS_DOWN_Handler);
//	button_attach(&btn1, PRESS_UP,         BTN1_PRESS_UP_Handler);
//	button_attach(&btn1, PRESS_REPEAT,     BTN1_PRESS_REPEAT_Handler);
//	button_attach(&button1, SINGLE_CLICK,     BTN1_SINGLE_Click_Handler);
	button_attach(&button1, DOUBLE_CLICK,     BTN1_DOUBLE_Click_Handler);
	button_attach(&button1, LONG_PRESS_START, BTN1_LONG_PRESS_START_Handler);
//	button_attach(&btn1, LONG_PRESS_HOLD,  BTN1_LONG_PRESS_HOLD_Handler);

//	button_attach(&button2, PRESS_DOWN,       BTN2_PRESS_DOWN_Handler);
//	button_attach(&btn2, PRESS_UP,         BTN2_PRESS_UP_Handler);
//	button_attach(&btn2, PRESS_REPEAT,     BTN2_PRESS_REPEAT_Handler);
//	button_attach(&button2, SINGLE_CLICK,     BTN2_SINGLE_Click_Handler);
	button_attach(&button2, DOUBLE_CLICK,     BTN2_DOUBLE_Click_Handler);
	button_attach(&button2, LONG_PRESS_START, BTN2_LONG_PRESS_START_Handler);
//	button_attach(&btn2, LONG_PRESS_HOLD,  BTN2_LONG_PRESS_HOLD_Handler);
	button_start(&button1);
	button_start(&button2);
}

void BTN1_DOUBLE_Click_Handler(void* btn)
{
//	buzzerEN=1;
	my_stop=1;
	
}
void BTN2_DOUBLE_Click_Handler(void* btn)
{
//	buzzerEN=1;
	
	
}

#define DATEWINDOW 0x01
#define DIALWINDOW 0x02
uint8_t cur_window=DIALWINDOW;
void BTN1_LONG_PRESS_START_Handler(void* btn)
{
	//enter DATEWINDOW

	
	if(cur_window != DATEWINDOW)
	{
		buzzerEN=1;
		cur_window=DATEWINDOW;
		DialPlateExit();
		LabelTopBarSetup();
	}
	else if(cur_window != DIALWINDOW)
	{
		buzzerEN=1;
		cur_window=DIALWINDOW;
		DateExit();
		DialPlateSetup();
	}
}

void BTN2_LONG_PRESS_START_Handler(void* btn)
{
	
//	if(cur_window != DIALWINDOW)
//	{
//		buzzerEN=1;
//		cur_window=DIALWINDOW;
//		DateExit();
//		DialPlateSetup();
//	}
}






uint8_t read_button1_GPIO()
{
	return HAL_GPIO_ReadPin(INT_TP1_GPIO_Port, INT_TP1_Pin);
}
uint8_t read_button2_GPIO()
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
}