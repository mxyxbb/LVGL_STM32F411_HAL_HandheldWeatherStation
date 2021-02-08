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
#define SYSTEM_GLOBAL

#include "bluetooth.h"
//
//
extern const DOWNLOAD_CMD_S download_cmd[];

/*****************************************************************************
函数名称 : set_bt_uart_byte
功能描述 : 写bt_uart字节
输入参数 : dest:缓存区其实地址;
           byte:写入字节值
返回参数 : 写入完成后的总长度
*****************************************************************************/
unsigned short set_bt_uart_byte(unsigned short dest, unsigned char byte)
{
  unsigned char *obj = (unsigned char *)bt_uart_tx_buf + DATA_START + dest;
  
  *obj = byte;
  dest += 1;
  
  return dest;
}
/*****************************************************************************
函数名称 : set_bt_uart_buffer
功能描述 : 写bt_uart_buffer
输入参数 : dest:目标地址
           src:源地址
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned short set_bt_uart_buffer(unsigned short dest, unsigned char *src, unsigned short len)
{
  unsigned char *obj = (unsigned char *)bt_uart_tx_buf + DATA_START + dest;
  
  my_memcpy(obj,src,len);
  
  dest += len;
  return dest;
}
/*****************************************************************************
函数名称 : bt_uart_write_data
功能描述 : 向bt uart写入连续数据
输入参数 : in:发送缓存指针
           len:数据发送长度
返回参数 : 无
*****************************************************************************/
static void bt_uart_write_data(unsigned char *in, unsigned short len)
{
  if((NULL == in) || (0 == len))
  {
    return;
  }
  while(len --)
  {
    uart_transmit_output(*in);
    in ++;
  }
}
/*****************************************************************************
函数名称 : get_check_sum
功能描述 : 计算校验和
输入参数 : pack:数据源指针
           pack_len:计算校验和长度
返回参数 : 校验和
*****************************************************************************/
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
  unsigned short i;
  unsigned char check_sum = 0;
  
  for(i = 0; i < pack_len; i ++)
  {
    check_sum += *pack ++;
  }
  
  return check_sum;
}
/*****************************************************************************
函数名称 : bt_uart_write_frame
功能描述 : 向bt串口发送一帧数据
输入参数 : fr_type:帧类型
           len:数据长度
返回参数 : 无
*****************************************************************************/
void bt_uart_write_frame(unsigned char fr_type, unsigned short len)
{
  unsigned char check_sum = 0;
  
  bt_uart_tx_buf[HEAD_FIRST] = 0x55;
  bt_uart_tx_buf[HEAD_SECOND] = 0xaa;
  bt_uart_tx_buf[PROTOCOL_VERSION] = 0x00;
  bt_uart_tx_buf[FRAME_TYPE] = fr_type;
  bt_uart_tx_buf[LENGTH_HIGH] = len >> 8;
  bt_uart_tx_buf[LENGTH_LOW] = len & 0xff;
  
  len += PROTOCOL_HEAD;
  check_sum = get_check_sum((unsigned char *)bt_uart_tx_buf, len - 1);
  bt_uart_tx_buf[len - 1] = check_sum;
  //
  bt_uart_write_data((unsigned char *)bt_uart_tx_buf, len);
}
/*****************************************************************************
函数名称 : heat_beat_check
功能描述 : 心跳包检测
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void heat_beat_check(void)
{
  unsigned char length = 0;
  static unsigned char mcu_reset_state = FALSE;
  
  if(FALSE == mcu_reset_state)
  {
    length = set_bt_uart_byte(length,FALSE);
    mcu_reset_state = TRUE;
  }
  else
  {
    length = set_bt_uart_byte(length,TRUE);
  }
  
  bt_uart_write_frame(HEAT_BEAT_CMD, length);
}
/*****************************************************************************
函数名称  : product_info_update
功能描述  : 产品信息上传
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void product_info_update(void)
{
  unsigned char length = 0;
  
  length = set_bt_uart_buffer(length,(unsigned char *)PRODUCT_KEY,my_strlen((unsigned char *)PRODUCT_KEY));
  length = set_bt_uart_buffer(length,(unsigned char *)MCU_VER,my_strlen((unsigned char *)MCU_VER));
  
  bt_uart_write_frame(PRODUCT_INFO_CMD, length);
}
/*****************************************************************************
函数名称 : get_mcu_bt_mode
功能描述 : 查询mcu和bt的工作模式
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void get_mcu_bt_mode(void)
{
  unsigned char length = 0;
  
#ifdef BT_CONTROL_SELF_MODE                                   //模块自处理
  length = set_bt_uart_byte(length, BT_STATE_KEY);
  length = set_bt_uart_byte(length, BT_RESERT_KEY);
#else                                                           
  //无需处理数据
#endif
  
  bt_uart_write_frame(WORK_MODE_CMD, length);
}
/*****************************************************************************
函数名称 : get_update_dpid_index
功能描述 : 或许制定DPID在数组中的序号
输入参数 : dpid:dpid
返回参数 : index:dp序号
*****************************************************************************/
static unsigned char get_dowmload_dpid_index(unsigned char dpid)
{
  unsigned char index;
  unsigned char total = get_download_cmd_total();
  
  for(index = 0; index < total; index ++)
  {
    if(download_cmd[index].dp_id == dpid)
    {
      break;
    }
  }
  
  return index;
}
/*****************************************************************************
函数名称 : data_point_handle
功能描述 : 下发数据处理
输入参数 : value:下发数据源指针
返回参数 : ret:返回数据处理结果
*****************************************************************************/
static unsigned char data_point_handle(const unsigned char value[])
{
  unsigned char dp_id,index;
  unsigned char dp_type;
  unsigned char ret;
  unsigned short dp_len;
  
  dp_id = value[0];
  dp_type = value[1];
  dp_len = value[2] * 0x100;
  dp_len += value[3];
  
  index = get_dowmload_dpid_index(dp_id);

  if(dp_type != download_cmd[index].dp_type)
  {
    //错误提示
    return FALSE;
  }
  else
  {
    ret = dp_download_handle(dp_id,value + 4,dp_len);
  }
  
  return ret;
}
/*****************************************************************************
函数名称 : data_handle
功能描述 : 数据帧处理
输入参数 : offset:数据起始位
返回参数 : 无
*****************************************************************************/
void data_handle(unsigned short offset)
{
#ifdef SUPPORT_MCU_FIRM_UPDATE
  unsigned char *firmware_addr;
  static unsigned long firm_length;                                             //MCU升级文件长度
  static unsigned char firm_update_flag;                                        //MCU升级标志
  unsigned long dp_len;
#else
  unsigned short dp_len;
#endif
  unsigned char length = 0;
  
  unsigned char ret;
  unsigned short i,total_len;
  unsigned char cmd_type = bt_uart_rx_buf[offset + FRAME_TYPE];


  signed char bt_rssi;

#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE 
  bt_time_struct_data_t bt_time;
  unsigned short time_zone_100;
  char current_timems_string[14] = "000000000000";
  long long time_stamp_ms;
#endif

  switch(cmd_type)
  {
  case HEAT_BEAT_CMD:                                   //心跳包
    heat_beat_check();
    break;
    
  case PRODUCT_INFO_CMD:                                //产品信息
    product_info_update();
    break;
    
  case WORK_MODE_CMD:                                   //查询MCU设定的模块工作模式
    get_mcu_bt_mode();
    break;
    
#ifndef BT_CONTROL_SELF_MODE
  case BT_STATE_CMD:                                  //bt工作状态	
    bt_work_state = bt_uart_rx_buf[offset + DATA_START];
    if(bt_work_state==0x01||bt_work_state==0x00)
    {
    	mcu_ota_init_disconnect();

    }
    bt_uart_write_frame(BT_STATE_CMD,0);
    break;

  case BT_RESET_CMD:                                  //重置bt(bt返回成功)
    reset_bt_flag = RESET_BT_SUCCESS;
    break;
#endif
    
  case DATA_QUERT_CMD:                                  //命令下发
    total_len = bt_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
    total_len += bt_uart_rx_buf[offset + LENGTH_LOW];
    
    for(i = 0;i < total_len;)
    {
      dp_len = bt_uart_rx_buf[offset + DATA_START + i + 2] * 0x100;
      dp_len += bt_uart_rx_buf[offset + DATA_START + i + 3];
      //
      ret = data_point_handle((unsigned char *)bt_uart_rx_buf + offset + DATA_START + i);
      
      if(SUCCESS == ret)
      {
        //成功提示
      }
      else
      {
        //错误提示
      }
      
      i += (dp_len + 4);
    }
    
    break;
    
  case STATE_QUERY_CMD:                                 //状态查询
    all_data_update();                               
    break;
    
#ifdef TUYA_BCI_UART_COMMON_RF_TEST 
	case TUYA_BCI_UART_COMMON_RF_TEST:
		if(my_memcmp((unsigned char *)bt_uart_rx_buf + offset + DATA_START+7,"true",4)==0)
		{
			bt_rssi = (bt_uart_rx_buf[offset + DATA_START+21]-'0')*10 + (bt_uart_rx_buf[offset + DATA_START+22]-'0');
			bt_rssi = -bt_rssi;
			bt_rf_test_result(1,bt_rssi);
		}
		else
		{
			bt_rf_test_result(0,0);
		}
		break;
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE 
	case TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE:
		bt_send_recordable_dp_data_result(bt_uart_rx_buf[offset + DATA_START]);
		break;
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE 
	case TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE:
		ret = bt_uart_rx_buf[offset + DATA_START];
		if(ret==0)//获取时间成功
		{
			if(bt_uart_rx_buf[offset + DATA_START+1]==0x00)//时间格式0   	获取7字节时间时间类型+2字节时区信息
			{
				bt_time.nYear = bt_uart_rx_buf[offset + DATA_START+2] + 2018;

				bt_time.nMonth = bt_uart_rx_buf[offset + DATA_START+3];
				bt_time.nDay = bt_uart_rx_buf[offset + DATA_START+4];
				bt_time.nHour = bt_uart_rx_buf[offset + DATA_START+5];
				bt_time.nMin = bt_uart_rx_buf[offset + DATA_START+6];
				bt_time.nSec = bt_uart_rx_buf[offset + DATA_START+7];
				bt_time.DayIndex = bt_uart_rx_buf[offset + DATA_START+8];
				time_zone_100 = ((unsigned short)bt_uart_rx_buf[offset + DATA_START+9]<<8)+bt_uart_rx_buf[offset + DATA_START+10];
			}
			else if(bt_uart_rx_buf[offset + DATA_START+1]==0x01)//时间格式1	获取13字节ms级unix时间+2字节时区信息
			{
				my_memcpy(current_timems_string,&bt_uart_rx_buf[offset + DATA_START+2],13);
				time_stamp_ms = my_atoll(current_timems_string);
				time_zone_100 = ((unsigned short)bt_uart_rx_buf[offset + DATA_START+15]<8)+bt_uart_rx_buf[offset + DATA_START+16];
			}
			else if(bt_uart_rx_buf[offset + DATA_START+1]==0x02)//时间格式2	获取7字节时间时间类型+2字节时区信息
			{
				bt_time.nYear = bt_uart_rx_buf[offset + DATA_START+2] + 2000;
				bt_time.nMonth = bt_uart_rx_buf[offset + DATA_START+3];
				bt_time.nDay = bt_uart_rx_buf[offset + DATA_START+4];
				bt_time.nHour = bt_uart_rx_buf[offset + DATA_START+5];
				bt_time.nMin = bt_uart_rx_buf[offset + DATA_START+6];
				bt_time.nSec = bt_uart_rx_buf[offset + DATA_START+7];
				bt_time.DayIndex = bt_uart_rx_buf[offset + DATA_START+8];
				time_zone_100 = ((unsigned short)bt_uart_rx_buf[offset + DATA_START+9]<<8)+bt_uart_rx_buf[offset + DATA_START+10];
			}
			bt_time_sync_result(0,bt_uart_rx_buf[offset + DATA_START+1],bt_time,time_zone_100,time_stamp_ms);
		}
		else//获取时间失败
		{
			bt_time_sync_result(1,bt_uart_rx_buf[offset + DATA_START+1],bt_time,time_zone_100,time_stamp_ms);
		}
		break;
#endif

#ifdef TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL
	case TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL:
		bt_modify_adv_interval_result(bt_uart_rx_buf[offset + DATA_START]);
		break;
#endif
#ifdef TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME
	case TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME:
	  bt_close_timer_result(bt_uart_rx_buf[offset + DATA_START]);
	  break;
#endif
#ifdef TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER
	case TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER:
		bt_enable_lowpoer_result(bt_uart_rx_buf[offset + DATA_START]);
		break;
#endif
#ifdef TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN
	case TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN:
	  bt_send_one_time_password_token_result(bt_uart_rx_buf[offset + DATA_START]);
	  break;
#endif
#ifdef TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT
	case TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT:
		bt_disconnect_result(bt_uart_rx_buf[offset + DATA_START]);
		break;
#endif
#ifdef TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION
	case TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION:  
	  length = set_bt_uart_buffer(length,(unsigned char *)MCU_APP_VER_NUM,3);
	  length = set_bt_uart_buffer(length,(unsigned char *)MCU_HARD_VER_NUM,3);
	  bt_uart_write_frame(TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION,length);
	  break;
#endif
#ifdef TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY
	case TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY:	
		bt_factor_reset_notify();
		break;
#endif
#ifdef SUPPORT_MCU_FIRM_UPDATE
	  case TUYA_BCI_UART_COMMON_MCU_OTA_REQUEST:
	  case TUYA_BCI_UART_COMMON_MCU_OTA_FILE_INFO:
	  case TUYA_BCI_UART_COMMON_MCU_OTA_FILE_OFFSET:
	  case TUYA_BCI_UART_COMMON_MCU_OTA_DATA:
	  case TUYA_BCI_UART_COMMON_MCU_OTA_END:
		total_len = bt_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
		total_len += bt_uart_rx_buf[offset + LENGTH_LOW];
		mcu_ota_proc(cmd_type,&bt_uart_rx_buf[offset + DATA_START],total_len);
	  	break;
#endif   

  default:
    break;
  }
}
/*****************************************************************************
函数名称 : get_queue_total_data
功能描述 : 读取队列内数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/*****************************************************************************
函数名称 : Queue_Read_Byte
功能描述 : 读取队列1字节数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char Queue_Read_Byte(void)
{
  unsigned char value;
  
  if(queue_out != queue_in)
  {
    //有数据
    if(queue_out >= (unsigned char *)(bt_queue_buf + sizeof(bt_queue_buf)))
    {
      //数据已经到末尾
      queue_out = (unsigned char *)(bt_queue_buf);
    }
    
    value = *queue_out ++;   
  }
  
  return value;
}

