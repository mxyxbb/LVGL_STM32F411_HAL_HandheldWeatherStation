/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**

**--------------版本修订记录---------------------------------------------------
** 版  本: v1.0
** 日　期: 2017年5月3日
** 描　述: 1:创建涂鸦bluetooth对接MCU_SDK
**

** 版  本:v2.0
** 日　期: 2020年3月23日
** 描　述: 
1.	增加cmd 0x09模块解绑接口支持
2.	增加cmd 0x0e rf射频测试接口支持
3.	增加cmd 0xe0 记录型数据上报接口支持
4.	增加cmd 0xE1 获取实时时间接口支持
5.	增加 cmd 0xe2 修改休眠模式状态广播间隔支持
6.	增加 cmd 0xe4 关闭系统时钟功能支持
7.	增加 cmd 0xe5 低功耗使能支持
8.	增加 cmd 0xe6 获取一次性动态密码支持
9.	增加 cmd 0xe7断开蓝牙连接支持
10.	增加 cmd 0xe8 查询MCU版本号支持
11.	增加 cmd 0xe9 MCU主动发送版本号支持
12.	增加 cmd 0xea OTA升级请求支持
13.	增加 cmd 0xeb OTA升级文件信息支持
14.	增加 cmd 0xec OTA升级文件偏移请求支持
15.	增加 cmd 0xed OTA升级数据支持
16.	增加 cmd 0xee OTA升级结束支持
17.	增加 cmd 0xa0 MCU 获取模块版本信息支持
18.	增加 cmd 0xa1 恢复出厂设置通知支持
19.  增加MCU OTA demo
20. 优化串口解析器
**
**-----------------------------------------------------------------------------
******************************************************************************/

#define MCU_API_GLOBAL

#include "bluetooth.h"

#include <ctype.h>

/*****************************************************************************
函数名称 : hex_to_bcd
功能描述 : hex转bcd
输入参数 : Value_H:高字节/Value_L:低字节
返回参数 : bcd_value:转换完成后数据
*****************************************************************************/
unsigned char hex_to_bcd(unsigned char Value_H,unsigned char Value_L)
{
  unsigned char bcd_value;
  
  if((Value_H >= '0') && (Value_H <= '9'))
    Value_H -= '0';
  else if((Value_H >= 'A') && (Value_H <= 'F'))
    Value_H = Value_H - 'A' + 10;
  else if((Value_H >= 'a') && (Value_H <= 'f'))
    Value_H = Value_H - 'a' + 10;
   
  bcd_value = Value_H & 0x0f;
  
  bcd_value <<= 4;
  if((Value_L >= '0') && (Value_L <= '9'))
    Value_L -= '0';
  else if((Value_L >= 'A') && (Value_L <= 'F'))
    Value_L = Value_L - 'a' + 10;
  else if((Value_L >= 'a') && (Value_L <= 'f'))
    Value_L = Value_L - 'a' + 10;
  
  bcd_value |= Value_L & 0x0f;

  return bcd_value;
}
/*****************************************************************************
函数名称 : my_strlen
功能描述 : 求字符串长度
输入参数 : src:源地址
返回参数 : len:数据长度
*****************************************************************************/
unsigned long my_strlen(unsigned char *str)  
{
  unsigned long len = 0;
  if(str == NULL)
  { 
    return 0;
  }
  
  for(len = 0; *str ++ != '\0'; )
  {
    len ++;
  }
  
  return len;
}
/*****************************************************************************
函数名称 : my_memset
功能描述 : 把src所指内存区域的前count个字节设置成字符c
输入参数 : src:源地址
           ch:设置字符
           count:设置数据长度
返回参数 : src:数据处理完后的源地址
*****************************************************************************/
void *my_memset(void *src,unsigned char ch,unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  
  if(src == NULL)
  {
    return NULL;
  }
  
  while(count --)
  {
    *tmp ++ = ch;
  }
  
  return src;
}
/*****************************************************************************
函数名称 : mymemcpy
功能描述 : 内存拷贝
输入参数 : dest:目标地址
           src:源地址
           count:数据拷贝数量
返回参数 : src:数据处理完后的源地址
*****************************************************************************/
void *my_memcpy(void *dest, const void *src, unsigned short count)  
{  
  unsigned char *pdest = (unsigned char *)dest;  
  const unsigned char *psrc  = (const unsigned char *)src;  
  unsigned short i;
  
  if(dest == NULL || src == NULL)
  { 
    return NULL;
  }
  
  if((pdest <= psrc) || (pdest > psrc + count))
  {  
    for(i = 0; i < count; i ++)
    {  
      pdest[i] = psrc[i];  
    }  
  }
  else
  {
    for(i = count; i > 0; i --)
    {  
      pdest[i - 1] = psrc[i - 1];  
    }  
  }  
  
  return dest;  
}
/*****************************************************************************
函数名称 : memcmp
功能描述 : 内存比较
输入参数 : buffer1:内存1
           buffer2:内存2
           	count:比较长度
返回参数 : 大小比较值，0:buffer1=buffer2; -1:buffer1<buffer2; 1:buffer1>buffer2
*****************************************************************************/
int my_memcmp(const void *buffer1,const void *buffer2,int count)
{
   if (!count)
      return(0);
   while ( --count && *(char *)buffer1 == *(char *)buffer2)
   {
      buffer1 = (char *)buffer1 + 1;
        buffer2 = (char *)buffer2 + 1;
   }
   return( *((unsigned char *)buffer1) - *((unsigned char *)buffer2) );
}
/*****************************************************************************
函数名称 : atoll
功能描述 : 字符串转整数
输入参数 : p 字符串
返回参数 : 整数
*****************************************************************************/
long long my_atoll(const char *p)
{
	long long n;
	int c, neg = 0;
	unsigned char   *up = (unsigned char *)p;

	if (!isdigit(c = *up)) {
		while (isspace(c))
			c = *++up;
		switch (c) {
		case '-':
			neg++;
			/* FALLTHROUGH */
		case '+':
			c = *++up;
		}
		if (!isdigit(c))
			return (0);
	}

	for (n = '0' - c; isdigit(c = *++up); ) {
		n *= 10; /* two steps to avoid unnecessary overflow */
		n += '0' - c; /* accum neg to avoid surprises at MAX */
	}

	return (neg ? n : -n);
}


/*****************************************************************************
函数名称 : int_to_byte
功能描述 : 将int类型拆分四个字节
输入参数 : number:4字节原数据;value:处理完成后4字节数据
返回参数 :无
****************************************************************************/
void int_to_byte(unsigned long number,unsigned char value[4])
{
  value[0] = number >> 24;
  value[1] = number >> 16;
  value[2] = number >> 8;
  value[3] = number & 0xff;
}
/*****************************************************************************
函数名称 : byte_to_int
功能描述 : 将4字节合并为1个32bit变量
输入参数 : value:4字节数组
返回参数 : number:合并完成后的32bit变量
****************************************************************************/
unsigned long byte_to_int(const unsigned char value[4])
{
  unsigned long nubmer = 0;

  nubmer = (unsigned long)value[0];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[1];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[2];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[3];
  
  return nubmer;
}
#ifndef BT_CONTROL_SELF_MODE
/*****************************************************************************
函数名称 : mcu_get_reset_bt_flag
功能描述 : MCU获取复位bt成功标志
输入参数 : 无
返回参数 : 复位标志:RESET_BT_ERROR:失败/RESET_BT_SUCCESS:成功
使用说明 : 1:MCU主动调用mcu_reset_bt()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char mcu_get_reset_bt_flag(void)
{
  return reset_bt_flag;
}
/*****************************************************************************
函数名称 : mcu_reset_bt
功能描述 : MCU主动重置bt工作模式
输入参数 : 无
返回参数 : 无
使用说明 : 1:MCU主动调用,通过mcu_get_reset_bt_flag()函数获取重置bt是否成功
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void mcu_reset_bt(void)
{
  reset_bt_flag = RESET_BT_ERROR;
  
  bt_uart_write_frame(BT_RESET_CMD, 0);
}
/*****************************************************************************
函数名称 : mcu_get_bt_work_state
功能描述 : MCU主动获取当前bt工作状态
输入参数 : 无
返回参数 : BT_WORK_SATE_E:
            BT_UN_BIND:蓝牙未绑定
            BT_UNCONNECT:蓝牙已绑定未连接
            BT_CONNECTED:蓝牙已绑定并已连接
使用说明 : 无
*****************************************************************************/
unsigned char mcu_get_bt_work_state(void)
{
  return bt_work_state;
}
#endif

/*****************************************************************************
函数名称 : mcu_dp_raw_update
功能描述 : raw型dp数据上传
输入参数 : dpid:id号
           value:当前dp值指针
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_raw_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  //
  length = set_bt_uart_byte(length,dpid);
  length = set_bt_uart_byte(length,DP_TYPE_RAW);
  //
  length = set_bt_uart_byte(length,len / 0x100);
  length = set_bt_uart_byte(length,len % 0x100);
  //
  length = set_bt_uart_buffer(length,(unsigned char *)value,len);
  
  bt_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_bool_update
功能描述 : bool型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_bool_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_bt_uart_byte(length,dpid);
  length = set_bt_uart_byte(length,DP_TYPE_BOOL);
  //
  length = set_bt_uart_byte(length,0);
  length = set_bt_uart_byte(length,1);
  //
  if(value == FALSE)
  {
    length = set_bt_uart_byte(length,FALSE);
  }
  else
  {
    length = set_bt_uart_byte(length,1);
  }
  
  bt_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_value_update
功能描述 : value型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_bt_uart_byte(length,dpid);
  length = set_bt_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_bt_uart_byte(length,0);
  length = set_bt_uart_byte(length,4);
  //
  length = set_bt_uart_byte(length,value >> 24);
  length = set_bt_uart_byte(length,value >> 16);
  length = set_bt_uart_byte(length,value >> 8);
  length = set_bt_uart_byte(length,value & 0xff);
  
  bt_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_string_update
功能描述 : rstring型dp数据上传
输入参数 : dpid:id号
           value:当前dp值指针
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  //
  length = set_bt_uart_byte(length,dpid);
  length = set_bt_uart_byte(length,DP_TYPE_STRING);
  //
  length = set_bt_uart_byte(length,len / 0x100);
  length = set_bt_uart_byte(length,len % 0x100);
  //
  length = set_bt_uart_buffer(length,(unsigned char *)value,len);
  
  bt_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_enum_update
功能描述 : enum型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_enum_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_bt_uart_byte(length,dpid);
  length = set_bt_uart_byte(length,DP_TYPE_ENUM);
  //
  length = set_bt_uart_byte(length,0);
  length = set_bt_uart_byte(length,1);
  //
  length = set_bt_uart_byte(length,value);
  
  bt_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_fault_update
功能描述 : fault型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_fault_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
   
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_bt_uart_byte(length,dpid);
  length = set_bt_uart_byte(length,DP_TYPE_BITMAP);
  //
  length = set_bt_uart_byte(length,0);
  
  if((value | 0xff) == 0xff)
  {
    length = set_bt_uart_byte(length,1);
    length = set_bt_uart_byte(length,value);
  }
  else if((value | 0xffff) == 0xffff)
  {
    length = set_bt_uart_byte(length,2);
    length = set_bt_uart_byte(length,value >> 8);
    length = set_bt_uart_byte(length,value & 0xff);
  }
  else
  {
    length = set_bt_uart_byte(length,4);
    length = set_bt_uart_byte(length,value >> 24);
    length = set_bt_uart_byte(length,value >> 16);
    length = set_bt_uart_byte(length,value >> 8);
    length = set_bt_uart_byte(length,value & 0xff);
  }    
  
  bt_uart_write_frame(STATE_UPLOAD_CMD,length);

  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_get_dp_download_bool
功能描述 : mcu获取bool型下发dp值
输入参数 : value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : bool:当前dp值
*****************************************************************************/
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}
/*****************************************************************************
函数名称 : mcu_get_dp_download_enum
功能描述 : mcu获取enum型下发dp值
输入参数 : value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : enum:当前dp值
*****************************************************************************/
unsigned char mcu_get_dp_download_enum(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}
/*****************************************************************************
函数名称 : mcu_get_dp_download_value
功能描述 : mcu获取value型下发dp值
输入参数 : value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : value:当前dp值
*****************************************************************************/
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
  return(byte_to_int(value));
}
/*****************************************************************************
函数名称 : uart_receive_input
功能描述 : 收数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 在MCU串口接收函数中调用该函数,并将接收到的数据作为参数传入
*****************************************************************************/
void uart_receive_input(unsigned char value)
{
  if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(bt_queue_buf)))
  {
    //数据队列满
  }
  else if((queue_in < queue_out) && ((queue_out  - queue_in) == 0))
  {
    //数据队列满
  }
  else
  {
    //队列不满
    if(queue_in >= (unsigned char *)(bt_queue_buf + sizeof(bt_queue_buf)))
    {
      queue_in = (unsigned char *)(bt_queue_buf);
    }
    
    *queue_in ++ = value;
  }
}
/*
 *@brief Function for receive uart data.
 *@param
 *
 *@note
 *
 * */

typedef enum {
    MCU_UART_REV_STATE_FOUND_NULL,
    MCU_UART_REV_STATE_FOUND_HEAD,
    MCU_UART_REV_STATE_FOUND_CMD,
    MCU_UART_REV_STATE_FOUND_LEN_H,
    MCU_UART_REV_STATE_FOUND_LEN_L,
    MCU_UART_REV_STATE_FOUND_DATA,
    MCU_UART_REV_STATE_UNKOWN,
} mcu_uart_rev_state_type_t;

#define UART_RX_BUFFER_MAX   (PROTOCOL_HEAD + BT_UART_RECV_BUF_LMT)
#define UART_RX_DATA_LEN_MAX (PROTOCOL_HEAD + BT_UART_RECV_BUF_LMT)
static volatile mcu_uart_rev_state_type_t current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_NULL;
static uint8_t bt_uart_rx_buf_temp[3] = {0};
static uint16_t uart_data_len =  0;
static volatile uint16_t UART_RX_Count = 0;


static bool mcu_common_uart_data_unpack(uint8_t data)
{
    bool ret = false;

    bt_uart_rx_buf_temp[0] = bt_uart_rx_buf_temp[1];
    bt_uart_rx_buf_temp[1] = bt_uart_rx_buf_temp[2];
    bt_uart_rx_buf_temp[2] = data;

    if((bt_uart_rx_buf_temp[0]==0x55)&&(bt_uart_rx_buf_temp[1]==0xAA)&&(bt_uart_rx_buf_temp[2]==0x00))
    {
        my_memset(bt_uart_rx_buf,0,sizeof(bt_uart_rx_buf));
        my_memcpy(bt_uart_rx_buf,bt_uart_rx_buf_temp,3);
        my_memset(bt_uart_rx_buf_temp,0,3);
        UART_RX_Count = 3;
        current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_HEAD;
        uart_data_len = 0;
        return ret;
    }
    switch(current_uart_rev_state_type)
    {
    case MCU_UART_REV_STATE_FOUND_NULL:
        break;
    case MCU_UART_REV_STATE_FOUND_HEAD:
        bt_uart_rx_buf[UART_RX_Count++] = data;
        current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_CMD;
        break;
    case MCU_UART_REV_STATE_FOUND_CMD:
        bt_uart_rx_buf[UART_RX_Count++] = data;
        current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_LEN_H;
        break;
    case MCU_UART_REV_STATE_FOUND_LEN_H:
        bt_uart_rx_buf[UART_RX_Count++] = data;
        uart_data_len = (bt_uart_rx_buf[UART_RX_Count-2]<<8)|bt_uart_rx_buf[UART_RX_Count-1];
        if(uart_data_len>UART_RX_DATA_LEN_MAX)
        {
            my_memset(bt_uart_rx_buf_temp,0,3);
            my_memset(bt_uart_rx_buf,0,sizeof(bt_uart_rx_buf));
            UART_RX_Count = 0;
            current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_NULL;
            uart_data_len = 0;
        }
        else if(uart_data_len>0)
        {
            current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_LEN_L;
        }
        else
        {
            current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_DATA;
        }
        break;
    case MCU_UART_REV_STATE_FOUND_LEN_L:
        bt_uart_rx_buf[UART_RX_Count++] = data;   //DATA
        uart_data_len--;
        if(uart_data_len==0)
        {
            current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_DATA;
        }
        break;
    case MCU_UART_REV_STATE_FOUND_DATA:
        bt_uart_rx_buf[UART_RX_Count++] = data;  //sum data
        ret = true;
        break;
    default:
        my_memset(bt_uart_rx_buf_temp,0,3);
        my_memset(bt_uart_rx_buf,0,sizeof(bt_uart_rx_buf));
        UART_RX_Count = 0;
        current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_NULL;
        uart_data_len = 0;
        break;
    };

    return ret;

}

/*****************************************************************************
函数名称  : bt_uart_service
功能描述  : bt串口处理服务
输入参数 : 无
返回参数 : 无
使用说明 : 在MCU主函数while循环中调用该函数
*****************************************************************************/
void bt_uart_service(void)
{
  static unsigned short rx_in = 0;
  unsigned short offset = 0;
  unsigned short rx_value_len = 0;             //数据帧长度
  
  if((rx_in < sizeof(bt_uart_rx_buf)) && get_queue_total_data() > 0)
  {
    if(mcu_common_uart_data_unpack(Queue_Read_Byte()))
    {
		data_handle(0);
		rx_value_len = bt_uart_rx_buf[LENGTH_HIGH] * 0x100 + bt_uart_rx_buf[LENGTH_LOW] + PROTOCOL_HEAD;
		my_memset(bt_uart_rx_buf_temp,0,3);
        my_memset(bt_uart_rx_buf,0,sizeof(bt_uart_rx_buf));
        UART_RX_Count = 0;
        current_uart_rev_state_type = MCU_UART_REV_STATE_FOUND_NULL;
        uart_data_len = 0;
    }
  }

}
/*****************************************************************************
函数名称 :  bt_protocol_init
功能描述 : 协议串口初始化函数
输入参数 : 无
返回参数 : 无
使用说明 : 必须在MCU初始化代码中调用该函数
*****************************************************************************/
void bt_protocol_init(void)
{
  queue_in = (unsigned char *)bt_queue_buf;
  queue_out = (unsigned char *)bt_queue_buf;
  //
#ifndef BT_CONTROL_SELF_MODE
  bt_work_state = BT_SATE_UNKNOW;
#endif
	mcu_ota_init();

}

