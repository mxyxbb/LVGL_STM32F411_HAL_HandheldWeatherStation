# LVGL_STM32F411_HAL_HandheldWeatherStation

LVGL v6.2 | STM32F411CCU6 | use Stm32CubeMX

This project is developed using Tuya SDK, which enables you to quickly develop branded apps connect ing and cont rolling smart scenarios of many devices. For more informat ion, please check Tuya Developer Website.

video:https://www.bilibili.com/video/BV1Vv411e7ax/

PCB project:https://oshwhub.com/soulhand/tu-ya-zhi-neng-di-gong-hao-huan-jing-zhuan-gan


# 【快乐DIY】掌上气象站


> 不要相信压力会产生动力，压力只会转化成病历。人生真正的动力，是源自内心深处对于快乐和兴趣的追求



你好，我一直对于PCB和单片机比较感兴趣，这项工程向大家分享一下我最近制作的一个小东西，我叫它【掌上气象站】。

下面是PCB的原理图、2D图像和3D渲染图。


<img src="https://gitee.com/buddismblingblinghead/MxyPic/raw/master/img/image-20210125213416026.png" alt="image-20210125213416026" style="zoom: 50%;" />

![预览-3d-keyshot-big.jpg](https://gitee.com/buddismblingblinghead/MxyPic/raw/master/img/GrO41nWC4SD5Ic41RuDKGGvQHAi44D4A26bCRIQF.jpeg)



------



先说一下前因，前些日子半夜在网上瞎看时，看到了几个有趣的单片机作品，让人甚是心动，于是俺也想做一个。

> 下面是几个有意思的手表
>
> ①iWatch-v1.2 自制赛博朋克风可编程手表 https://www.bilibili.com/video/BV11T4y1K7Li
>
> ②第五届立创电子设计大赛：《Modular-NOW》项目 https://www.bilibili.com/video/BV1sA411n7v9
>
> ③基于STM32的手环设计 https://www.bilibili.com/video/BV1s54y1C7JV

有这么多案例可以参考学习，那么俺就入坑了。

入坑的时间大概是**期末考试**前四五天，快乐当前，期末考试算得了啥（不是）。

那几天，连夜分析了别人的作品，

这里是当时花了几晚上，边分析边写的东西，https://www.yuque.com/haomingzi-syzrw/gtkwig/gsbkfg

经过一番分析，感觉本菜鸡还是有希望把它做出来的。

另外，近几天适逢立创EDA举办了训练营，可以白嫖元器件，所以就动工了。

下图是我的主要元器件选型，

其实选器件的依据很直接，别人用过的，有参考代码的，我就用(●'◡'●)

![image-20210125212702696](https://gitee.com/buddismblingblinghead/MxyPic/raw/master/img/image-20210125212702696.png)


板载输入设备：

- 两个触摸按键——具体原理我也不懂，别人怎么用我就怎么用，能实现功能就行
- BME280——可以获取所处环境的温度、湿度、气压
- LSM6DSM——可以获取自身的三轴加速度和三轴角加速度

预期实现的功能如下：

- 时钟、闹钟、秒表、日历等时间功能

- 计步器

- 温湿度、气压显示

- 抬起亮屏

- 有能力就再加小游戏

器件选型：

单片机：stm32F411ccu6（100Mhz）

电源管理芯片：TP4056

3.3V电源dcdc芯片：TPS62740DSSR

锂电池：3.7V/200mAh，厚5mm宽20长30

显示屏：ips 0.96寸TFT

陀螺仪：LSM6DSM

触摸按键芯片：TTP232-CA6

蜂鸣器：贴片蜂鸣器5020

环境参数传感器：BME280

使用LVGL编写显示界面

移植了LSM6DSM的ST官方库，可读出三轴加速度和角加速度，库中带有很多例子，如实现方向检测、抬腕检测、自由落体检测等等...，本菜鸡因为懒所以没有额外添加功能

### **目前的功能**:

- RTC时间和日期显示
- 温湿度显示
- 蓝牙连接手机涂鸦智能APP，上报温湿度数据
- 串口以115200波特率发送LSM6DSM加速度计数据

工程代码：https://github.com/mxyxbb/LVGL_STM32F411_HAL_HandheldWeatherStation

前往Bilibili查看视频：https://www.bilibili.com/video/BV1Vv411e7ax/

## 相关说明：

### Github下载如何提速

原命令：`git clone https://github.com/mxyxbb/LVGL_STM32F411_HAL_HandheldWeatherStation`
现在替换为：`git clone https://github.com.cnpmjs.org/mxyxbb/LVGL_STM32F411_HAL_HandheldWeatherStation`
也就是把原 URL 中的 github.com 替换为 github.com.cnpmjs.org，其余保持不变