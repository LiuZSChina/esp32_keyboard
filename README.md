# 使用esp32为控制器的机械键盘
## **如何使用**

### PCB部分
Gerber文件位于 [PCB文件地址](https://github.com/LiuZSChina/esp32_keyboard/blob/main/Joker60/01/Gerber_PCB_Joker60_ver1.zip) 
大部分PCB定制厂商都支持直接使用Gerber文件制作

----
## **JOKER60**
`Joker 60` 
### **简介**
Joker60是一把poker配列61键键盘，具有一个fn键和一个pn键。
fn键用于输入第二层的键值，如<kbd>F1</kbd>、<kbd>ESC</kbd>（当左上角键被设置为<kbd>`</kbd>时）

### **快捷键**
+ 开启、关闭内置LED灯：<kbd>Pn</kbd> + <kbd>L</kbd>
+ 重置倒计时：<kbd>Pn</kbd> + <kbd>C</kbd>
+ 连接模式转换：<kbd>Pn</kbd> + <kbd>M</kbd> 并且保持三秒以上

### **1.0版本**——2022.4
初始发布版本，具有蓝牙和有线连接两种方式。\
**已知问题：** 
1. USB转蓝牙模式会导致死机
2. 更改模式需要重新烧录代码
### **1.1版本**——未发布
1. 增加开机状态切换模式
2. 增加CapsLock指示



## ~~JOKER40 **(未完成)**~~ 
