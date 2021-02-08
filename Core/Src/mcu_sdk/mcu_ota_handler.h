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


#ifndef  TUYA_OTA_HANDLER_H_
#define  TUYA_OTA_HANDLER_H_

#include <stdint.h>

#include "bluetooth.h"
/******************************************************************************
                          1:配置升级         
MCU_OTA_VERSION MCU软件的版本
MCU_OTA_TYPE    0//0x00-允许升级， 0x01-拒绝升级

******************************************************************************/

#define MCU_OTA_VERSION MCU_APP_VER_NUM

#define MCU_OTA_TYPE    0//0x00-允许升级， 0x01-拒绝升级


/******************************************************************************
                          2:LOG         
如果想要打印一些log，可以完善下面宏

******************************************************************************/

#define TUYA_OTA_LOG(...)
#define  TUYA_OTA_HEXDUMP(...)



/******************************************************************************
                          3:配置升级地址         
APP_NEW_FW_START_ADR 新固件起始地址

APP_NEW_FW_END_ADR    0  新固件结束地址

APP_NEW_FW_SETTING_ADR  	固件升级配置信息存储地址

******************************************************************************/

#define	    APP_NEW_FW_START_ADR	            (0x44000)

#define	    APP_NEW_FW_END_ADR	                (0x61000-1)

#define 	APP_NEW_FW_SETTING_ADR				(0x63000)

#define	    APP_NEW_FW_MAX_SIZE                (APP_NEW_FW_END_ADR - APP_NEW_FW_START_ADR)//116K


#pragma anon_unions
typedef struct
{
    uint32_t command_size;              /**< The size of the current init command stored in the DFU settings. */
    uint32_t command_offset;            /**< The offset of the currently received init command data. The offset will increase as the init command is received. */
    uint32_t command_crc;               /**< The calculated CRC of the init command (calculated after the transfer is completed). */
    uint32_t data_object_size;          /**< The size of the last object created. Note that this size is not the size of the whole firmware image.*/
    struct
    {
        uint32_t firmware_file_version;
        uint32_t firmware_file_length;
        uint32_t firmware_file_crc;
        uint8_t  firmware_file_md5[16];        
    };
    union
    {
        struct
        {
            uint32_t firmware_image_crc;        /**< CRC value of the current firmware (continuously calculated as data is received). */
            uint32_t firmware_image_crc_last;   /**< The CRC of the last executed object. */
            uint32_t firmware_image_offset;     /**< The offset of the current firmware image being transferred. Note that this offset is the offset in the entire firmware image and not only the current object. */
            uint32_t firmware_image_offset_last;/**< The offset of the last executed object from the start of the firmware image. */
        };
        struct
        {
            uint32_t update_start_address;      /**< Value indicating the start address of the new firmware (before copy). It's always used, but it's most important for an SD/SD+BL update where the SD changes size or if the DFU process had a power loss when updating a SD with changed size. */
        };
    };
} dfu_progress_t;

/** @brief Description of a single bank. */
#pragma pack(4)
typedef struct
{
    uint32_t                image_size;         /**< Size of the image in the bank. */
    uint32_t                image_crc;          /**< CRC of the image. If set to 0, the CRC is ignored. */
    uint32_t                bank_code;          /**< Identifier code for the bank. */
} dfu_bank_t;

typedef struct
{
    uint32_t            crc;                /**< CRC for the stored DFU settings, not including the CRC itself. If 0xFFFFFFF, the CRC has never been calculated. */
    uint32_t            settings_version;   /**< Version of the current DFU settings struct layout. */
    uint32_t            app_version;        /**< Version of the last stored application. */
    uint32_t            bootloader_version; /**< Version of the last stored bootloader. */

    uint32_t            bank_layout;        /**< Bank layout: single bank or dual bank. This value can change. */
    uint32_t            bank_current;       /**< The bank that is currently used. */

    dfu_bank_t      bank_0;             /**< Bank 0. */
    dfu_bank_t      bank_1;             /**< Bank 1. */

    uint32_t            write_offset;       /**< Write offset for the current operation. */
    uint32_t            sd_size;            /**< Size of the SoftDevice. */

    dfu_progress_t      progress;           /**< Current DFU progress. */

    uint32_t            enter_buttonless_dfu;
    uint8_t             init_command[256];  /**< Buffer for storing the init command. */
} dfu_settings_t;


#define NRF_DFU_BANK_INVALID     0x00 /**< Invalid image. */
#define NRF_DFU_BANK_VALID_APP   0x01 /**< Valid application. */
#define NRF_DFU_BANK_VALID_SD    0xA5 /**< Valid SoftDevice. */
#define NRF_DFU_BANK_VALID_BL    0xAA /**< Valid bootloader. */
#define NRF_DFU_BANK_VALID_SD_BL 0xAC /**< Valid SoftDevice and bootloader. */

#define DFU_SETTING_SAVE_ADDR	APP_NEW_FW_SETTING_ADR


typedef enum 
{
	MCU_OTA_STATUS_NONE,
	MCU_OTA_STATUS_START,
	MCU_OTA_STATUS_FILE_INFO,
	MCU_OTA_STATUS_FILE_OFFSET,
    MCU_OTA_STATUS_FILE_DATA,
    MCU_OTA_STATUS_FILE_END,
	MCU_OTA_STATUS_MAX,
}mcu_ota_status_t;

/*****************************************************************************
函数名称 : mcu_ota_proc
功能描述 :mcu ota 处理函数
输入参数 :

返回参数 : 无
使用说明 : 
*****************************************************************************/

void mcu_ota_proc(uint16_t cmd,uint8_t *recv_data,uint32_t recv_len);
/*****************************************************************************
函数名称 : mcu_ota_init
功能描述 :mcu ota初始化函数
输入参数 :

返回参数 : 无
使用说明 : 用户需要将flash初始化函数在此完善，如果在其他处有flash初始化操作，该函数可以不被调用
*****************************************************************************/

uint32_t mcu_ota_init(void);
/*****************************************************************************
函数名称 : mcu_ota_init_disconnect
功能描述 :ota 状态初始化函数
输入参数 :

返回参数 : 无
使用说明 : 重启\模块掉线\ota失败后需要主动调用
*****************************************************************************/

uint8_t mcu_ota_init_disconnect(void);



#endif /* TUYA_OTA_H_ */





