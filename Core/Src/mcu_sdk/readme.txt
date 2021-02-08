
此MCU_SDK是根据涂鸦开发平台上定义的产品功能，自动生成的MCU代码。在此基础上进行修改、补充，可快速完成MCU程序。



开发步骤：

1：需要根据产品实际情况（重置bluetooth按钮和bluetooth状态指示灯处理方式、是否支持MCU升级等）进行配置，请在protocol.h内修改此配置；
2：移植此MCU_SDK，请查看protocol.c文件内的移植步骤,并正确完成移植。移植后，请完成数据下发处理、数据上报部分的代码，即可完成全部bluetooth功能。



文件概览：
此MCU_SDK包括9个文件：
	（1）protocol.h和protocol.c是需要你修改的。protocol.h 和protocol.c文件内有详细修改说明，请仔细阅读。
	（2）bluetooth.h文件为总的.h文件，如需要调用bluetooth内部功能，请#include "bluetooth.h"。
	（3）system.c和system.h是bluetooth功能实现代码，用户无需修改。
	（4）mcu_api.c和mcu_api.h内实现全部此用户需调用函数，用户无需修改。
	（5）mcu_ota_handler.h 和 mcu_ota_handler.c 
	是你需要修改的。ota和芯片强相关，需要用户自行调试修改适用自己的芯片平台，这两个文件仅作为一个参考