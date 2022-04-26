# 使用esp32为控制器的机械键盘
## **如何使用**
## **所需工具**
1. **电烙铁及焊锡等** ***必须***\
    推荐使用可调温电烙铁，普通电烙铁也可以。尽量选择头部比较尖锐的烙铁，方便焊接小零件。*有些元件非常小，因此建议先学习电烙铁使用方法，不要一上来就焊接*
2. **螺丝刀** ***必须***\
    一把3.5mm十字花螺丝刀足够
3. **万用表** ***可选***\
    推荐购买一个万用表，可以方便得检查焊接是否正确。没有也不影响正常组装，但是一旦出现问题难以排查。*万用表质量不需要非常好，便宜的就能够胜任*
4. **镊子** ***可选但是强烈推荐***\
    部分零件非常小，除非你的手特别灵活或者你特别闲脾气又好否则没有镊子你会气死。
5. **小刀** ***可选***\
    用来修整3D打印带来的毛边等。小号的壁纸刀最好。
6. **电线** ***v1.0版本必须***\
    用来飞线。建议使用26AWG粗细的。
7. **BOM表上的其他零件** ***必须***\
    [JOKER60 BOM](https://github.com/LiuZSChina/esp32_keyboard/blob/main/Joker60/01/BOM_PCB_Joker60_ver1_2022-04-18.csv)。除了芯片类的东西比如螺丝、二极管等尽量多买几个，*不建议买正好数量，尤其是螺丝*。建议在立创商城搜索相关零件，如果去淘宝购买注意要购买针脚一样的（***USB接口和开关是重点***）否则装不上。（当然我是在淘宝买的，比较便宜 **:D**


###  **PCB部分**
Gerber文件：
1. [JOKER60 PCB](https://github.com/LiuZSChina/esp32_keyboard/blob/main/Joker60/01/Gerber_PCB_Joker60_ver1.zip) \
大部分PCB定制厂商都支持直接使用Gerber文件制作。推荐制作1.6mm厚度。
### **外壳、定位板部分**
外壳、定位板文件：
1. [JOKER60 外壳](https://github.com/LiuZSChina/esp32_keyboard/tree/main/Joker60/3D/ver1.0)\
-定位板推荐使用1.5mm厚度。不锈钢、亚克力等都可以，但是手感不同。样机使用不锈钢、1.5mm激光切割\
-外壳为stl文件、可以直接进行3D打印。打印后压入热熔螺母（M3、外径4.3mm，长度4mm）（需要使用电烙铁进行操作）\
-（可选）亚克力推荐使用2.5mm左右材料进行制造，装饰面板需要与屏幕盖板粘合。不使用这些（亚克力材质的）零件不影响使用，但是电路板会暴露在外，容易积灰。
### **制作**
1. 找厂家生产pcb和外壳等，并购买所需要元件（详见BOM表）
2. 按照BOM表上的元件位号焊接材料，主要二极管（1N4147）有反正之分，请注意区分。
3. 刷入固件，详见各版本介绍中烧录条目。源码中的库需要在Arduino中安装。

----
## **JOKER60**
`Joker 60` 
### **简介**
Joker60是一把poker配列61键键盘，具有一个fn键和一个pn键。
fn键用于输入第二层的键值，如<kbd>F1</kbd>、<kbd>ESC</kbd>（当左上角键被设置为<kbd>`</kbd>时）\
![JOKER60 Layout](https://github.com/LiuZSChina/esp32_keyboard/blob/main/Joker60/Joker60-layout.jpg)\
OLED屏幕可以显示当前模式，fn键状态。并且具有每30分钟提醒休息的功能，也可以自己修改源代码进行配置\
具有外接电池位置(PCB上B+、GND) 、外接LED灯或其他部件位置(x_LED)和i2c总线接口（*OLED屏幕使用中、可以串联其他不同地址的i2c设备同时使用*）

### **烧录**
+ **1.0版本**\
    需要将开关打至接通ch340的档位，具体方向和焊接方向有关，可以从电脑设备管理器中查看是否连接上ch340进行判断(***v1.0版本（Gerber_PCB_Joker60_ver1）* 中ch340的tx和rx接线是反的，需要飞线调整，并且用小刀割断pcb上的连线**)\
    将gnd和boot短接后通电，进入下载模式，使用arduino ide或其他编辑器上传固件（具体方法网上很多教程）。gnd和boot在断开的情况再次断电再通电即可退出下载模式运行键盘程序，此时usb口功能通过开关选择ch340和ch9328在串口调试和USB键盘之间切换，*无法同时使用*

### **快捷键**
+ 开启、关闭内置LED灯：<kbd>Pn</kbd> + <kbd>L</kbd>
+ 重置倒计时：<kbd>Pn</kbd> + <kbd>C</kbd>
+ 连接模式转换：<kbd>Pn</kbd> + <kbd>M</kbd> 并且保持三秒以上

### **1.0版本**
——2022.4——\
初始发布版本，具有蓝牙和有线连接两种方式。\
**已知问题：** 
1. USB转蓝牙模式会导致死机
2. 更改模式需要重新烧录代码
3. CH340N的rx、tx接反了，需要飞线连接。
### **1.1版本**——未发布
*预计更改*：
1. 增加开机状态切换模式的功能
2. 增加CapsLock指示
3. 修改PCB文件



## ~~JOKER40~~ **(未完成)** 
