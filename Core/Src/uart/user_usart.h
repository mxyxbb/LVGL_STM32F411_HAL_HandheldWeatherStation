#ifndef _user_usart_H
#define _user_usart_H

#include "stm32f4xx_hal.h"

#define USER_MAIN_DEBUG

#ifdef USER_MAIN_DEBUG
#define user_main_printf(format, ...)  printf( format "\r\n", ##__VA_ARGS__)
#else
#define user_main_printf(format, ...)  
#endif

#endif
