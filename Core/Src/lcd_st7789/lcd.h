#ifndef __LCD_H
#define __LCD_H			  	 
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "main.h"

#define LCD_W 80 //160
#define LCD_H 80

#define	u8 unsigned char
#define	uint16_t unsigned int
#define	u32 unsigned long

//-----------------LCD端口定义----------------


#define LCD_RST_Clr()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin, GPIO_PIN_RESET)//RES
#define LCD_RST_Set()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin, GPIO_PIN_SET)

#define LCD_DC_Clr() HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin, GPIO_PIN_RESET)//DC
#define LCD_DC_Set() HAL_GPIO_WritePin(LCD_DC_GPIO_Port,GPIO_PIN_6, GPIO_PIN_SET)
 		     
#define LCD_BLK_Clr()  HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_RESET)//BLK
#define LCD_BLK_Set()  HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET)

#define LCD_CS_Clr() HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)
#define LCD_CS_Set() HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)


#define LCD_CMD  0	//写命令
#define LCD_DATA 1	//写数据

extern  uint16_t BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void Lcd_Init(void); 
void LCD_Clear(uint16_t Color);
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void LCD_WR_DATA8(char da); //发送数据-8位参数
void LCD_WR_DATA(int da);
void LCD_WR_REG(char da);

void LCD_DrawPoint(uint16_t x,uint16_t y);//画点
void LCD_DrawPoint_big(uint16_t x,uint16_t y);//画一个大点
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y); //读点
void Draw_Circle(uint16_t x0,uint16_t y0,u8 r);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,u8 num,u8 mode);//显示一个字符
void LCD_ShowNum(uint16_t x,uint16_t y,u32 num,u8 len);//显示数字
void LCD_Show2Num(uint16_t x,uint16_t y,uint16_t num,u8 len);//显示2个数字
void LCD_ShowString(uint16_t x,uint16_t y,const u8 *p);		 //显示一个字符串,16字体
 void LCD_Clear_DMA(uint16_t Color);
void showhanzi(unsigned int x,unsigned int y,unsigned char index);
void LCD_ShowImg(uint8_t *img);

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430
#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
#define LIGHTGREEN     	 0X841F
#define LGRAY 			 0XC618
#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12


					  		 
#endif  
	 
	 



