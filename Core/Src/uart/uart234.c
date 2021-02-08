#include "uart/uart234.h"
#include "uart/user_usart.h"
#include "usart.h"
//#include "Max7219.h"
#include <stdio.h>
//#include "ssd1306.h"
#include "i2c.h"
#include "bluetooth.h"

extern uint8_t bRxBuffer;
uint8_t myBuffer[] = "I have gotten your message: ";
uint8_t countOfGetBuffer=0;
uint8_t getBuffer[10];
uint8_t Enter[] = "\r\n";

void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
	if(UartHandle==&huart2)
	{
		uart_receive_input(bRxBuffer);
//	printf("%s",&bRxBuffer);
// getBuffer[countOfGetBuffer++] = bRxBuffer;
// if(countOfGetBuffer == 10)
// {
//	while(HAL_UART_Transmit(&huart2, (uint8_t*)myBuffer, COUNTOF(myBuffer), 5000)!= HAL_OK);
//	while(HAL_UART_Transmit(&huart2, (uint8_t*)getBuffer, countOfGetBuffer, 5000)!= HAL_OK);
//	while(HAL_UART_Transmit(&huart2, (uint8_t*)Enter, COUNTOF(Enter), 5000)!= HAL_OK);
//	countOfGetBuffer = 0;
// }
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&bRxBuffer,1);  //由于接收中断是每接收一个字符便进入一次，所以这一行代码必须添加，否则只能接收一个字符，而无法接收整个字符串
	}
}
