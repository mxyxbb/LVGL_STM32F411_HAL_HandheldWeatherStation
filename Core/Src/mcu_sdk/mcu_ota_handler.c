/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
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
**
**-----------------------------------------------------------------------------
******************************************************************************/

/******************************************************************************
                          特别注意！！！      
MCU OTA的方式和芯片强相关，该MCU OTA程序demo不一定适用所有芯片平台，但大同小异，用户可根据自己芯片平台的升级方式对该demo进行修改或参考该demo自行编写完成MCU OTA功能

******************************************************************************/

#include "mcu_ota_handler.h"
#include "string.h"
#include "bluetooth.h"

#ifdef SUPPORT_MCU_FIRM_UPDATE

/*****************************************************************************
函数名称 : mcu_flash_init
功能描述 :flash初始化函数
输入参数 :

返回参数 : 无
使用说明 : 用户需要将flash初始化函数在此完善，如果在其他处有flash初始化操作，该函数可以不被调用
*****************************************************************************/
uint8_t mcu_flash_init(void)
{
//	#error "请自行完善该功能,完成后请删除该行"
}
/*****************************************************************************
函数名称 : mcu_flash_erase
功能描述 :flash擦除函数
输入参数 :addr 地址 size 大小

返回参数 : 无
使用说明 : 用户自行完善
*****************************************************************************/
uint8_t mcu_flash_erase(uint32_t addr,uint32_t size)
{
//	#error "请自行完善该功能,完成后请删除该行"
}
/*****************************************************************************
函数名称 : mcu_flash_write
功能描述 :flash写函数
输入参数 :addr 地址 size 大小 p_data待写入数据地址

返回参数 : 无
使用说明 : 用户自行完善
*****************************************************************************/

uint8_t mcu_flash_write(uint32_t addr, const uint8_t *p_data, uint32_t size)
{
//	#error "请自行完善该功能,完成后请删除该行"
}

/*****************************************************************************
函数名称 : mcu_flash_read
功能描述 :flash读函数
输入参数 :addr 地址 size 大小 p_data待读出数据地址

返回参数 : 无
使用说明 : 用户自行完善
*****************************************************************************/

uint8_t mcu_flash_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
//	#error "请自行完善该功能,完成后请删除该行"
}
/*****************************************************************************
函数名称 : mcu_device_delay_restart
功能描述 :延迟重启函数，建议延迟500ms重启，以等待mcu完成一些必要的操作
输入参数 : 

返回参数 : 无
使用说明 : 用户自行完善
*****************************************************************************/

void mcu_device_delay_restart(void)
{
//	error "请自行完善该功能,完成后请删除该行"
}







static dfu_settings_t s_dfu_settings;

static volatile mcu_ota_status_t tuya_ota_status;
void mcu_ota_status_set(mcu_ota_status_t status)
{
	tuya_ota_status = status;
}
mcu_ota_status_t mcu_ota_status_get(void)
{
	return tuya_ota_status;
}


#define MAX_DFU_DATA_LEN  200
#define CODE_PAGE_SIZE	4096
#define MAX_DFU_BUFFERS   ((CODE_PAGE_SIZE / MAX_DFU_DATA_LEN) + 1)


static uint32_t m_firmware_start_addr;          /**< Start address of the current firmware image. */
static uint32_t m_firmware_size_req;


static uint16_t current_package = 0;
static uint16_t last_package = 0;

static uint32_t crc32_compute(uint8_t const * p_data, uint32_t size, uint32_t const * p_crc)
{
	uint32_t crc;
    crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
    for (uint32_t i = 0; i < size; i++)
    {
        crc = crc ^ p_data[i];
        for (uint32_t j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}

static void mcu_ota_start_req(uint8_t*recv_data,uint32_t recv_len)
{
    uint8_t p_buf[12];
    uint8_t payload_len = 0;
    uint32_t current_version = MCU_OTA_VERSION;
	uint16_t length = 0;

    if(mcu_ota_status_get()!=MCU_OTA_STATUS_NONE)
    {
        TUYA_OTA_LOG("current ota status is not MCU_OTA_STATUS_NONE  and is : %d !",mcu_ota_status_get());
        return;
    }

    p_buf[0] = MCU_OTA_TYPE;
    p_buf[1] = (current_version>>16)&0xff;
    p_buf[2] = (current_version>>8)&0xff;
    p_buf[3] = current_version&0xff;

    p_buf[4] = MAX_DFU_DATA_LEN>>8;
    p_buf[5] = MAX_DFU_DATA_LEN;
    
    mcu_ota_status_set(MCU_OTA_STATUS_START);
    payload_len = 6;

	length = set_bt_uart_buffer(length,(unsigned char *)p_buf,payload_len);
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_OTA_REQUEST,length);
}


static uint8_t file_crc_check_in_flash(uint32_t len,uint32_t *crc)
{

    static uint8_t buf[257]={0};
    if(len == 0)
    {
        return 1;
    }
    uint32_t crc_temp = 0;
    uint32_t read_addr = APP_NEW_FW_START_ADR;
    uint32_t cnt = len/256;
    uint32_t remainder = len % 256;
    for(uint32_t i = 0; i<cnt; i++)
    {
        TUYA_OTA_LOG("%d",i);
        mcu_flash_read(read_addr,buf,256);
        crc_temp = crc32_compute(buf, 256, &crc_temp);
        read_addr += 256;
    }

    if(remainder>0&&remainder<256)
    {
        TUYA_OTA_LOG("%d",remainder);
        mcu_flash_read(read_addr,buf,remainder);
        crc_temp = crc32_compute(buf, remainder, &crc_temp);
        read_addr += remainder;
    }
    
    *crc = crc_temp;
    
    return 0;
}




static void mcu_ota_file_info_req(uint8_t*recv_data,uint32_t recv_len)
{
    uint8_t p_buf[30];
    uint8_t payload_len = 0;
    uint32_t file_version;
    uint32_t file_length;
    uint32_t file_crc;
    bool file_md5;
    // uint8_t file_md5[16];
    uint16_t length = 0;
    uint8_t state;
	
    if(mcu_ota_status_get()!=MCU_OTA_STATUS_START)
    {
        TUYA_OTA_LOG("current ota status is not MCU_OTA_STATUS_START  and is : %d !",mcu_ota_status_get());
        return;
    }

    file_version = recv_data[0+8]<<16;
    file_version += recv_data[1+8]<<8;
    file_version += recv_data[2+8];

    if(memcmp(s_dfu_settings.progress.firmware_file_md5,&recv_data[3+8],16)==0)
    {
        file_md5 = true;
    }
    else
    {
        file_md5 = false;
    }

    file_length = recv_data[27]<<24;
    file_length += recv_data[28]<<16;
    file_length += recv_data[29]<<8;
    file_length += recv_data[30];

    file_crc = recv_data[31]<<24;
    file_crc += recv_data[32]<<16;
    file_crc += recv_data[33]<<8;
    file_crc += recv_data[34];



    if (memcmp(&recv_data[0], PRODUCT_KEY, 8) == 0)
    {
        if((file_version > MCU_OTA_VERSION)&&(file_length <= APP_NEW_FW_MAX_SIZE))
        {

            if(file_md5&&(s_dfu_settings.progress.firmware_file_version==file_version)&&(s_dfu_settings.progress.firmware_file_length==file_length)
                    &&(s_dfu_settings.progress.firmware_file_crc==file_crc))
            {
                state = 0;
            }
            else
            {
                memset(&s_dfu_settings.progress, 0, sizeof(dfu_progress_t));
                s_dfu_settings.progress.firmware_image_crc_last = 0;
                s_dfu_settings.progress.firmware_file_version = file_version;
                s_dfu_settings.progress.firmware_file_length = file_length;
                s_dfu_settings.progress.firmware_file_crc = file_crc;
                memcpy(s_dfu_settings.progress.firmware_file_md5,&recv_data[3+8],16);
                s_dfu_settings.write_offset = s_dfu_settings.progress.firmware_image_offset_last;
                state = 0;
                mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
            }

            m_firmware_start_addr = APP_NEW_FW_START_ADR;
            m_firmware_size_req = s_dfu_settings.progress.firmware_file_length;

        }
        else
        {
            if(file_version <= MCU_OTA_VERSION)
            {
                TUYA_OTA_LOG("ota file version error !");
                state = 2;
            }
            else
            {
                TUYA_OTA_LOG("ota file length is bigger than rev space !");
                state = 3;
            }
        }

    }
    else
    {
        TUYA_OTA_LOG("ota pid error !");
        state = 1;
    }

    memset(p_buf,0,sizeof(p_buf));
    p_buf[0] = state;
    if(state==0)
    {
        uint32_t crc_temp  = 0;
        if(file_crc_check_in_flash(s_dfu_settings.progress.firmware_image_offset_last,&crc_temp)==0)
        {
            if(crc_temp != s_dfu_settings.progress.firmware_image_crc_last)
            {
                s_dfu_settings.progress.firmware_image_offset_last = 0;
                s_dfu_settings.progress.firmware_image_crc_last = 0;
                s_dfu_settings.write_offset = s_dfu_settings.progress.firmware_image_offset_last;
                mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
            }
        }

        p_buf[1] = s_dfu_settings.progress.firmware_image_offset_last>>24;
        p_buf[2] = s_dfu_settings.progress.firmware_image_offset_last>>16;
        p_buf[3] = s_dfu_settings.progress.firmware_image_offset_last>>8;
        p_buf[4] = (uint8_t)s_dfu_settings.progress.firmware_image_offset_last;
        
        p_buf[5] = s_dfu_settings.progress.firmware_image_crc_last>>24;
        p_buf[6] = s_dfu_settings.progress.firmware_image_crc_last>>16;
        p_buf[7] = s_dfu_settings.progress.firmware_image_crc_last>>8;
        p_buf[8] = (uint8_t)s_dfu_settings.progress.firmware_image_crc_last;
        mcu_ota_status_set(MCU_OTA_STATUS_FILE_INFO);
        current_package = 0;
        last_package = 0;

        TUYA_OTA_LOG("ota file length  : 0x%04x",s_dfu_settings.progress.firmware_file_length);
        TUYA_OTA_LOG("ota file  crc    : 0x%04x",s_dfu_settings.progress.firmware_file_crc);
        TUYA_OTA_LOG("ota file version : 0x%04x",s_dfu_settings.progress.firmware_file_version);
        TUYA_OTA_LOG("ota firmware_image_offset_last : 0x%04x",s_dfu_settings.progress.firmware_image_offset_last);
        TUYA_OTA_LOG("ota firmware_image_crc_last    : 0x%04x",s_dfu_settings.progress.firmware_image_crc_last);
        TUYA_OTA_LOG("ota firmware   write offset    : 0x%04x",s_dfu_settings.write_offset);

    }
    payload_len = 25;

	length = set_bt_uart_buffer(length,(unsigned char *)p_buf,payload_len);
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_OTA_FILE_INFO,length);


}


static void mcu_ota_offset_req(uint8_t*recv_data,uint32_t recv_len)
{
    uint8_t p_buf[5];
    uint8_t payload_len = 0;
    uint32_t offset;
	uint16_t length = 0;
	
    if(mcu_ota_status_get()!=MCU_OTA_STATUS_FILE_INFO)
    {
        TUYA_OTA_LOG("current ota status is not MCU_OTA_STATUS_FILE_INFO  and is : %d !",mcu_ota_status_get());
        return;
    }


    offset  = recv_data[0]<<24;
    offset += recv_data[1]<<16;
    offset += recv_data[2]<<8;
    offset += recv_data[3];

    if((offset==0)&&(s_dfu_settings.progress.firmware_image_offset_last!=0))
    {
        s_dfu_settings.progress.firmware_image_crc_last = 0;
        s_dfu_settings.progress.firmware_image_offset_last = 0;
        s_dfu_settings.write_offset = s_dfu_settings.progress.firmware_image_offset_last;
        mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
    }

    p_buf[0] = s_dfu_settings.progress.firmware_image_offset_last>>24;
    p_buf[1] = s_dfu_settings.progress.firmware_image_offset_last>>16;
    p_buf[2] = s_dfu_settings.progress.firmware_image_offset_last>>8;
    p_buf[3] = (uint8_t)s_dfu_settings.progress.firmware_image_offset_last;

    mcu_ota_status_set(MCU_OTA_STATUS_FILE_OFFSET);

    payload_len = 4;

	length = set_bt_uart_buffer(length,(unsigned char *)p_buf,payload_len);
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_OTA_FILE_OFFSET,length);


}



static void mcu_ota_data_req(uint8_t*recv_data,uint32_t recv_len)
{
	TUYA_OTA_LOG("%s",__func__);
    uint8_t p_buf[2];
    uint8_t payload_len = 0;
    uint8_t state = 0;
    uint16_t len;
    uint8_t p_balloc_buf[256];
    uint16_t length = 0;
    

    if((mcu_ota_status_get()!=MCU_OTA_STATUS_FILE_OFFSET)&&(mcu_ota_status_get()!=MCU_OTA_STATUS_FILE_DATA))
    {
        TUYA_OTA_LOG("current ota status is not MCU_OTA_STATUS_FILE_OFFSET  or MCU_OTA_STATUS_FILE_DATA and is : %d !",mcu_ota_status_get());
        return;
    }

    state = 0;


    current_package = (recv_data[0]<<8)|recv_data[1];
    len = (recv_data[2]<<8)|recv_data[3];

    if((current_package!=(last_package+1))&&(current_package!=0))
    {
        TUYA_OTA_LOG("ota received package number error.received package number : %d",current_package);
        state = 1;
    }
    else  if(len>MAX_DFU_DATA_LEN)
    {
        TUYA_OTA_LOG("ota received package data length error : %d",len);
        state = 5;
    }
    else
    {
        uint32_t const write_addr = APP_NEW_FW_START_ADR +  s_dfu_settings.write_offset;
        if(write_addr>=APP_NEW_FW_END_ADR)
        {
            TUYA_OTA_LOG("ota write addr error.");
            state = 1;
        }

        if(write_addr%CODE_PAGE_SIZE==0)
        {
            if (mcu_flash_erase(write_addr,4096)!= 0)
            {
                TUYA_OTA_LOG("ota Erase page operation failed");
                state = 4;
            }
        }

        if(state==0)
        {

            len = (recv_data[2]<<8)|recv_data[3];

            memcpy(p_balloc_buf, &recv_data[6], len);

            uint8_t ret = mcu_flash_write(write_addr, p_balloc_buf, len);
			TUYA_OTA_LOG("ota save len :%d",len);
			
            if (ret != 0)
            {
                state = 4;
            }
            else
            {
                s_dfu_settings.progress.firmware_image_crc_last = crc32_compute(p_balloc_buf, len, &s_dfu_settings.progress.firmware_image_crc_last);
                s_dfu_settings.write_offset    += len;
                s_dfu_settings.progress.firmware_image_offset_last += len;

                if((current_package+1)%32==0)
                {
                    mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
                }


            }
			
        }

    }

    p_buf[0] = state;

    mcu_ota_status_set(MCU_OTA_STATUS_FILE_DATA);

    payload_len = 1;

	length = set_bt_uart_buffer(length,(unsigned char *)p_buf,payload_len);
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_OTA_DATA,length);



    if(state!=0)//�������ָ���ʼ״̬
    {
        TUYA_OTA_LOG("ota error so free!");
        mcu_ota_status_set(MCU_OTA_STATUS_NONE);
        mcu_ota_init_disconnect();
        memset(&s_dfu_settings, 0, sizeof(dfu_settings_t));
        mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
    }
    else
    {
        last_package = current_package;
    }


}


static void reset_after_flash_write(void * p_context)
{
    TUYA_OTA_LOG("start reset~~~.");
    mcu_device_delay_restart();
}


static void on_dfu_complete(void)
{

    TUYA_OTA_LOG("All flash operations have completed. DFU completed.");

    reset_after_flash_write(NULL);
}



static void on_data_write_request_sched(void * data)
{
    uint8_t          ret;
    uint8_t p_buf[1];
    uint8_t payload_len = 0;
    uint8_t state;
	uint16_t length = 0;

    if (s_dfu_settings.progress.firmware_image_offset_last == m_firmware_size_req)
    {
        TUYA_OTA_LOG("Whole firmware image received. Postvalidating.");
        uint32_t crc_temp  = 0;
        if(file_crc_check_in_flash(s_dfu_settings.progress.firmware_image_offset_last,&crc_temp)==0)
        {
            if(s_dfu_settings.progress.firmware_image_crc_last != crc_temp)
            {
                TUYA_OTA_LOG("file crc check in flash diff from crc_last. firmware_image_offset_last = 0x%04x,crc_temp = 0x%04x,crc_last = 0x%04x",s_dfu_settings.progress.firmware_image_offset_last,crc_temp,s_dfu_settings.progress.firmware_image_crc_last);
                s_dfu_settings.progress.firmware_image_crc_last = crc_temp;
            }
            
        }
        TUYA_OTA_LOG("file crc check past.");
        
		TUYA_OTA_LOG("file crc check in flash diff from crc_last. firmware_image_offset_last = 0x%04x,crc_temp = 0x%04x,crc_last = 0x%04x",s_dfu_settings.progress.firmware_image_offset_last,crc_temp,s_dfu_settings.progress.firmware_image_crc_last);
        if(s_dfu_settings.progress.firmware_image_crc_last!=s_dfu_settings.progress.firmware_file_crc)
        {
            TUYA_OTA_LOG("ota file crc check error,last_crc = 0x%04x ,file_crc = 0x%04x",s_dfu_settings.progress.firmware_image_crc_last,s_dfu_settings.progress.firmware_file_crc);
            state = 2;
        }
        else
        {
            s_dfu_settings.bank_1.image_crc = s_dfu_settings.progress.firmware_image_crc_last;
            s_dfu_settings.bank_1.image_size = m_firmware_size_req;
            s_dfu_settings.bank_1.bank_code = NRF_DFU_BANK_VALID_APP;

            memset(&s_dfu_settings.progress, 0, sizeof(dfu_progress_t));

            s_dfu_settings.write_offset                  = 0;
            s_dfu_settings.progress.update_start_address = APP_NEW_FW_START_ADR;

            state = 0;


        }


    }
    else
    {

        state = 1;
    }

    p_buf[0] = state;
    mcu_ota_status_set(MCU_OTA_STATUS_NONE);
    payload_len = 1;

	length = set_bt_uart_buffer(length,(unsigned char *)p_buf,payload_len);
	bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_OTA_END,length);

    
    if(state==0)
    {
    	TUYA_OTA_LOG("ota will success!");
    	mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings)); 
        on_dfu_complete();
    }
    else
    {
        TUYA_OTA_LOG("ota crc error!");
        mcu_ota_status_set(MCU_OTA_STATUS_NONE);
        mcu_ota_init_disconnect();
        memset(&s_dfu_settings, 0, sizeof(dfu_settings_t));
        mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
    }

}



static void mcu_ota_end_req(uint8_t*recv_data,uint32_t recv_len)
{
    if(mcu_ota_status_get()==MCU_OTA_STATUS_NONE)
    {
        TUYA_OTA_LOG("current ota status is MCU_OTA_STATUS_NONE!");
        return;
    }
    on_data_write_request_sched(NULL);

}


void mcu_ota_proc(uint16_t cmd,uint8_t*recv_data,uint32_t recv_len)
{
    switch(cmd)
    {
    case TUYA_BCI_UART_COMMON_MCU_OTA_REQUEST:
        mcu_ota_start_req(recv_data,recv_len);
        break;
    case TUYA_BCI_UART_COMMON_MCU_OTA_FILE_INFO:
        mcu_ota_file_info_req(recv_data,recv_len);
        break;
    case TUYA_BCI_UART_COMMON_MCU_OTA_FILE_OFFSET:
        mcu_ota_offset_req(recv_data,recv_len);
        break;
    case TUYA_BCI_UART_COMMON_MCU_OTA_DATA:
        mcu_ota_data_req(recv_data,recv_len);
        break;
    case TUYA_BCI_UART_COMMON_MCU_OTA_END:
        mcu_ota_end_req(recv_data,recv_len);
        break;
    default:
    	TUYA_OTA_LOG("tuya_ota_proc cmd err.");
        break;
    }

}


uint8_t mcu_ota_init_disconnect(void)
{
    if(mcu_ota_status_get() != MCU_OTA_STATUS_NONE)
    {
        mcu_flash_write(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));
        mcu_ota_status_set(MCU_OTA_STATUS_NONE);
    }
    current_package = 0;
    last_package = 0;
	return 0;
}

uint32_t mcu_ota_init(void)
{
    mcu_ota_status_set(MCU_OTA_STATUS_NONE);

    current_package = 0;
    last_package = 0;
	mcu_flash_read(DFU_SETTING_SAVE_ADDR,(uint8_t*)&s_dfu_settings,sizeof(s_dfu_settings));


    return 0;
}
#endif

