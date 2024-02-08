#include <Arduino.h>
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

IRAM_ATTR void handleInterrupt();

LiquidCrystal_I2C lcd(0x27, 16, 2);
int state;
int sw1 = D3,sw2 = D4;

const int START = 1;
const int TIME_START = 2;
const int TIME_STOP = 3;
const int TIME_SAVE = 4;
const int TIME_RESET = 5;

unsigned long start = 0,count = 0,sec = 0,minute = 0,milli = 0;
String time_count,time_save;
int press = false;

void setup(){
  lcd.init();
  Wire.begin();
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  attachInterrupt(digitalPinToInterrupt(D3),handleInterrupt,FALLING);
  attachInterrupt(digitalPinToInterrupt(D4),handleInterrupt,FALLING);
  lcd.backlight();
  state = START;
}

void loop(){
  if (state == START){
    lcd.setCursor(2, 0);
    lcd.print("Press button 1");
    lcd.setCursor(4, 1);
    lcd.print("to start");
    if(press == true){
      press = false;
      if (digitalRead(sw1) == 0){
        state = TIME_START;
        lcd.clear();
        start = millis();
      }
    }
  }
  if (state == TIME_START){
    lcd.setCursor(0, 0);
    lcd.print("B1:SAVE ");
    lcd.setCursor(0, 1);
    lcd.print("B2:STOP ");
    count = millis() - start;
    minute = (count / 60000) % 60;
    sec = (count / 1000) % 60; 
    milli = count % 1000;
    time_count = String(minute) + ":" + String(sec) + ":" + milli;
    lcd.setCursor(9, 0);
    lcd.print(time_count);
    Serial.println(1);
    if(press == true){
      press = false;
      if (digitalRead(sw1) == 0){
        state = TIME_SAVE;
      }
      if (digitalRead(sw2) == 0){
        state = TIME_STOP;
      }
    }
  }
  if (state == TIME_STOP){
    lcd.setCursor(0, 0);
    lcd.print("B1:START");
    lcd.setCursor(0, 1);
    lcd.print("B2:RESET");
    start = millis() - count;
    if(press == true){
      press = false;
      if (digitalRead(sw1) == 0){
        state = TIME_START;
      }
      if (digitalRead(sw2) == 0){
        state = TIME_RESET;
        lcd.clear();
      }
    }
  }
  if (state == TIME_SAVE){
    time_save = time_count;
    lcd.setCursor(9, 1);
    lcd.print(time_save);
    state = TIME_START;
  }
  if (state == TIME_RESET){
    start = 0;
    count = 0;
    sec = 0;
    minute = 0;
    milli = 0;
    time_count = "";
    time_save = "";
    state = START;
  }
}

IRAM_ATTR void handleInterrupt(){
  press = true;
}
