/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "ControlTask.h"
#include "BMP280/bmp280.h"

#include "mcu_sdk/bluetooth.h"
/* vim: set ai et ts=4 sw=4: */
#include <string.h>
#include "st7735/st7735.h"
#include "st7735/fonts.h"
#include "st7735/testimg.h"
//lvgl includes
#include "lvgl/lvgl.h"
//multiButton
#include "multi_button/multi_button.h"
//lowpower
#include "lowpower/lpmode.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Buffer used for transmission */
uint8_t aTxStartMessage[] = "****UART-Hyperterminal communication based on IT ****\r\n Enter 10 characters using keyboard :\r\n";
uint8_t aTxEndMessage[] = "\r\nExample Finished\r\n";

/* Buffer used for reception */
uint8_t aRxBuffer[20];
uint8_t bRxBuffer;
uint8_t tuya_test_string[]={0x55,0xAA,0x00,0x0A,0x00,0x00,0x09};


BMP280_HandleTypedef bmp280;
uint16_t size;
uint8_t Data[256];
uint8_t bme280ready=0;

//clock configuration
uint8_t clockTime=0;

//LVGL
extern lv_font_t myFont;			// 声明制作好的字库
uint8_t completed1 = 0;
//lowpower-stop
extern uint8_t my_stop;
//multibutton

//blutetooth flag
uint8_t bt_update=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Buzzer_En(void);
void Buzzer_Clr(void);
void LP_Clr(void);

//void BME280_SendMessage();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void init() {
	LP_Clr();										// 关闭蓝牙低功�?
	bt_protocol_init();					// 蓝牙模组初始�?
  ST7735_Init();							// LCD初始�?
	ST7735_FillScreen(ST7735_BLACK);
	lv_init();									// LittleVGL初始�?
	lv_port_disp_init();        // 显示器初始化
	my_button_init();						//multi_button初始�?
	if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1) != 0x32F2)//RTC时间初始化
		set_time();
	set_alarm();
	HAL_UART_Receive_IT(&huart2, &bRxBuffer, 1);//串口中断接收
}


static void lv_ex_label_1(void)
{
    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(label2, true);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SROLL_CIRC); /*Circular scroll*/
    lv_obj_set_width(label2, 120);
    // Hello world ! Trisuborn.
    lv_label_set_text(label2, "#ff0000 Today# #00ff00 I'm very # #0000ff happy # #0f0f0f lalalala #");
    lv_obj_align(label2, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void lv_test(void)
{
    lv_obj_t *myFont_label = lv_label_create(lv_scr_act(), NULL);
    if(myFont_label == NULL)
    {
        printf("init failed\r\n");
        while(1);
    }
    static lv_style_t myFont_style;
    lv_style_copy(&myFont_style,&lv_style_plain);
    myFont_style.text.font = &myFont;
		
		lv_label_set_long_mode(myFont_label, LV_LABEL_LONG_SROLL_CIRC); /*Circular scroll*/
		lv_obj_set_size(myFont_label, 120,80);
		lv_label_set_recolor(myFont_label, true);
		
    lv_label_set_style(myFont_label,LV_LABEL_STYLE_MAIN,&myFont_style);
    lv_label_set_text(myFont_label," #ff0000 fantastic!# ");
    lv_label_set_body_draw(myFont_label,true);
    lv_obj_align(myFont_label,NULL,LV_ALIGN_CENTER,0,0);
}

void lv_ex_obj_1(void)
{
    lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj1, 100, 50);
    lv_obj_set_style(obj1, &lv_style_plain_color);
    lv_obj_align(obj1, NULL, LV_ALIGN_CENTER, -10, -5);

    /*Copy the previous object and enable drag*/
    lv_obj_t * obj2;
    obj2 = lv_obj_create(lv_scr_act(), obj1);
    lv_obj_set_style(obj2, &lv_style_pretty_color);
    lv_obj_align(obj2, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_drag(obj2, true);

    static lv_style_t style_shadow;
    lv_style_copy(&style_shadow, &lv_style_pretty);
    style_shadow.body.shadow.width = 6;
    style_shadow.body.radius = LV_RADIUS_CIRCLE;

    /*Copy the previous object (drag is already enabled)*/
    lv_obj_t * obj3;
    obj3 = lv_obj_create(lv_scr_act(), obj2);
    lv_obj_set_style(obj3, &style_shadow);
    lv_obj_align(obj3, NULL, LV_ALIGN_CENTER, 10, 5);
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        lv_calendar_date_t * date = lv_calendar_get_pressed_date(obj);
        if(date) {
            lv_calendar_set_today_date(obj, date);
        }
    }
}

void lv_ex_calendar_1(void)
{
    lv_obj_t  * calendar = lv_calendar_create(lv_scr_act(), NULL);
    lv_obj_set_size(calendar,160, 160);
    lv_obj_align(calendar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(calendar, event_handler);

    /*Set the today*/
    lv_calendar_date_t today;
    today.year = 2018;
    today.month = 10;
    today.day = 23;

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);

    /*Highlight some days*/
    static lv_calendar_date_t highlihted_days[3];       /*Only it's pointer will be saved so should be static*/
    highlihted_days[0].year = 2018;
    highlihted_days[0].month = 10;
    highlihted_days[0].day = 6;

    highlihted_days[1].year = 2018;
    highlihted_days[1].month = 10;
    highlihted_days[1].day = 11;

    highlihted_days[2].year = 2018;
    highlihted_days[2].month = 11;
    highlihted_days[2].day = 22;

    lv_calendar_set_highlighted_dates(calendar, highlihted_days, 3);
}

void lv_ex_tileview_1(void)
{
    static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,1}};
    lv_obj_t *tileview;
    tileview = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(tileview, valid_pos, 3);
    lv_tileview_set_edge_flash(tileview, true);

    lv_obj_t * tile1 = lv_obj_create(tileview, NULL);
    lv_obj_set_size(tile1, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style(tile1, &lv_style_pretty);
    lv_tileview_add_element(tileview, tile1);

    /*Tile1: just a label*/
    lv_obj_t * label = lv_label_create(tile1, NULL);
    lv_label_set_text(label, "Tile 1");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Tile2: a list*/
    lv_obj_t * list = lv_list_create(tileview, NULL);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(list, 0, LV_VER_RES);
    lv_list_set_scroll_propagation(list, true);
    lv_list_set_sb_mode(list, LV_SB_MODE_OFF);
    lv_tileview_add_element(tileview, list);

    lv_obj_t * list_btn;
    list_btn = lv_list_add_btn(list, NULL, "One");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Two");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Three");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Four");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Five");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Six");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Seven");
    lv_tileview_add_element(tileview, list_btn);

    list_btn = lv_list_add_btn(list, NULL, "Eight");
    lv_tileview_add_element(tileview, list_btn);

    /*Tile3: a button*/
    lv_obj_t * tile3 = lv_obj_create(tileview, tile1);
    lv_obj_set_pos(tile3, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(tileview, tile3);

    lv_obj_t * btn = lv_btn_create(tile3, NULL);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);

    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Button");
}

#define CANVAS_WIDTH  100
#define CANVAS_HEIGHT  75

void lv_ex_canvas_1(void)
{
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.body.main_color = LV_COLOR_RED;
    style.body.grad_color = LV_COLOR_MAROON;
    style.body.radius = 4;
    style.body.border.width = 2;
    style.body.border.color = LV_COLOR_BLACK;
    style.body.shadow.color = LV_COLOR_BLACK;
    style.body.shadow.width = 4;
    style.line.width = 2;
    style.line.color = LV_COLOR_BLACK;
    style.text.color = LV_COLOR_BLUE;

    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act(), NULL);
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_fill_bg(canvas, LV_COLOR_SILVER);

    lv_canvas_draw_rect(canvas, 35, 30, 50, 35, &style);

    lv_canvas_draw_text(canvas, 20, 10, 50, &style, "Some text", LV_LABEL_ALIGN_LEFT);

    /* Test the rotation. It requires an other buffer where the orignal image is stored.
     * So copy the current image to buffer and rotate it to the canvas */
    lv_color_t cbuf_tmp[CANVAS_WIDTH * CANVAS_HEIGHT];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_WIDTH;
    img.header.h = CANVAS_HEIGHT;

    lv_canvas_fill_bg(canvas, LV_COLOR_SILVER);
    lv_canvas_rotate(canvas, &img, 30, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2);
}

void ready_call_back()
{
	printf("anim\n\r");

}
lv_obj_t * btn1;
lv_anim_t a;
void my_lv_anim1()
{
	
	btn1 = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(btn1, 50, 20);
	lv_obj_set_style(btn1, &lv_style_btn_ina);
	lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -30, 0);


	memset(&a, 0, sizeof(lv_anim_t));

	a.var = btn1;  //动画对象

	a.exec_cb = lv_obj_set_y; // y轴移动
	a.time =   lv_anim_speed_to_time(100,0,100); //1S的动画时间
	a.start = 20;  // 开始坐标
	a.end = 110;   // 结束坐标

	a.act_time = 0;  //第一次运行动画时，延时500ms开始

	a.path_cb = lv_anim_path_ease_out ;//lv_anim_path_bounce; //模拟弹性物体下落动画
	a.ready_cb = ready_call_back;  // 自定义的打印函数

	a.playback = 0;     //不开启动画回放
	a.playback_pause = 2000;

//	a.repeat = 1;       //开始重复动画
//	a.repeat_pause = 200;//每次重复动画时间间隔200ms
	lv_anim_create(&a);  //必须调用的函数,否则无法使用
}
/*��ҳ�洰��*/
static lv_obj_t * appWindow;
/*����ͼƬ*/
static lv_obj_t * imgBg;
//static void ImgBg_Create()
//{
//		appWindow = lv_obj_create(lv_scr_act(), NULL);
//		lv_obj_set_size(appWindow, 160, 80);
//    LV_IMG_DECLARE(ImgBg);
//    imgBg = lv_img_create(appWindow, NULL);
//    lv_img_set_src(imgBg, &ImgBg);
//    lv_obj_align(imgBg, NULL, LV_ALIGN_CENTER, 0, 0);
//}

    
    
    
   
    

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	init();
//	ImgBg_Create();
//	my_lv_anim1();
	DialPlateSetup();
//	lv_ex_canvas_1();
	
	HAL_TIM_Base_Start_IT(&htim10);



	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		size = sprintf((char *)Data, "BMP280 initialization failed\n");
		HAL_UART_Transmit(&huart1, Data, size, 1000);
		HAL_Delay(2000);
	}
	bool bme280p = bmp280.id == BME280_CHIP_ID;
	size = sprintf((char *)Data, "BMP280: found %s\n", bme280p ? "BME280" : "BMP280");
	HAL_UART_Transmit(&huart1, Data, size, 1000);
	
	
//	HAL_UART_Transmit(&huart2, tuya_test_string, sizeof(tuya_test_string), 5000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//		prinf("%s\n\r",bt_uart_rx_buf);
		bt_uart_service();
		lv_task_handler();
		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		if(my_stop){
			my_stop=0;
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,1);
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			module_pwr_enter_stop_mode();
			module_pwr_stop_mode_wake();
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			
//			init();	
//			DialPlateSetup();
		}
		
		
//		if(bme280ready==1){
//			bme280ready=0;
//			bmp280_read_float(&bmp280, &temperature, &pressure, &humidity);
//			
//		}
		if(get_time_flag)
		{
			get_time();
		}
		if(bt_update){
			bt_update=0;
			all_data_update();
		}

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Buzzer_En()
{
	buzzerEN=1;
}
void Buzzer_Clr()
{
	buzzerEN=0;
	HAL_GPIO_WritePin(BUZZER1_EN_GPIO_Port,BUZZER1_EN_Pin,GPIO_PIN_RESET);
}
//Bluetooth low power mode
void LP_Clr()
{
	HAL_GPIO_WritePin(BT7L_LP_GPIO_Port,BT7L_LP_Pin,GPIO_PIN_SET);
}


//extern bool bme280p;
//void BME280_SendMessage(){
//	while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity)) {
//		size = sprintf((char *)Data,
//				"Temperature/pressure reading failed\n");
//		HAL_UART_Transmit(&huart1, Data, size, 1000);
//		HAL_Delay(2000);
//	}

//	size = sprintf((char *)Data,"Pressure: %.2f Pa, Temperature: %.2f C",
//			pressure, temperature);
//	HAL_UART_Transmit(&huart1, Data, size, 1000);
//	if (bme280p) {
//		size = sprintf((char *)Data,", Humidity: %.2f\n", humidity);
//		HAL_UART_Transmit(&huart1, Data, size, 1000);
//	}
//	else {
//		size = sprintf((char *)Data, "\n");
//		HAL_UART_Transmit(&huart1, Data, size, 1000);
//	}
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
