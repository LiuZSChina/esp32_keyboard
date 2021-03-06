#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\poker_keyboard.ino"
# include "poker_keyboard.h"
# include "poker_bt.h"
# include <BleKeyboard.h>
# include <Wire.h>
# include "keyboard_self.h"
# include <Arduino.h>
# include "pcf8575.h"
# include "scan.h"
# include "joker_usb.h"
# include "oled_buff.h"
# include "save.h"

void setup() {
  // DBG 设置
  Serial.begin(115200);
  Serial.println("Joker setting up!");
  delay(100);

  //读取保存的数据
  load_config();
  DFT_BOOT_MODE = Keyboard_Config.BOOT_MODE ;
  Serial.println(String(DFT_BOOT_MODE));
  Serial.println(String(Keyboard_Config.Version_KBD));
  // I2c 设置
  Wire.begin(21,22);
  Serial.println("I2C SET!");

  //初始化扫描工作
  scan_start();
  Serial.println("SCAN SET!");

  // 9328USB输出
  Serial2.begin(115200, SERIAL_8N1, 17, 16); 
  delay(100);
  Serial.println("USB SET!");

  //键盘硬件设置
  keyboard_setup();
  Oled_Start();
  LineDisp("      Joker 60 by LiuZS      ",3);
  LineDisp("====Started!====",0);
  display.drawString(0,0 ,"Joker On !");
  display.display();
  //LineDisp("                              1.0",4);
  delay(200);
  LineDisp(" ",3);
  Serial.println("Joker setted up!");


  if(DFT_BOOT_MODE == 0){
    joker_usb_start();
    //joker_usb_work();
  }
  if(DFT_BOOT_MODE == 1){
    joker_bt_start();
  }
  cnt_start();
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
}

#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\joker_usb.ino"
#include "joker_usb.h"
#include "scan.h"
#include "oled_buff.h"
#include "keyboard_self.h"
#include "poker_bt.h"
#include "save.h"
#include "esp32-hal.h"


void joker_usb_test(){
    if (key_code[2] == 0x00){
        key_code[2] = KEY_USB_A;
        Serial.println("set");
    }
    delay(100);
    Serial2.write(key_code,8);

    key_code[2] = 0x00;
    delay(1000);
    Serial2.write(key_code,8);
    
    Serial.println("sending");
    
}

void joker_usb_work(void *pvParameters){
    bool start_flag = 0;
    bool usb_send = 0;
    for (int i = 0; i < 8; i++)
    {
        key_code[i] = 0x00;
    }
    LineDisp("<MODE>===========USB", ble_line);
    LineDisp("<FN>----------------------------OFF", fn_line);
    int start_time = micros();
    
    for (;;){
        

        key_scan();

        //PN按下
        if(pn_stat){//pn被按下
            if ((key_press[LED_ROW][LED_COL]==0)&&(filter_key_press[LED_ROW][LED_COL]==1)){ //LED 控制
                if(!LED_STAT){
                    open_inter_led();
                    //draw_dog();
                }
                else{
                    close_inter_led();
                }
            }//LED 控制

            //画dog
            if ((key_press[DOG_ROW][DOG_COL]==0)&&(filter_key_press[DOG_ROW][DOG_COL]==1)){ 
                if(oled_mode!=1){
                    oled_mode = 1;
                    draw_dog();
                }
                else{
                    oled_mode = 0;
                }
            }
            //dog结束

            //MODE 控制
            if((key_press[MODE_ROW][MODE_COL]==0)&&(filter_key_press[MODE_ROW][MODE_COL]==1)){ // 第一次按下
                USB_to_change_mode = 1; 
                USB_chang_mode_time = millis();
                LineDisp("<MODE>===========CHG", ble_line);
            }
            if ((key_press[MODE_ROW][MODE_COL]==0)&&USB_to_change_mode&&((millis()-USB_chang_mode_time)>3000)){//连续按压了三秒以上
                USB_to_change_mode = 0;
                joker_usb2bt();
                LineDisp("<MODE>===========BTS", ble_line);
            }
            if(USB_to_change_mode&&(key_press[MODE_ROW][MODE_COL]==1)){//三秒内放开
                USB_to_change_mode = 0;
                LineDisp("<MODE>===========USB", ble_line);
            }
            //MODE 控制

            //重置倒计时
            if ((key_press[cnt_rst_ROW][cnt_rst_COL]==0)&&(filter_key_press[cnt_rst_ROW][cnt_rst_COL]==1)){ 
                rst_cnt_time = 1;
            }
            //重置倒计时


        }
        //PN第一次按下
        if(start_flag&&(pn_stat==0)&&(key_press[PN_ROW][PN_COL] == 0)){//PN第一次按下
            if (DBG_KEYBOARD){
                Serial.println("PN ON!");
            }
            for (int i = 0; i < 8; i++){
                key_code[i] = 0x00;
            }
            usb_send = 1;

            pn_stat = 1;
        }//pn第一次按下
        //PN第一次松开
        if(start_flag&&(pn_stat==1)&&(key_press[PN_ROW][PN_COL] == 1)){//PN第一次松开
            if (DBG_KEYBOARD){
                Serial.println("PN OFF!");
            }
            pn_stat = 0;
            USB_to_change_mode = 0;
        }//pn第一次松开

        // FN 第一次被按下
        if (start_flag&&(key_press[FN_ROW][FN_COL]==0)&&(fn_stat==0)){ 
            if (DBG_KEYBOARD){
            Serial.println("FN IS ON !");
            }
            LineDisp("<FN>-----------------------------ON", fn_line);
            fn_stat = 1;
            for (int ROW = 0; ROW < number_out; ROW++){//行循环判断
                for (int COL = 0; COL < number_in; COL++){//列循环
                    if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_ALL_FN[ROW][COL])){//键被按下，且此键的键值在fn按下后发生了变化
                        usb_send = 1;
                        if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//是第一位的键值
                                key_code[0] = key_code[0]|USB_LayOut_ALL[ROW][COL];
                        }//是第一位的键值

                        else{//是正常的键值
                            words_change(USB_LayOut_ALL[ROW][COL],1); //松开不是fn的键值
                            words_change(USB_LayOut_words_FN[ROW][COL],0); //按下fn对应的键值
                        }//是正常的键值
                    }// 按下fn后改变键值结束
                }//列循环
            }//行循环判断
        }//fn第一次按下结束
        // FN 第一次被松开
        if (start_flag&&(key_press[FN_ROW][FN_COL]==1)&&(fn_stat==1)){ 
            if (DBG_KEYBOARD){
                Serial.println("FN IS OFF !");
            }
            LineDisp("<FN>----------------------------OFF", fn_line);
            fn_stat = 0;

            /*
            for (int ROW = 0; ROW < number_out; ROW++){//行循环判断
                for (int COL = 0; COL < number_in; COL++){//列循环
                    if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_ALL_FN[ROW][COL])){//键被按下，且此键的键值在fn按下后发生了变化
                        usb_send = 1;
                        if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//是第一位的键值
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL[ROW][COL]);
                        }//是第一位的键值

                        else{//是正常的键值
                            words_change(USB_LayOut_words_FN[ROW][COL],1); //松开是fn的键值
                            words_change(USB_LayOut_words[ROW][COL],0); //按下fn对应的键值
                        }//是正常的键值
                    }//松开fn改变键值结束
                }
            }
            */
           
        }//fn第一次松开结束
        

        //循环赋值
        for (int ROW = 0; ROW < number_out; ROW++){//循环赋值
            for (int COL = 0; COL < number_in; COL++){
                if (start_flag && (old_key_press[ROW][COL]!=key_press[ROW][COL]) && (filter_key_press[ROW][COL]==key_press[ROW][COL]) &&(!pn_stat)){//键值变化且pn没有按下 并且连续两次按下的一样
                        usb_send = 1;
                    if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//是第一位的键值
                        if(key_press[ROW][COL]==0){//按下
                            if(fn_stat){
                                key_code[0] = key_code[0]|USB_LayOut_ALL_FN[ROW][COL];
                            }
                            else{
                                key_code[0] = key_code[0]|USB_LayOut_ALL[ROW][COL];
                            }
                        }
                        if(key_press[ROW][COL]==1){//松开
                            if(fn_stat){
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL_FN[ROW][COL]);
                            }
                            else{
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL[ROW][COL]);
                            }
                        }
                    }//是第一位的键值

                    else{//是正常的键值
                        if(fn_stat){
                                words_change(USB_LayOut_ALL_FN[ROW][COL],key_press[ROW][COL]);
                            }
                            else{
                                words_change(USB_LayOut_ALL[ROW][COL],key_press[ROW][COL]);
                            }
                    }//是正常的键值

                }//键值变化结束
               
            }//列结束   
        }//行结束
       
       if(!start_flag){
            for (int i = 0; i < number_out; i++){
                for (int j = 0; j < number_in; j++){
                    old_key_press[i][j] = key_press[i][j]; //如果通过了消抖则赋值
                    filter_key_press[i][j] = key_press[i][j]; //无论是否消抖都和前一样赋值
                } 
            }//新旧赋值结束
            start_flag = 1;
        }
        
        //新旧赋值
        for (int i = 0; i < number_out; i++){
            for (int j = 0; j < number_in; j++){
                if (filter_key_press[i][j]==key_press[i][j]){
                     old_key_press[i][j] = key_press[i][j]; //如果通过了消抖则赋值
                }
                filter_key_press[i][j] = key_press[i][j]; //无论是否消抖都和前一样赋值
            } 
        }//新旧赋值结束

        
        //发送按键
        if (usb_send){
            Serial2.write(key_code,8);
            usb_send = 0; //发送后设置为0

            if (DBG_KEYBOARD){
                Serial.println("sending:" + String(key_code[0]));
                Serial.println("Time:" + String(micros()-start_time));
                
            } 
        }
        start_time = micros();
        #ifdef USB_TASK_DELAY
        vTaskDelay(USB_TASK_DELAY);
        #endif
        
    }//循环扫描不会停止
    
}

void words_change(uint8_t word_name, bool stat){
    if(stat==0){//按下
        for (int i = 2; i < 8; i++){//找地方放
            if (key_code[i]==0x00){
                key_code[i]=word_name;
                    break;
            }  
        } 
    }
    if(stat==1){//松开
        for (int i = 2; i < 8; i++){//找地方放
            if (key_code[i]==word_name){
                key_code[i]=0x00;
                break;
            }  
        } 
    }
}


bool joker_usb_start(){
    disableCore0WDT();
    xReturned_usb = xTaskCreatePinnedToCore(joker_usb_work, "JOKER USB WORK", USB_TASK_STACK, NULL, USB_TASK_PRI, &USB_TASK_Handle, USB_TASK_CORE) ; 
    if(xReturned_usb == pdPASS){
        Serial.println("Usb Started!");
        return 1;
    }
    return 0;
}

/*
void joker_usb2bt(){
    Serial.println("CHANGING MODE!");
    joker_bt_start();
    if(USB_TASK_Handle!=NULL){
        USB_TASK_Handle = NULL;
        Serial.println("USB TASK DELETE");
        vTaskDelete(USB_TASK_Handle);
    }
}
*/

void joker_usb2bt(){
    Keyboard_Config.BOOT_MODE = 1;
    save_config();
    ESP.restart();
}

#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\keyboard_self.ino"
# include "keyboard_self.h"
# include "poker_keyboard.h"
# include "oled_buff.h"


void keyboard_setup(){
  //初始化io扩展
  //pcf8575_begin();
  pinMode(LED_PIN,OUTPUT);
  open_inter_led();
  invert_disp(1);
  delay(100);
  close_inter_led();
  invert_disp(0);
}

void open_inter_led(){
  digitalWrite(LED_PIN,HIGH);
  LED_STAT = 1;
}
void close_inter_led(){
  digitalWrite(LED_PIN,LOW);
  LED_STAT = 0;
}

void cnt_work(void *pvParameters){
  int start_time = millis();
  int time_pass;
  int cnt_down;
  for (;;){
    if(rst_cnt_time){
      start_time = millis();
      rst_cnt_time = 0;
      vTaskDelay(1000);
    }
    time_pass = millis() - start_time;
    cnt_down = CNT_TIME - time_pass;
    //LineDisp("<COUNT DOWN>---------" + String(cnt_down/60000),5);
    if (cnt_down<=0){
      LineDisp("<Need Rest in>---------NOW!",5);
      invert_disp(1);
    }
    else{
      LineDisp("<Need Rest in>---------" + String(cnt_down/60000),5);
      invert_disp(0);
    }
    vTaskDelay(1000);
  }
}

bool cnt_start(){
  xReturned_cnt = xTaskCreatePinnedToCore(cnt_work, "JOKER CNT WORK", CNT_TASK_STACK, NULL, CNT_TASK_PRI, &CNT_TASK_Handle, CNT_TASK_CORE) ; 
  if(xReturned_cnt == pdPASS){
    return 1;
  }
  return 0;
}


void keyboard_device_test(){
    open_i2c_keyboard_led();
    delay(100);
    close_i2c_caps_led();
    delay(5000);
    close_i2c_keyboard_led();
    delay(100);
    open_i2c_caps_led();
}



#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\oled_buff.ino"
#include "oled_buff.h"
# include "Pic.h"
# define HEIGHT_WORDS 9 

void Oled_Start(void){    // 开启oled显示
  display.init();
  display.flipScreenVertically();
  display.setBrightness(OLED_BRIGHT);
}

void LineDisp(String to_print,int line, bool if_clear){
  /*
    if (if_clear){
        for(int i=0; i<buff_size; i++){
            Line_Buff[i] = "";
        }
    }
    Line_Buff[line] = to_print;

    if(oled_mode == 0){
        display.clear();

        for(int i=0; i<buff_size; i++){
            display.drawString(0, HEIGHT_WORDS*i ,Line_Buff[i]);
        }
        display.display();
    }
    */
}

void invert_disp(bool if_invert){
    if (!if_invert){//不反向
        if (oled_invert){//是反向
            display.normalDisplay();
            oled_invert = 0;
        }
    }
    else{//反显
        if (!oled_invert){
            display.invertDisplay();
            oled_invert = 1;
        }
    }
    
}

void draw_dog(){
    if (oled_mode==1){
        display.clear(); //for Clearing the display
        display.drawXbm(0, 0, 128, 64, dog_bmp);
        display.display();
    }
}

#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\pcf8575.ino"
#include "pcf8575.h"

void pcf8575_begin(){
    pcf8575_writeAll(word(B11111111,B11111111));
}

void pcf8575_writeAll(uint16_t data) {
  Wire.beginTransmission(pcf_address);
  Wire.write(lowByte(data));
  Wire.write(highByte(data));
  Wire.endTransmission();
}

void pcf8575_readAll() {
    Wire.requestFrom(pcf_address, 16);    // Request 8 bytes from slave device number two
    // Slave may send less than requested
	byte b1 = Wire.read();
	byte b2 = Wire.read();

    if (DBG_KEYBOARD){
        Serial.println("..");
        Serial.println(b1);
	    Serial.println(b2);
    }
    
}

#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\poker_bt.ino"
#include "poker_bt.h"
#include "oled_buff.h"
#include "keyboard_self.h"
#include "joker_usb.h"
#include "save.h"

BleKeyboard bleKeyboard(DeviceName, Manufacturer, DevicePower);

void bt_test(){
  int i = 0;
    pinMode(34,INPUT);
    if(DBG_KEYBOARD){
        Serial.println("Starting BLE TEST!");
    }

    bleKeyboard.begin();//Start blekeyboard service
  pinMode(2,OUTPUT);
  //
    for(;;){
    
        if(bleKeyboard.isConnected()&&(!digitalRead(34))) {
            Serial.println("w");
            if(i==0){
              bleKeyboard.press(KEY_W);
              digitalWrite(2,HIGH);
            }
             if(i==1){
               bleKeyboard.releaseAll();
               digitalWrite(2,LOW);
             }

            //bleKeyboard.releaseAll();
            //
            // Below is an example of pressing multiple keyboard modifiers 
            // which by default is commented out. 
            // 
            /* Serial.println("Sending Ctrl+Alt+Delete...");
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.press(KEY_DELETE);
            delay(100);
            bleKeyboard.releaseAll();
            */
          i = (i+1)%2;
  }
    }
  Serial.println("Waiting 5 seconds...");
  delay(5000);
}

/*
蓝牙工作模式
*/
void bt_work(void *pvParameters){
  if(DBG_KEYBOARD){
        Serial.println("Starting BLE work!");
  }
  LineDisp("-------BLE------", ble_line);
  vTaskDelay(1000);
  bleKeyboard.begin();//Start blekeyboard service
  LineDisp("Waiting for BLE", ble_line);
  
  bool start_flag = 0;
  
//循环扫描
for (;;){
  if (bleKeyboard.isConnected()){//连接上
    bt_stat = 1;

     //第一次先赋值

    if (!start_flag){
      Serial.println("connected!");
      //LineDisp(">========BLE========<", ble_line);
      LineDisp("<MODE>===========BLE", ble_line);
      LineDisp("<FN>-------------------------OFF", fn_line);
    }
    
    //扫描
    key_scan();

    //PN按下
    if(pn_stat){//pn被按下
      if ((key_press[LED_ROW][LED_COL]==0)&&(old_key_press[LED_ROW][LED_COL]==1)){ //LED 控制
        if(!LED_STAT){
          open_inter_led();
        }
        else{
          close_inter_led();
        }
      }//LED 控制
      
      //MODE 控制
      if((key_press[MODE_ROW][MODE_COL]==0)&&(old_key_press[MODE_ROW][MODE_COL]==1)){ // 第一次按下
        bt_to_change_mode = 1; 
        bt_chang_mode_time = millis();
        LineDisp("<MODE>===========CHG", ble_line);
      }
      if ((key_press[MODE_ROW][MODE_COL]==0)&&bt_to_change_mode&&((millis()-bt_chang_mode_time)>3000)){//连续按压了三秒以上
        bt_to_change_mode = 0;
        joker_bt2usb();
        LineDisp("<MODE>===========LIS", ble_line);
      }
      if(bt_to_change_mode&&(key_press[MODE_ROW][MODE_COL]==1)){//三秒内放开
        bt_to_change_mode = 0;
        LineDisp("<MODE>===========BLE", ble_line);
      }
      //MODE 控制

      //重置倒计时
      if ((key_press[cnt_rst_ROW][cnt_rst_COL]==0)&&(old_key_press[cnt_rst_ROW][cnt_rst_COL]==1)){ 
        rst_cnt_time = 1;
      }
      //重置倒计时
    }
    //PN第一次按下
    if(start_flag&&(pn_stat==0)&&(key_press[PN_ROW][PN_COL] == 0)){//PN第一次按下
      if (DBG_KEYBOARD){
        Serial.println("PN ON!");
      }
      for (int i = 0; i < 8; i++){
        bleKeyboard.releaseAll();
      }

      pn_stat = 1;
    }//pn第一次按下
    //PN第一次松开
    if(start_flag&&(pn_stat==1)&&(key_press[PN_ROW][PN_COL] == 1)){//PN第一次松开
      if (DBG_KEYBOARD){
        Serial.println("PN OFF!");
      }
      pn_stat = 0;
    }//pn第一次松开

    // FN 第一次被按下
    if (start_flag&&(key_press[FN_ROW][FN_COL]==0)&&(fn_stat==0)){ 
      if (DBG_KEYBOARD){
        Serial.println("FN IS ON !");
      }
      LineDisp("<FN>--------------------------ON", fn_line);
      fn_stat = 1;

      for (int ROW = 0; ROW < number_out; ROW++){//行循环判断
        for (int COL = 0; COL < number_in; COL++){//列循环
          if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(LayOut_ALL[ROW][COL]!=LayOut_ALL_FN[ROW][COL])){//键被按下，且此键的键值在fn按下后发生了变化
            bleKeyboard.release(LayOut_ALL[ROW][COL]);
            bleKeyboard.press(LayOut_ALL_FN[ROW][COL]);
          }
        }
      }
    }//fn第一次按下结束

    if (start_flag&&(key_press[FN_ROW][FN_COL]==1)&&(fn_stat==1)){ // FN 第一次被松开
      if (DBG_KEYBOARD){
        Serial.println("FN IS OFF !");
      }
      LineDisp("<FN>-------------------------OFF", fn_line);
      fn_stat = 0;

      for (int ROW = 0; ROW < number_out; ROW++){//行循环判断
        for (int COL = 0; COL < number_in; COL++){//列循环
          if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(LayOut_ALL[ROW][COL]!=LayOut_ALL_FN[ROW][COL])){//键被按下，且此键的键值在fn按下后发生了变化
            bleKeyboard.release(LayOut_ALL_FN[ROW][COL]);
            bleKeyboard.press(LayOut_ALL[ROW][COL]);
          }
        }
      }
    }//fn第一次松开结束
    
    for (int ROW = 0; ROW < number_out; ROW++){//行循环判断
      for (int COL = 0; COL < number_in; COL++){//列循环
          
        //判断键值变化
        if ((!pn_stat)&&start_flag && ( (old_key_press[ROW][COL]!=key_press[ROW][COL]))){ 
          if (DBG_KEYBOARD){
            Serial.println("***["+ String(ROW) + "," + String(COL) + "]:" + String(key_press[ROW][COL])+ "means:" + String(LayOut_ALL[ROW][COL]) );
          }

          //发送按键
          if (LayOut_ALL[ROW][COL]!=0){
            if (key_press[ROW][COL]==0){//如果低电平则按下
              if (fn_stat){
                bleKeyboard.press(LayOut_ALL_FN[ROW][COL]);
              }
              else{
                bleKeyboard.press(LayOut_ALL[ROW][COL]);
              }
            }
            if(key_press[ROW][COL]==1){//高电平则松开
              if (fn_stat){
                bleKeyboard.release(LayOut_ALL_FN[ROW][COL]);
              }
              else{
                bleKeyboard.release(LayOut_ALL[ROW][COL]);
              }
            } 

          }//发送结束
            
        }//判断键值变化结束
        
      }//列循环
    }//行循环   
      //新旧赋值
        for (int i = 0; i < number_out; i++){
            for (int j = 0; j < number_in; j++){
                if (filter_key_press[i][j]==key_press[i][j]){
                     old_key_press[i][j] = key_press[i][j]; //如果通过了消抖则赋值
                }
                filter_key_press[i][j] = key_press[i][j]; //无论是否消抖都和前一样赋值
            } 
        }//新旧赋值结束



  } //连接上
  else{
    delay(10);//没连接上就等着
  }
  #ifdef BT_TASK_DELAY
        vTaskDelay(BT_TASK_DELAY);
        #endif

}//循环扫描不会结束

}//end bt_work

bool joker_bt_start(){
    Serial.println("bt starting!");
    xReturned_bt = xTaskCreatePinnedToCore(bt_work, "JOKER BT WORK", BT_TASK_STACK, NULL, BT_TASK_PRI, &BT_TASK_Handle, BT_TASK_CORE) ; 
    if(xReturned_bt == pdPASS){
        return 1;
    }
    return 0;
}

/*
void joker_bt2usb(){
    Serial.println("CHANGING MODE!");
    bleKeyboard.end();
    joker_usb_start();
    if(BT_TASK_Handle!=NULL){
        BT_TASK_Handle = NULL;
        Serial.println("BT TASK DELETE");
        vTaskDelete(BT_TASK_Handle);
    }
}
*/

void joker_bt2usb(){
    Serial.println("CHANGING MODE!");
    Keyboard_Config.BOOT_MODE = 0; 
    save_config();
    ESP.restart();
}
#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\save.ino"
#include <EEPROM.h>
#include "save.h" 

void save_config()
{
    EEPROM.begin(1024);
    uint8_t *p = (uint8_t *)(&Keyboard_Config);
    for (int i = 0; i < sizeof(Keyboard_Config); i++)
    {
        EEPROM.write(i, *(p + i));
    }
    EEPROM.end();
}

void load_config()
{
    EEPROM.begin(1024);
    uint8_t *p = (uint8_t *)(&Keyboard_Config);
    for (int i = 0; i < sizeof(Keyboard_Config); i++)
    {
        *(p + i) = EEPROM.read(i);
    }
    EEPROM.end();

    if (Keyboard_Config.Version_KBD != KEYBOARD_VER)
    {
        Serial.println("initing");
        Keyboard_Config.BOOT_MODE = DEFAULT_BOOT_MODE ; 
        Keyboard_Config.Version_KBD = KEYBOARD_VER;
        save_config();
    }
}


#line 1 "c:\\Users\\liuzeshu\\Desktop\\esp32_keyboard\\v1.0\\poker_keyboard\\scan.ino"
#include "scan.h"

void scan_start()
{
  //初始化输入引脚
  for (int i = 0; i < number_in; i++)
  {
    pinMode(key_pin_in[i], INPUT_PULLUP);
  }
  //初始化输出引脚
  for (int j = 0; j < number_out; j++)
  {
    pinMode(key_pin_out[j], OUTPUT);
    digitalWrite(key_pin_out[j], HIGH);
  }
}

void key_scan()
{
  //扫描
  for (int ROW = 0; ROW < number_out; ROW++)
  {
    //写电平
    if (ROW == 0)
    {
      digitalWrite(key_pin_out[(number_out - 1)], HIGH); //最后一组写为高，第一组写为低
      digitalWrite(key_pin_out[ROW], LOW);
    }
    else
    {
      digitalWrite(key_pin_out[(ROW - 1)], HIGH); //其余的将前一个写为高，自己写低
      digitalWrite(key_pin_out[ROW], LOW);
    }
    delayMicroseconds(10);
    //读取电平
    for (int COL = 0; COL < number_in; COL++)
    {
      key_press[ROW][COL] = digitalRead(key_pin_in[COL]);
    } //读取电平结束

  } //扫描结束
}
