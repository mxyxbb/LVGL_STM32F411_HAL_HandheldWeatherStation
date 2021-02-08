/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: system.c
**描        述: bluetooth数据处理函数
**使 用 说 明 : 用户无需关心该文件实现内容
**
**
**--------------历史版本修订---------------------------------------------------
** 版  本: v1.0
** 日　期: 2017年5月3日
** 描　述: 1:创建涂鸦bluetooth对接MCU_SDK
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
#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#ifdef SYSTEM_GLOBAL
  #define SYSTEM_EXTERN
#else
  #define SYSTEM_EXTERN   extern
#endif

//=============================================================================
//帧的字节顺序
//=============================================================================
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1        
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6

//=============================================================================
//数据帧类型
//=============================================================================
#define         HEAT_BEAT_CMD                   0                               //心跳包
#define         PRODUCT_INFO_CMD                1                               //产品信息
#define         WORK_MODE_CMD                   2                               //查询MCU 设定的模块工作模式	
#define         BT_STATE_CMD                    3                               //bluetooth工作状态	
#define         BT_RESET_CMD                    4                               //重置bluetooth
#define         DATA_QUERT_CMD                  6                               //命令下发
#define         STATE_UPLOAD_CMD                7                               //状态上报	 
#define         STATE_QUERY_CMD                 8                               //状态查询   

//////////////////////////////////当前MCU SDK版本较上一版本新增支持协议接口////////////////////
//如果不需要某一条命令接口，直接注释掉命令宏的定义，相关代码将不会被编译，以减小代码空间
#define TUYA_BCI_UART_COMMON_UNBOUND_REQ					0x09				//模块解绑
#define TUYA_BCI_UART_COMMON_RF_TEST	            	    0x0E				//rf射频测试
#define TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE              0xE0				//记录型数据上报（离线缓存）
#define TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE            0xE1				//获取实时时间
#define TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL		    0xE2				//修改休眠模式广播间隔
#define TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME		    0xE4				//关闭系统时钟功能
#define TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER		    0xE5				//低功耗使能
#define TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN 	0xE6				//获取一次性动态密码匹配结果
#define TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT			    0xE7				//断开蓝牙连接

#define TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION			    0xE8				//查询MCU版本号
#define TUYA_BCI_UART_COMMON_MCU_SEND_VERSION			    0xE9				//MCU主动发送版本号

#define TUYA_BCI_UART_COMMON_MCU_OTA_REQUEST			    0xEA				//OTA升级请求
#define TUYA_BCI_UART_COMMON_MCU_OTA_FILE_INFO			    0xEB				//OTA升级文件信息
#define TUYA_BCI_UART_COMMON_MCU_OTA_FILE_OFFSET	        0xEC				//OTA升级文件偏移请求
#define TUYA_BCI_UART_COMMON_MCU_OTA_DATA 			        0xED				//OTA升级数据
#define TUYA_BCI_UART_COMMON_MCU_OTA_END			        0xEE				//OTA升级结束


//#define TUYA_BCI_UART_COMMON_QUERY_MOUDLE_VERSION			0xA0				//MCU 获取模块版本信息

#define TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY			0xA1				//恢复出厂设置通知


#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE 
/* 自定义的时间结构体 */
typedef struct
{
    unsigned short nYear;
    unsigned char nMonth;
    unsigned char nDay;
    unsigned char nHour;
    unsigned char nMin;
    unsigned char nSec;
    unsigned char DayIndex; /* 0 = Sunday */
} bt_time_struct_data_t;
#endif





//=============================================================================
#define         VERSION                 0x00                                            //协议版本号
#define         PROTOCOL_HEAD           0x07                                            //固定协议头长度
#define         FIRM_UPDATA_SIZE        256                                            //升级包大小
#define         FRAME_FIRST             0x55
#define         FRAME_SECOND            0xaa
//============================================================================= 
SYSTEM_EXTERN unsigned char volatile bt_queue_buf[PROTOCOL_HEAD + BT_UART_QUEUE_LMT];  //串口队列缓存
SYSTEM_EXTERN unsigned char bt_uart_rx_buf[PROTOCOL_HEAD + BT_UART_RECV_BUF_LMT];         //串口接收缓存
SYSTEM_EXTERN unsigned char bt_uart_tx_buf[PROTOCOL_HEAD + BT_UART_SEND_BUF_LMT];        //串口发送缓存
//
SYSTEM_EXTERN volatile unsigned char *queue_in;
SYSTEM_EXTERN volatile unsigned char *queue_out;

SYSTEM_EXTERN unsigned char stop_update_flag;

#ifndef BT_CONTROL_SELF_MODE
SYSTEM_EXTERN unsigned char reset_bt_flag;                                                  //重置bt标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN unsigned char set_btmode_flag;                                                //设置bluetooth工作模式标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN unsigned char bt_work_state;                                                  //bt模块当前工作状态
#endif


/*****************************************************************************
函数名称 : set_bt_uart_byte
功能描述 : 写bt_uart字节
输入参数 : dest:缓存区其实地址;
           byte:写入字节值
返回参数 : 写入完成后的总长度
*****************************************************************************/
unsigned short set_bt_uart_byte(unsigned short dest, unsigned char byte);

/*****************************************************************************
函数名称 : set_bt_uart_buffer
功能描述 : 写bt_uart_buffer
输入参数 : dest:目标地址
           src:源地址
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned short set_bt_uart_buffer(unsigned short dest, unsigned char *src, unsigned short len);

/*****************************************************************************
函数名称 : bt_uart_write_frame
功能描述 : 向bt串口发送一帧数据
输入参数 : fr_type:帧类型
           len:数据长度
返回参数 : 无
*****************************************************************************/
void bt_uart_write_frame(unsigned char fr_type, unsigned short len);

/*****************************************************************************
函数名称 : get_check_sum
功能描述 : 计算校验和
输入参数 : pack:数据源指针
           pack_len:计算校验和长度
返回参数 : 校验和
*****************************************************************************/
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);

/*****************************************************************************
函数名称 : data_handle
功能描述 : 数据帧处理
输入参数 : offset:数据起始位
返回参数 : 无
*****************************************************************************/
void data_handle(unsigned short offset);

/*****************************************************************************
函数名称 : get_queue_total_data
功能描述 : 读取队列内数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char get_queue_total_data(void);

/*****************************************************************************
函数名称 : Queue_Read_Byte
功能描述 : 读取队列1字节数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char Queue_Read_Byte(void);

#endif
  

