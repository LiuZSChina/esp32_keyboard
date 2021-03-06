#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_keyboard.ino"
# include "poker_keyboard.h"
# include "poker_bt.h"
# include <BleKeyboard.h>
# include <Wire.h>
# include "keyboard_self.h"
# include <Arduino.h>
# include "pcf8575.h"
# include "scan.h"
# include "joker_usb.h"
# include "save.h"
# include "Seg_disp.h"
# include "oled_buff.h"

#line 14 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_keyboard.ino"
void setup();
#line 60 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_keyboard.ino"
void loop();
#line 3 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\Seg_disp.ino"
void init_seg();
#line 11 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\joker_usb.ino"
void joker_usb_test();
#line 27 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\joker_usb.ino"
void joker_usb_work(void *pvParameters);
#line 267 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\joker_usb.ino"
void words_change(uint8_t word_name, bool stat);
#line 287 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\joker_usb.ino"
bool joker_usb_start();
#line 309 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\joker_usb.ino"
void joker_usb2bt();
#line 8 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
void keyboard_setup();
#line 35 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
void open_inter_led();
#line 39 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
void close_inter_led();
#line 44 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
void cnt_work(void *pvParameters);
#line 72 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
bool cnt_start();
#line 83 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
void keyboard_device_test();
#line 3 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\pcf8575.ino"
void pcf8575_begin();
#line 7 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\pcf8575.ino"
void pcf8575_writeAll(uint16_t data);
#line 14 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\pcf8575.ino"
void pcf8575_readAll();
#line 9 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_bt.ino"
void bt_test();
#line 54 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_bt.ino"
void bt_work(void *pvParameters);
#line 258 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_bt.ino"
bool joker_bt_start();
#line 280 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_bt.ino"
void joker_bt2usb();
#line 4 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\save.ino"
void save_config();
#line 15 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\save.ino"
void load_config();
#line 3 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\scan.ino"
void scan_start();
#line 18 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\scan.ino"
void key_scan_once();
#line 43 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\scan.ino"
void key_scan();
#line 14 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_keyboard.ino"
void setup() {
  // DBG ??????
  Serial.begin(115200);
  Serial.println("Joker setting up!");
  delay(100);

  //?????????????????????
  load_config();
  DFT_BOOT_MODE = Keyboard_Config.BOOT_MODE ;
  Serial.println(String(DFT_BOOT_MODE));
  Serial.println(String(Keyboard_Config.Version_KBD));
  // I2c ??????
  #ifdef Joker
  Wire.begin(21,22);
  Serial.println("I2C SET!");
  #endif
  
  //?????????????????????
  scan_start();
  Serial.println("SCAN SET!");

  // 9328USB??????
  #ifdef Joker
  Serial2.begin(115200, SERIAL_8N1, 17, 16); 
  #endif
  #ifdef MasterYoda
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  #endif

  delay(100);
  Serial.println("USB SET!");

  //??????????????????
  keyboard_setup();
  Serial.println("Joker setted up!");

  if(DFT_BOOT_MODE == 0){
    joker_usb_start();
    //joker_usb_work();
  }
  if(DFT_BOOT_MODE == 1){
    //joker_bt_start();
  }
  //cnt_start();
}

void loop() {
  // put your main code here, to run repeatedly:
tm1637.display(2, (8));
    tm1637.display(3, (8));
  delay(1000);
}

#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\Seg_disp.ino"
#include "Seg_disp.h"

void init_seg(){
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
    tm1637.display(0, (8));
    tm1637.display(1, (8));
    tm1637.display(2, (8));
    tm1637.display(3, (8));
    tm1637.point(0);
    Serial.println("Seg ON");
    delay(200);
}
#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\joker_usb.ino"
#include "joker_usb.h"
#include "scan.h"
#include "oled_buff.h"
#include "keyboard_self.h"
#include "poker_bt.h"
#include "save.h"
#include "esp32-hal.h"
#include "Layout.h"


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

    #ifdef Joker
    LineDisp("<MODE>===========USB", ble_line);
    LineDisp("<FN>----------------------------OFF", fn_line);
    display.drawString(0, ble_line*8 ,"<MODE>===========USB");
    display.display();
    #endif

    int start_time = micros();
    
    for (;;){
        

        key_scan();

        //PN??????
        #ifdef Joker
        //PN??????
        if(pn_stat){//pn?????????
            if ((key_press[LED_ROW][LED_COL]==0)&&(old_key_press[LED_ROW][LED_COL]==1)){ //LED ??????
                if(!LED_STAT){
                    open_inter_led();
                    //draw_dog();
                }
                else{
                    close_inter_led();
                }
            }//LED ??????

            //???dog
            if ((key_press[DOG_ROW][DOG_COL]==0)&&(old_key_press[DOG_ROW][DOG_COL]==1)){ 
                if(oled_mode!=1){
                    oled_mode = 1;
                    draw_dog();
                }
                else{
                    oled_mode = 0;
                }
            }
            //dog??????

            //MODE ??????
            if((key_press[MODE_ROW][MODE_COL]==0)&&(old_key_press[MODE_ROW][MODE_COL]==1)){ // ???????????????
                USB_to_change_mode = 1; 
                USB_chang_mode_time = millis();
                LineDisp("<MODE>===========CHG", ble_line);
            }
            if ((key_press[MODE_ROW][MODE_COL]==0)&&USB_to_change_mode&&((millis()-USB_chang_mode_time)>3000)){//???????????????????????????
                USB_to_change_mode = 0;
                joker_usb2bt();
                LineDisp("<MODE>===========BTS", ble_line);
            }
            if(USB_to_change_mode&&(key_press[MODE_ROW][MODE_COL]==1)){//???????????????
                USB_to_change_mode = 0;
                LineDisp("<MODE>===========USB", ble_line);
            }
            //MODE ??????

            //???????????????
            if ((key_press[cnt_rst_ROW][cnt_rst_COL]==0)&&(old_key_press[cnt_rst_ROW][cnt_rst_COL]==1)){ 
                rst_cnt_time = 1;
            }
            //???????????????


        }
        //PN???????????????
        if(start_flag&&(pn_stat==0)&&(key_press[PN_ROW][PN_COL] == 0)){//PN???????????????
            if (DBG_KEYBOARD){
                Serial.println("PN ON!");
            }
            for (int i = 0; i < 8; i++){
                key_code[i] = 0x00;
            }
            usb_send = 1;

            pn_stat = 1;
        }//pn???????????????
        //PN???????????????
        if(start_flag&&(pn_stat==1)&&(key_press[PN_ROW][PN_COL] == 1)){//PN???????????????
            if (DBG_KEYBOARD){
                Serial.println("PN OFF!");
            }
            pn_stat = 0;
            USB_to_change_mode = 0;
        }//pn???????????????
        #endif

        // FN ??????????????????
        if (start_flag&&(key_press[FN_ROW][FN_COL]==0)&&(fn_stat==0)){ 
            if (DBG_KEYBOARD){
            Serial.println("FN IS ON !");
            }
            #ifdef Joker
            LineDisp("<FN>-----------------------------ON", fn_line);
            #endif
            fn_stat = 1;
            for (int ROW = 0; ROW < number_out; ROW++){//???????????????
                for (int COL = 0; COL < number_in; COL++){//?????????
                    if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_ALL_FN[ROW][COL])){//????????????????????????????????????fn????????????????????????
                        usb_send = 1;
                        if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//?????????????????????
                                key_code[0] = key_code[0]|USB_LayOut_ALL[ROW][COL];
                        }//?????????????????????

                        else{//??????????????????
                            words_change(USB_LayOut_ALL[ROW][COL],1); //????????????fn?????????
                            words_change(USB_LayOut_words_FN[ROW][COL],0); //??????fn???????????????
                        }//??????????????????
                    }// ??????fn?????????????????????
                }//?????????
            }//???????????????
        }//fn?????????????????????
        // FN ??????????????????
        if (start_flag&&(key_press[FN_ROW][FN_COL]==1)&&(fn_stat==1)){ 
            if (DBG_KEYBOARD){
                Serial.println("FN IS OFF !");
            }
            #ifdef Joker
            LineDisp("<FN>----------------------------OFF", fn_line);
            #endif
            fn_stat = 0;

            /*
            for (int ROW = 0; ROW < number_out; ROW++){//???????????????
                for (int COL = 0; COL < number_in; COL++){//?????????
                    if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_ALL_FN[ROW][COL])){//????????????????????????????????????fn????????????????????????
                        usb_send = 1;
                        if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//?????????????????????
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL[ROW][COL]);
                        }//?????????????????????

                        else{//??????????????????
                            words_change(USB_LayOut_words_FN[ROW][COL],1); //?????????fn?????????
                            words_change(USB_LayOut_words[ROW][COL],0); //??????fn???????????????
                        }//??????????????????
                    }//??????fn??????????????????
                }
            }
            */
             for (int ROW = 0; ROW < number_out; ROW++){//???????????????
                for (int COL = 0; COL < number_in; COL++){//?????????
                    if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_ALL_FN[ROW][COL])){//????????????????????????????????????fn????????????????????????
                        usb_send = 1;
                        if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//?????????????????????
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL[ROW][COL]);
                        }//?????????????????????

                        else{//??????????????????
                            words_change(USB_LayOut_words_FN[ROW][COL],1); //?????????fn?????????
                        }//??????????????????
                    }//??????fn??????????????????
                }
            }
           
        }//fn?????????????????????
        

        //????????????
        for (int ROW = 0; ROW < number_out; ROW++){//????????????
            for (int COL = 0; COL < number_in; COL++){
                if (start_flag && (old_key_press[ROW][COL]!=key_press[ROW][COL]) &&(!pn_stat)){//???????????????pn???????????? ?????????????????????????????????
                        usb_send = 1;
                    if(USB_LayOut_ALL[ROW][COL]!=USB_LayOut_words[ROW][COL]){//?????????????????????
                        if(key_press[ROW][COL]==0){//??????
                            if(fn_stat){
                                key_code[0] = key_code[0]|USB_LayOut_ALL_FN[ROW][COL];
                            }
                            else{
                                key_code[0] = key_code[0]|USB_LayOut_ALL[ROW][COL];
                            }
                        }
                        if(key_press[ROW][COL]==1){//??????
                            if(fn_stat){
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL_FN[ROW][COL]);
                            }
                            else{
                                key_code[0] = key_code[0]&(!USB_LayOut_ALL[ROW][COL]);
                            }
                        }
                    }//?????????????????????

                    else{//??????????????????
                        if(fn_stat){
                                words_change(USB_LayOut_ALL_FN[ROW][COL],key_press[ROW][COL]);
                            }
                            else{
                                words_change(USB_LayOut_ALL[ROW][COL],key_press[ROW][COL]);
                            }
                    }//??????????????????

                }//??????????????????
               
            }//?????????   
        }//?????????
       
       if(!start_flag){
            for (int i = 0; i < number_out; i++){
                for (int j = 0; j < number_in; j++){
                    old_key_press[i][j] = key_press[i][j]; //??????????????????????????????
                } 
            }//??????????????????
            start_flag = 1;
        }
        
        //????????????
        for (int i = 0; i < number_out; i++){
            for (int j = 0; j < number_in; j++){
                 old_key_press[i][j] = key_press[i][j]; //??????
            } 
        }//??????????????????

        
        //????????????
        if (usb_send){
            Serial2.write(key_code,8);
            usb_send = 0; //??????????????????0

            if (DBG_KEYBOARD){
                Serial.println("sending:" + String(key_code[0]));
                Serial.println("Time:" + String(micros()-start_time));
                
            } 
        }
        start_time = micros();
        #ifdef USB_TASK_DELAY
        vTaskDelay(USB_TASK_DELAY);
        #endif
        
    }//????????????????????????
    
}

void words_change(uint8_t word_name, bool stat){
    if(stat==0){//??????
        for (int i = 2; i < 8; i++){//????????????
            if (key_code[i]==0x00){
                key_code[i]=word_name;
                    break;
            }  
        } 
    }
    if(stat==1){//??????
        for (int i = 2; i < 8; i++){//????????????
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

#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\keyboard_self.ino"
# include "keyboard_self.h"
# include "poker_keyboard.h"
# include "oled_buff.h"
# include "Layout.h"
# include "Seg_disp.h"


void keyboard_setup(){
  //Joker SETUP
  #ifdef Joker
  //Flash LED
  pinMode(LED_PIN,OUTPUT);
  open_inter_led();
  delay(100);
  close_inter_led();
  
  //OLED Display
  Oled_Start();
  invert_disp(1);
  LineDisp("      Joker 60 by LiuZS      ",3);
  LineDisp("====Started!====",0);
  //LineDisp("                              1.0",4);
  delay(200);
  invert_disp(0);
  LineDisp(" ",3);
  #endif

  //Master Yoda SETUP
  #ifdef MasterYoda
  //INIT DISP
  init_seg();
  #endif
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
  #ifdef Joker
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
  #endif
}

bool cnt_start(){
  #ifdef Joker
  xReturned_cnt = xTaskCreatePinnedToCore(cnt_work, "JOKER CNT WORK", CNT_TASK_STACK, NULL, CNT_TASK_PRI, &CNT_TASK_Handle, CNT_TASK_CORE) ; 
  if(xReturned_cnt == pdPASS){
    return 1;
  }
  return 0;
  #endif
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



#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\oled_buff.ino"
#ifdef Joker
# include "oled_buff.h"
# include "Pic.h"
# define HEIGHT_WORDS 9 

void Oled_Start(void){    // ??????oled??????
  
  display.init();
  display.flipScreenVertically();
  display.setBrightness(OLED_BRIGHT);

}

void LineDisp(String to_print,int line, bool if_clear){
    #ifdef Joker
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
    #endif
}

void invert_disp(bool if_invert){
    #ifdef Joker
    if (!if_invert){//?????????
        if (oled_invert){//?????????
            display.normalDisplay();
            oled_invert = 0;
        }
    }
    else{//??????
        if (!oled_invert){
            display.invertDisplay();
            oled_invert = 1;
        }
    }
    #endif
}

void draw_dog(){
    #ifdef Joker
    if (oled_mode==1){
        display.clear(); //for Clearing the display
        display.drawXbm(0, 0, 128, 64, dog_bmp);
        display.display();
    }
    #endif
}

#endif
#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\pcf8575.ino"
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

#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\poker_bt.ino"
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
??????????????????
*/
void bt_work(void *pvParameters){
  if(DBG_KEYBOARD){
        Serial.println("Starting BLE work!");
  }
  #ifdef Joker
  LineDisp("-------BLE------", ble_line);
  #endif
  vTaskDelay(1000);
  bleKeyboard.begin();//Start blekeyboard service
  Keyboard_Config.BOOT_MODE = 0;
  save_config();
  #ifdef Joker
  LineDisp("Waiting for BLE", ble_line);
  display.drawString(0, ble_line*8 ,"Waiting for BLE");
  #endif
  bool start_flag = 0;
  
//????????????
for (;;){
  if (bleKeyboard.isConnected()){//?????????
    
    bt_stat = 1;

     //??????????????????

    if (!start_flag){
      Serial.println("connected!");
      #ifdef Joker
      display.drawString(0, ble_line*8 ,"<MODE>===========BLE");
      display.display();
      //LineDisp(">========BLE========<", ble_line);
      LineDisp("<MODE>===========BLE", ble_line);
      LineDisp("<FN>-------------------------OFF", fn_line);
      #endif
      Keyboard_Config.BOOT_MODE = 1;
      save_config();
    }
    
    //??????
    key_scan();

    //PN??????
    #ifdef Joker
    //PN??????
    if(pn_stat){//pn?????????
      if ((key_press[LED_ROW][LED_COL]==0)&&(old_key_press[LED_ROW][LED_COL]==1)){ //LED ??????
        if(!LED_STAT){
          open_inter_led();
        }
        else{
          close_inter_led();
        }
      }//LED ??????
      
      //MODE ??????
      if((key_press[MODE_ROW][MODE_COL]==0)&&(old_key_press[MODE_ROW][MODE_COL]==1)){ // ???????????????
        bt_to_change_mode = 1; 
        bt_chang_mode_time = millis();
        LineDisp("<MODE>===========CHG", ble_line);
      }
      if ((key_press[MODE_ROW][MODE_COL]==0)&&bt_to_change_mode&&((millis()-bt_chang_mode_time)>3000)){//???????????????????????????
        bt_to_change_mode = 0;
        joker_bt2usb();
        LineDisp("<MODE>===========LIS", ble_line);
      }
      if(bt_to_change_mode&&(key_press[MODE_ROW][MODE_COL]==1)){//???????????????
        bt_to_change_mode = 0;
        LineDisp("<MODE>===========BLE", ble_line);
      }
      //MODE ??????

      //???????????????
      if ((key_press[cnt_rst_ROW][cnt_rst_COL]==0)&&(old_key_press[cnt_rst_ROW][cnt_rst_COL]==1)){ 
        rst_cnt_time = 1;
      }
      //???????????????
    }
    //PN???????????????
    if(start_flag&&(pn_stat==0)&&(key_press[PN_ROW][PN_COL] == 0)){//PN???????????????
      if (DBG_KEYBOARD){
        Serial.println("PN ON!");
      }
      for (int i = 0; i < 8; i++){
        bleKeyboard.releaseAll();
      }

      pn_stat = 1;
    }//pn???????????????
    //PN???????????????
    if(start_flag&&(pn_stat==1)&&(key_press[PN_ROW][PN_COL] == 1)){//PN???????????????
      if (DBG_KEYBOARD){
        Serial.println("PN OFF!");
      }
      pn_stat = 0;
    }//pn???????????????
    #endif

    // FN ??????????????????
    if (start_flag&&(key_press[FN_ROW][FN_COL]==0)&&(fn_stat==0)){ 
      if (DBG_KEYBOARD){
        Serial.println("FN IS ON !");
      }
      #ifdef Joker
      LineDisp("<FN>--------------------------ON", fn_line);
      #endif
      fn_stat = 1;

      for (int ROW = 0; ROW < number_out; ROW++){//???????????????
        for (int COL = 0; COL < number_in; COL++){//?????????
          if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(LayOut_ALL[ROW][COL]!=LayOut_ALL_FN[ROW][COL])){//????????????????????????????????????fn????????????????????????
            bleKeyboard.release(LayOut_ALL[ROW][COL]);
            bleKeyboard.press(LayOut_ALL_FN[ROW][COL]);
          }
        }
      }
    }//fn?????????????????????

    if (start_flag&&(key_press[FN_ROW][FN_COL]==1)&&(fn_stat==1)){ // FN ??????????????????
      if (DBG_KEYBOARD){
        Serial.println("FN IS OFF !");
      }
      #ifdef Joker
      LineDisp("<FN>-------------------------OFF", fn_line);
      #endif
      fn_stat = 0;

      for (int ROW = 0; ROW < number_out; ROW++){//???????????????
        for (int COL = 0; COL < number_in; COL++){//?????????
          if((old_key_press[ROW][COL]==0)&&(key_press[ROW][COL]==0)&&(LayOut_ALL[ROW][COL]!=LayOut_ALL_FN[ROW][COL])){//????????????????????????????????????fn????????????????????????
            bleKeyboard.release(LayOut_ALL_FN[ROW][COL]);
            bleKeyboard.press(LayOut_ALL[ROW][COL]);
          }
        }
      }
    }//fn?????????????????????
    
    for (int ROW = 0; ROW < number_out; ROW++){//???????????????
      for (int COL = 0; COL < number_in; COL++){//?????????
          
        //??????????????????
        if ((!pn_stat)&&start_flag && ( (old_key_press[ROW][COL]!=key_press[ROW][COL]))){ 
          if (DBG_KEYBOARD){
            Serial.println("***["+ String(ROW) + "," + String(COL) + "]:" + String(key_press[ROW][COL])+ "means:" + String(LayOut_ALL[ROW][COL]) );
          }

          //????????????
          if (LayOut_ALL[ROW][COL]!=0){
            if (key_press[ROW][COL]==0){//????????????????????????
              if (fn_stat){
                bleKeyboard.press(LayOut_ALL_FN[ROW][COL]);
              }
              else{
                bleKeyboard.press(LayOut_ALL[ROW][COL]);
              }
            }
            if(key_press[ROW][COL]==1){//??????????????????
              if (fn_stat){
                bleKeyboard.release(LayOut_ALL_FN[ROW][COL]);
              }
              else{
                bleKeyboard.release(LayOut_ALL[ROW][COL]);
              }
            } 

          }//????????????
            
        }//????????????????????????
        
      }//?????????
    }//?????????   
    
      //????????????
        for (int i = 0; i < number_out; i++){
            for (int j = 0; j < number_in; j++){
                if (old_key_press[i][j]==key_press[i][j]){
                     old_key_press[i][j] = key_press[i][j]; //??????????????????????????????
                }
                old_key_press[i][j] = key_press[i][j]; //???????????????????????????????????????
            } 
        }//??????????????????

        
      if (!start_flag){
      start_flag = 1;
      for (int i = 0; i < number_out; i++){
            for (int j = 0; j < number_in; j++){
                old_key_press[i][j] = key_press[i][j]; //??????????????????????????????
                old_key_press[i][j] = key_press[i][j]; //???????????????????????????????????????
            } 
        }//??????????????????
    }

  } //?????????
  else{
    delay(10);//?????????????????????
  }
  #ifdef BT_TASK_DELAY
        vTaskDelay(BT_TASK_DELAY);
        #endif

}//????????????????????????

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

#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\save.ino"
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


#line 1 "d:\\Data\\esp32_keyboard\\Keyboard_code\\poker_keyboard\\scan.ino"
#include "scan.h"

void scan_start()
{
  //?????????????????????
  for (int i = 0; i < number_in; i++)
  {
    pinMode(key_pin_in[i], INPUT_PULLUP);
  }
  //?????????????????????
  for (int j = 0; j < number_out; j++)
  {
    pinMode(key_pin_out[j], OUTPUT);
    digitalWrite(key_pin_out[j], HIGH);
  }
}

void key_scan_once(){
  //??????
  for (int ROW = 0; ROW < number_out; ROW++)
  {
    //?????????
    if (ROW == 0)
    {
      digitalWrite(key_pin_out[(number_out - 1)], HIGH); //??????????????????????????????????????????
      digitalWrite(key_pin_out[ROW], LOW);
    }
    else
    {
      digitalWrite(key_pin_out[(ROW - 1)], HIGH); //?????????????????????????????????????????????
      digitalWrite(key_pin_out[ROW], LOW);
    }
    delayMicroseconds(10);
    //????????????
    for (int COL = 0; COL < number_in; COL++)
    {
      key_press[ROW][COL] += digitalRead(key_pin_in[COL]);
    } //??????????????????

  } //????????????
}

void key_scan(){
  //??????
  for (int ROW = 0; ROW < number_out; ROW++){
    for (int COL = 0; COL < number_in; COL++){
      key_press[ROW][COL] = 0;
    } 
  } 

  //??????
  for (int i=0; i<3; i++){ //????????????
    key_scan_once();
  } //????????????

  //??????
  for (int ROW = 0; ROW < number_out; ROW++){
    for (int COL = 0; COL < number_in; COL++){
      key_press[ROW][COL] =  key_press[ROW][COL]/2;
    } 
  }//????????????
  
}
