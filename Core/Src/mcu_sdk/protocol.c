/*************************************    ***Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: protocol.c
**描        述: 下发/上报数据处理函数
**使 用 说 明 :

                  *******非常重要，一定要看哦！！！********

** 1、用户在此文件中实现数据下发/上报功能
** 2、DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
** 3、当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
**
**--------------版本修订记录---------------------------------------------------

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

******************************************************************************/

#include "bluetooth.h"
#include "usart.h"
  

/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的bt_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

         
/******************************************************************************
                              第一步:初始化
1:在需要使用到bt相关文件的文件中include "bt.h"
2:在MCU初始化中调用mcu_api.c文件中的bt_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的bt_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
const DOWNLOAD_CMD_S download_cmd[] =
{
  {DPID_TEMP_CURRENT, DP_TYPE_VALUE},
  {DPID_HUMIDITY_VALUE, DP_TYPE_VALUE},
  {DPID_BATTERY_PERCENTAGE, DP_TYPE_VALUE},
  {DPID_CLOCK_SET, DP_TYPE_RAW},
};




/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

/*****************************************************************************
函数名称 : uart_transmit_output
功能描述 : 发数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
*****************************************************************************/
void uart_transmit_output(unsigned char value)
{
 HAL_UART_Transmit(&huart2, (uint8_t *)&value, 1, 0xFFFF); 
/*
  //示例:
  extern void Uart_PutChar(unsigned char value);
  Uart_PutChar(value);	                                //串口发送函数
*/  
}
/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

//自动化生成数据上报函数

/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上传,实现APP和muc数据同步
输入参数 : 无
返回参数 : 无
使用说明 : 此函数SDK内部需调用;
           MCU必须实现该函数内数据上报功能;包括只上报和可上报可下发型数据
*****************************************************************************/
extern float pressure, temperature, humidity;
extern uint8_t clockTime;
void all_data_update(void)
{
//  #error "请在此处理可下发可上报数据及只上报数据示例,处理完成后删除该行"
  //此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数
    mcu_dp_value_update(DPID_TEMP_CURRENT,(unsigned long)(temperature*10)); //VALUE型数据上报;
    mcu_dp_value_update(DPID_HUMIDITY_VALUE,humidity); //VALUE型数据上报;
    mcu_dp_value_update(DPID_BATTERY_PERCENTAGE,100); //VALUE型数据上报;
    mcu_dp_raw_update(DPID_CLOCK_SET,&clockTime,sizeof(clockTime)); //RAW型数据上报;



}


/******************************************************************************
                                WARNING!!!    
                            2:所有数据上报处理
自动化代码模板函数,具体请用户自行实现数据处理
******************************************************************************/

/*****************************************************************************
函数名称 : dp_download_clock_set_handle
功能描述 : 针对DPID_CLOCK_SET的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_clock_set_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为RAW
    unsigned char ret;
    /*
    //RAW类型数据处理
    
    */
    
    //处理完DP数据后应有反馈
    ret = mcu_dp_raw_update(DPID_CLOCK_SET,value,length);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}


/******************************************************************************
                                WARNING!!!                     
此代码为SDK内部调用,请按照实际dp数据实现函数内部数据
******************************************************************************/

/******************************************************************************
                                WARNING!!!                     
以下函数用户请勿修改!!
******************************************************************************/

/*****************************************************************************
函数名称 : dp_download_handle
功能描述 : dp下发处理函数
输入参数 : dpid:DP序号
           value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERRO
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下发/可上报数据调用                    
  具体函数内需要实现下发数据处理
  完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
  ***********************************/
  unsigned char ret;
  switch(dpid)
  {
        case DPID_CLOCK_SET:
            //闹钟设置处理函数
            ret = dp_download_clock_set_handle(value,length);
        break;


  default:
    break;
  }
  return ret;
}
/*****************************************************************************
函数名称 : get_download_cmd_total
功能描述 : 获取所有dp命令总和
输入参数 : 无
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}

//////////////////////////////////当前MCU SDK版本较上一版本新增支持协议接口////////////////////
#ifdef TUYA_BCI_UART_COMMON_UNBOUND_REQ 
/*****************************************************************************
函数名称 :  bt_unbound_req
功能描述 : 向模块发送解绑请求，模块收到该指令后会解除蓝牙绑定关系
输入参数 : 无
返回参数 : 无
使用说明 : MCU主动解绑调用
*****************************************************************************/
void bt_unbound_req(void)
{
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_UNBOUND_REQ,0);
}
#endif

#ifdef TUYA_BCI_UART_COMMON_RF_TEST 
/*****************************************************************************
函数名称 :  bt_rf_test_req
功能描述 : 向模块发射频测试请求
输入参数 : 无
返回参数 : 无
使用说明 : 
*****************************************************************************/
void bt_rf_test_req(void)
{
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_RF_TEST,0);
}
/*****************************************************************************
函数名称 : bt_rf_test_result
功能描述 : 蓝牙rf测试反馈
输入参数 : result:蓝牙rf测试结果;0:失败/1:成功
           rssi:测试成功表示蓝牙信号强度/测试失败值无意义
返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_rf_test_result(unsigned char result,signed char rssi)
{
//  #error "请自行完善该功能,完成后请删除该行"
  if(result == 0)
  {
		printf("There is sth wrong in bt_rf_test_result");
    //测试失败
  }
  else
  {
    //测试成功
		printf("bt_rf_test_result OK!");
    //rssi为信号强度，一般大于-70dbm 蓝牙信号都在正常范围内
  }
  
}
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE 
/*****************************************************************************
函数名称 : bt_send_recordable_dp_data
功能描述 : 记录型数据上报
输入参数 : type -1： 蓝牙模块自带时间上报 -2： 只上报原始数据，无时间-3： MCU自带时间上报
		dpid:  datapoint 序号
		dptype:对应开放平台上某datapoint 具体的数据类型
		value:具体数据
		len:数据的长度
返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
	建议使用缓存队列，将要发给模块的所有数据放到MCU缓存队列中，上报成功一条后再上报下一条，记录型数据要确保每条数据都上报成功
*****************************************************************************/
void bt_send_recordable_dp_data(unsigned char snedType,unsigned char dpid,unsigned char dpType, unsigned char value[],unsigned short len)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(snedType==0x01)//格式1，蓝牙模块自带时间上报
	{

	}
	else if(snedType==0x02)//格式2，只上报原始数据，无时间  （备注:telink对接平台不支持该格式）
	{

	}
	else if(snedType==0x03)//格式3，MCU自带时间上报
	{

	}
}
/*****************************************************************************
函数名称 : bt_send_recordable_dp_data_result
功能描述 : 记录型数据上报
输入参数 : result ：0存储成功，1存储失败
返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_send_recordable_dp_data_result(unsigned char result)
{
//	#error "请自行完善该功能,完成后请删除该行"
}
#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE 
/*****************************************************************************
函数名称 : bt_send_time_sync_type
功能描述 : 向模块发送时间同步请求
输入参数 :sync_time_type
0x00- 获取7字节时间时间类型+2
字节时区信息
0x01- 获取13字节ms级unix时间
+2字节时区信息
0x02- 获取7字节时间时间类型+2
字节时区信息

返回参数 : 无
使用说明 :
*****************************************************************************/
void bt_send_time_sync_req(unsigned char sync_time_type)
{
	unsigned short length = 0;
  
  	length = set_bt_uart_byte(length,sync_time_type);
  	
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE,length);
	
}
/*****************************************************************************
函数名称 : bt_time_sync_result
功能描述 : 向模块发送时间同步的结果
输入参数 :result同步结果 0成功，其他失败
		sync_time_type时间格式
		bt_time自定义时间（如果是时间格式0或者1该值有效）
		time_zone_100 时区
		time_stamp_ms 时间戳（如果是时间格式1，该值有效）
返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_time_sync_result(unsigned char result,unsigned char sync_time_type,bt_time_struct_data_t bt_time,unsigned short time_zone_100,long long time_stamp_ms)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(result == 0x00)
	{
		//同步时间成功
		if(sync_time_type==0x00||sync_time_type==0x02)
		{
			//将bt_time中的自定义时间格式的数据填充到mcu时钟系统中
			//time_zone_100时区
		}
		else if(sync_time_type==0x01)
		{
			//将time_stamp_ms中的时间戳填充到mcu时钟系统中
			//time_zone_100时区
		}
	}
	else
	{
		//同步时间失败
	}
}
#endif
#endif

#ifdef TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL
/*****************************************************************************
函数名称 : bt_modify_adv_interval_req
功能描述 : 向模块发送修改低功耗下模块广播间隔的请求
输入参数 :value * 100ms等于要修改的广播间隔，value（0-20）
返回参数 : 无
使用说明 : 
*****************************************************************************/
void bt_modify_adv_interval_req(unsigned char value)
{
	unsigned short length = 0;
  
  	length = set_bt_uart_byte(length,value);
  	
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL,length);
}
/*****************************************************************************
函数名称 : bt_modify_adv_interval_result
功能描述 :向模块发送修改低功耗下模块广播间隔的结果
输入参数 :result同步结果 0成功，其他失败

返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_modify_adv_interval_result(unsigned char result)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(result == 0x00)
	{
		//成功

	}
	else
	{
		//失败
	}
}
#endif

#ifdef TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME
/*****************************************************************************
函数名称 : bt_close_timer_req
功能描述 : 向模块发送关闭系统时钟的请求（目前仅适用telink平台）
输入参数 :value 0关闭，1打开
返回参数 : 无
使用说明 : 
*****************************************************************************/
void bt_close_timer_req(unsigned char value)
{
	unsigned short length = 0;
  
  	length = set_bt_uart_byte(length,value);
  	
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME,length);
}
/*****************************************************************************
函数名称 : bt_close_timer_result
功能描述 :向模块发送关闭系统时钟的结果
输入参数 :result同步结果 0成功，其他失败

返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_close_timer_result(unsigned char result)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(result == 0x00)
	{
		//成功

	}
	else
	{
		//失败
	}
}
#endif

#ifdef TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER
/*****************************************************************************
函数名称 : bt_enable_lowpoer_req
功能描述 : 向模块发送使能低功耗的请求（目前仅适用telink平台）
输入参数 :value 0关闭，1打开
返回参数 : 无
使用说明 :
*****************************************************************************/
void bt_enable_lowpoer_req(unsigned char value)
{
	unsigned short length = 0;
  
  	length = set_bt_uart_byte(length,value);
  	
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER,length);
}
/*****************************************************************************
函数名称 : bt_enable_lowpoer_result
功能描述 :向模块发送使能低功耗的结果
输入参数 :result同步结果 0成功，其他失败

返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_enable_lowpoer_result(unsigned char result)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(result == 0x00)
	{
		//成功

	}
	else
	{
		//失败
	}
}
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN
/*****************************************************************************
函数名称 : bt_send_one_time_password_token
功能描述 : 向模块发送获取一次性动态密码匹配结果的请求
输入参数 :value cmcu端输入的动态密码，len 8
返回参数 : 无
使用说明 :用于锁通用串口对接动态密码功能
*****************************************************************************/
unsigned char bt_send_one_time_password_token(unsigned char value[],unsigned char len)
{
	unsigned short length = 0;
 	if(len!=8)return 0;
 	
  	length = set_bt_uart_buffer(length,value,8);
  	
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN,length);
}
/*****************************************************************************
函数名称 : bt_send_one_time_password_token_result
功能描述 :向模块获取一次性动态密码匹配的结果
输入参数 :result同步结果 0x00密码核对通过，0x01密码核对失败

返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_send_one_time_password_token_result(unsigned char result)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(result == 0x00)
	{
		//密码核对通过

	}
	else
	{
		//密码核对失败
	}
}
#endif

#ifdef TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT
/*****************************************************************************
函数名称 : bt_enable_lowpoer_req
功能描述 : 向模块发送断开蓝牙连接的请求
输入参数 :value 0关闭，1打开
返回参数 : 无
使用说明 :
*****************************************************************************/
void bt_disconnect_req(void)
{
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT,0);
}
/*****************************************************************************
函数名称 : bt_enable_lowpoer_result
功能描述 :向模块发送断开蓝牙连接的结果
输入参数 :result结果 0成功，其他失败

返回参数 : 无
使用说明 : MCU需要自行完善该函数功能
*****************************************************************************/
void bt_disconnect_result(unsigned char result)
{
//	#error "请自行完善该功能,完成后请删除该行"
	if(result == 0x00)
	{
		//成功

	}
	else
	{
		//失败
	}
}
#endif

#ifdef TUYA_BCI_UART_COMMON_MCU_SEND_VERSION
/*****************************************************************************
函数名称 : bt_send_mcu_ver
功能描述 :向模块主动发送MCU版本号，主要是为了模块能更及时获取到MCU的版本信息
输入参数 :

返回参数 : 无
使用说明 : MCU可以在串口初始化后调用一次
*****************************************************************************/
void bt_send_mcu_ver(void)
{
	unsigned short length = 0;
	length = set_bt_uart_buffer(length,(unsigned char *)MCU_APP_VER_NUM,3);
	length = set_bt_uart_buffer(length,(unsigned char *)MCU_HARD_VER_NUM,3);
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_SEND_VERSION,length);
}
#endif
#ifdef TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY
/*****************************************************************************
函数名称 : bt_factor_reset_notify
功能描述 :模块恢复出厂设置后向mcu发送的通知
输入参数 :

返回参数 : 无
使用说明 : MCU可以在此处完成MCU恢复出厂设置的操作
*****************************************************************************/
void bt_factor_reset_notify(void)
{
//	#error "请自行完善该功能,完成后请删除该行"
}
#endif

