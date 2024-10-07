// input serial monitor dd mm yyyy hh mm ss

#include "MD_Parola.h"
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"
#include <DS3231.h>
#include <Wire.h>
#include <PS2Keyboard.h>
DS3231 Clock;

PS2Keyboard keyboard;
const int key_pin = 2;
const int IRQpin = 3;

bool Century = false;
bool h12;
bool PM;
byte dd, mm, yyy;
uint16_t h, m, s;

#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define SPEED_TIME 35
#define PAUSE_TIME 0

#define MAX_MESG 100

#define temperature_pin A1
float temperature = 0.0;
float lm35_adj = 20.0;

int LDR_PIN = A0;
//int AnalogValue;

#define buzzer_pin 8

char szTime[9];
char szMesg[MAX_MESG + 1] = "";

//alarm
int A1_hour = 9;
int A1_min = 00;

int A2_day = 8;
int A2_hour = 9;
int A2_min = 36;

int A3_day = 8;
int A3_hour = 9;
int A3_min = 46;

//input text
String text = "Ini Alarm 3";

void getTime(char *psz, bool f = true) {
  s = Clock.getSecond();
  m = Clock.getMinute();

  sprintf(psz, "%02d%c%02d%c%02d", h, (f ? ':' : ' '), m, (f ? ':' : ' '), s);
  //sprintf(psz, "%02d:%02d:%02d", h, m, s);
  if (Clock.getHour(h12, PM) >= 13 || Clock.getHour(h12, PM) == 0) {
    h = Clock.getHour(h12, PM);
    //h = Clock.getHour(h12,PM) - 12;
  } else {
    h = Clock.getHour(h12, PM);
  }
}

void getDate(char *psz) {
  //char szBuf[10];

  dd = Clock.getDate();
  mm = Clock.getMonth(Century);
  yyy = Clock.getYear();
  sprintf(psz, "%02d%c%02d%c%02d", dd,'.', mm,'.', (yyy - 48));
  //Serial.println(yyy - 48);
}

void displayTemperature(char *psz) {
  float value = analogRead(temperature_pin) * (5.0 / 1024.0);
  temperature = (value * 100.0);
  temperature = temperature - lm35_adj;
  //Serial.print("Temperature : ");
  //Serial.println(temperature);

  char tempString[10];
  dtostrf(temperature, 0, 2, tempString);
  sprintf(psz, "%s c", tempString);

  temperature = 0;
}

void getName(char *psz) {
  snprintf(psz, MAX_MESG, "Ridho Walid Al Maulidi 5024201036");
  //Serial.println("Nama");
}

void getNRP(char *psz) {
  snprintf(psz, MAX_MESG, "5024201036");
}

void getText(char *psz)
{
  snprintf(psz, MAX_MESG, "%s",text.c_str());
}

void Alarm_1(int hour, int minute) {
  h = Clock.getHour(h12, PM);
  m = Clock.getMinute();
  s = Clock.getSecond();

  if (h == hour && m == minute) {
    //P.setFont(0, 0);
    //P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    //P.setPause(0, 0);
    //getNRP(szMesg);
    buzzer();
  }
}

void Alarm_2(int day, int hour, int minute) {
  dd = Clock.getDate();
  h = Clock.getHour(h12, PM);
  m = Clock.getMinute();

  if (h == hour && m == minute && dd == day) {
    P.setFont(0, 0);
    P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    P.setPause(0, 0);
    getName(szMesg);
    //buzzer();
  }
}

void Alarm_3(int day, int hour, int min)
{
  dd = Clock.getDate();
  h = Clock.getHour(h12, PM);
  m = Clock.getMinute();

  if (h == hour && m == min && dd == day) {
    P.setFont(0, 0);
    P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    P.setPause(0, 0);
    getText(szMesg);
}
}

void adjustDate(String data) {
  byte _day = data.substring(0, 2).toInt();
  byte _month = data.substring(3, 5).toInt();
  int _year = data.substring(6, 10).toInt();

  //Clock.setA1Time(_day, _hour, _min, _sec, );
  //Clock.setDate(16, 12, 2022);
  Clock.setDate(_day);
  Clock.setMonth(_month);
  Clock.setYear(_year);
  
  //Serial.println(_day);
  //Serial.println(_year);
  Serial.println(F(">>Date successfully set!"));
}

void adjustTime(String data) {
  byte _hour = data.substring(11, 13).toInt();
  byte _min = data.substring(14, 16).toInt();
  byte _sec = data.substring(17, 19).toInt();

  //Clock.setA1Time(_day, _hour, _min, _sec, );
  //Clock.setDate(16, 12, 2022);
  Clock.setHour(_hour);
  Clock.setMinute(_min);
  Clock.setSecond(_sec);
  
  //Serial.println(_hour);
  //Serial.println(_min);
  Serial.println(F(">> Time successfully set!"));
}

void buzzer()
{
  tone(buzzer_pin,500);
  delay(1000);
  noTone(buzzer_pin);
  delay(2000);
}

void setup(void) {
  //Serial.begin(9600);
  delay(3000);
  P.begin(2);
  P.setInvert(false);
  Wire.begin();

  //P.setIntensity(0);
  pinMode(temperature_pin, INPUT);
  analogRead(temperature_pin);
  P.setFont(0, 0);

  pinMode(A0, INPUT);
  Serial.begin(9600);

  pinMode(buzzer_pin, OUTPUT);

  keyboard.begin(key_pin,IRQpin);

  P.setZone(0, MAX_DEVICES - 4, MAX_DEVICES - 1);

  P.setZone(1, MAX_DEVICES - 4, MAX_DEVICES - 1);
  P.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);

  P.displayZoneText(0, szMesg, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  //P.displayZoneText(1, szMesg, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
}

void loop(void) {
  static uint32_t lastTime = 0;
  //static int lastTime = 0;
  //static uint8_t  display = 0;
  static int display = 0;
  static int key_display = 0;
  //Serial.print(display);
  static bool flasher = false;

  static int AnalogValue = 0;
  AnalogValue = analogRead(A0);
  //Serial.println(AnalogValue);
  if (AnalogValue < 220) {
    //AnalogValue = AnalogValue / 10;
    P.setIntensity(15);
    //Serial.println("LDR : 1");
    //Serial.println(AnalogValue);
  } else if (AnalogValue >= 220 && AnalogValue < 700) {
    P.setIntensity(4);
    //Serial.println("LDR : 2");
  } else if (AnalogValue >= 700) {
    //AnalogValue = AnalogValue/100;
    P.setIntensity(0);
    //Serial.println("LDR : 3");
    //Serial.println(AnalogValue);
  }

  Alarm_1(A1_hour, A1_min);//hh,mm
  Alarm_2(A2_day, A2_hour, A2_min);//dd,hh,mm
  Alarm_3(A3_day, A3_hour, A3_min);

  if (Serial.available() > 0) {
    String data = Serial.readString();
    adjustDate(data);
    adjustTime(data);
  }

  s = Clock.getSecond();

  static int index = 0;
  static int zone = 0;
  if (keyboard.available()) {
    
    // read the next key
    char c = keyboard.read();
    if (c == PS2_DOWNARROW && zone == 1)
    {
    Serial.println("DOWN");
    if (key_display == 5)
      key_display = key_display;
    else
      key_display++;
  }
  else if(c == PS2_UPARROW && zone == 1)
  {
    Serial.println("UP");
    if (key_display == 1)
      key_display = key_display;
    else
      key_display--;
  }

  else if (c == PS2_ENTER)
  {
    Serial.println("ENTER");
    zone = 1;
    key_display = 0;
  }
  else if (c == PS2_ESC)
  {
    Serial.println("ESC");
    zone = 0;
    display = 1;
    index = 0;
  }
  else
  {
    int max;
    char input[100];
    if (key_display == 1)
    {
      max = 6;
      input[index] = c; // "c" merupakan input dari keyboard
      Serial.print(input[index]);
      //Serial.println(index);
      index++;
      
      if(index == max)
      {
        int inputInt[max];
        for (int i = 0; i<max ; i++)
        {
          if (i %2 == 0)
          {
            inputInt[i] = input[i] - '0';
            inputInt[i] *= 10;
            //Serial.println(inputInt[i]);
          }
          else
          {
            inputInt[i] = input[i] - '0';
            //Serial.println(inputInt[i]);
          }
        }
        //Serial.println("input =");
        /*for (int i = 0; i<max ;i++)
        {
          Serial.println(input[i]);
        }*/
        int hour = inputInt[0]+inputInt[1];
        int min = inputInt[2]+inputInt[3];
        int sec = inputInt[4]+inputInt[5];
        Serial.println("");
        Serial.print("jam = ");
        Clock.setHour(hour);
        Clock.setMinute(min);
        Clock.setSecond(sec);
        Serial.print(hour);
        Serial.print(min);
        Serial.print(sec);
        index = 0;
      }
    }

    else if (key_display == 2)
    {
      max = 6;
      input[index] = c; // "c" merupakan input dari keyboard
      Serial.print(input[index]);
      //Serial.println(index);
      index++;
      
      if(index == max)
      {
        int inputInt[max];
        for (int i = 0; i<max ; i++)
        {
          if (i %2 == 0)
          {
            inputInt[i] = input[i] - '0';
            inputInt[i] *= 10;
            //Serial.println(inputInt[i]);
          }
          else
          {
            inputInt[i] = input[i] - '0';
            //Serial.println(inputInt[i]);
          }
        }
        //Serial.println("input =");
        /*for (int i = 0; i<max ;i++)
        {
          Serial.println(input[i]);
        }*/
        int day = inputInt[0]+inputInt[1];
        int month = inputInt[2]+inputInt[3];
        int year = inputInt[4]+inputInt[5] + 2000;
        Serial.println("");
        Serial.print("Date = ");
        Clock.setDate(day);
        Clock.setMonth(month);
        Clock.setYear(year);
        Serial.print(day);
        Serial.print(month);
        Serial.print(year);
        index = 0;
      }
    }
    else if (key_display == 3) //Alarm 1
    {
      max = 4;
      input[index] = c; // "c" merupakan input dari keyboard
      Serial.print(input[index]);
      //Serial.println(index);
      index++;
      
      if(index == max)
      {
        int inputInt[max];
        for (int i = 0; i<max ; i++)
        {
          if (i %2 == 0)
          {
            inputInt[i] = input[i] - '0';
            inputInt[i] *= 10;
            //Serial.println(inputInt[i]);
          }
          else
          {
            inputInt[i] = input[i] - '0';
            //Serial.println(inputInt[i]);
          }
        }
        //Serial.println("input =");
        /*for (int i = 0; i<max ;i++)
        {
          Serial.println(input[i]);
        }*/
        A1_hour = inputInt[0]+inputInt[1];
        A1_min = inputInt[2]+inputInt[3];
        Serial.println("");
        Serial.print("Alarm 1 = ");
        Serial.print(A1_hour);
        Serial.print(A1_min);
        key_display = 7;
        //Alarm_1(A1_hour,A1_min);
        index = 0;
      }
    }
    else if (key_display == 4) //Alarm 2
    {
      max = 6;
      input[index] = c; // "c" merupakan input dari keyboard
      Serial.print(input[index]);
      //Serial.println(index);
      index++;
      
      if(index == max)
      {
        int inputInt[max];
        for (int i = 0; i<max ; i++)
        {
          if (i %2 == 0)
          {
            inputInt[i] = input[i] - '0';
            inputInt[i] *= 10;
            //Serial.println(inputInt[i]);
          }
          else
          {
            inputInt[i] = input[i] - '0';
            //Serial.println(inputInt[i]);
          }
        }
        //Serial.println("input =");
        /*for (int i = 0; i<max ;i++)
        {
          Serial.println(input[i]);
        }*/
        A2_day = inputInt[0]+inputInt[1];
        A2_hour = inputInt[2]+inputInt[3];
        A2_min = inputInt[4]+inputInt[5];
        Serial.println("");
        Serial.print("Alarm 2 = ");
        Serial.print(A2_day);
        Serial.print(A2_hour);
        Serial.print(A2_min);
        //Alarm_2(A2_day,A2_hour,A2_min);
        key_display = 7;
        index = 0;
      }
    }
    else if (key_display == 5) //text
    {
      max = 100;
      input[index] = c; // "c" merupakan input dari keyboard
      Serial.print(input[index]);
      //Serial.println(index);
      if (c == PS2_RIGHTARROW)
      {
        max = index;
      }
      index++;
      

      if(index >= max)
      {
        String inputString = "";
        for (int i = 0; i<max ; i++)
        {
          inputString += input[i];
        }
        Serial.println("");
        Serial.print("Text = ");
        Serial.print(inputString);
        text = inputString;
        key_display = 6;
        index = 0;
      }
    }
  }
  } 
  P.displayAnimate();
  if (P.getZoneStatus(0)) {
    if (zone == 0)
    {
      /*
      Serial.println("");
      Serial.println("Alarm 1");
      Serial.print(A1_hour);
      Serial.print(", ");
      Serial.print(A1_min);
      Serial.println("");

      Serial.println("Alarm 2");
      Serial.print(A2_day);
      Serial.print(", ");
      Serial.print(A2_hour);
      Serial.print(", ");
      Serial.print(A2_min);*/
      switch (display) {
      case 0:
        P.setFont(0, 0);
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        P.setPause(0, 0);
        getName(szMesg);
        //getNRP(szMesg);
        display++;
        break;

      case 1:
        //Serial.println(display);
        P.setFont(0, numeric7Seg);
        P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
        P.setPause(0, 0);
        if (millis() - lastTime >= 1000) {
          lastTime = millis();
          getTime(szMesg, flasher);
          flasher = !flasher;
        }
        if (s == 10 || s == 40) {
          //P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          //getDate(szMesg);
          display++;
          P.setTextEffect(0, PA_PRINT, PA_SCROLL_UP);
          break;
        }
        break;
      case 2:
        //Serial.println(display);
        P.setFont(0, numeric7Seg);
        P.setTextEffect(0, PA_PRINT, PA_SCROLL_UP);
        //P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
        P.setPause(0, 3000);
        getDate(szMesg);
        display++;
        break;

      case 3:
      //Serial.println(display);
        P.setFont(0, numeric7Seg);
        temperature = 0;
        P.setTextEffect(0, PA_PRINT, PA_SCROLL_UP);
        P.setPause(0, 3000);
        displayTemperature(szMesg);

        display = 1;
        break;
      }
    }
    
    else if (zone == 1)
    {
      switch (key_display)
      {
        case 0 :
          P.displayClear(0);
          P.setFont(0, 0);
          P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          P.setPause(0, 0);
          snprintf(szMesg, MAX_MESG, "Keyboard Detected");
          //Serial.println(key_display);
          break;

        case 1 :
          P.setFont(0, 0);
          P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
          P.setPause(0, 1000);
          snprintf(szMesg, MAX_MESG, "Time");
          break;

        case 2 :
          P.setFont(0, 0);
          P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
          P.setPause(0, 1000);
          snprintf(szMesg, MAX_MESG, "Date");
          //Serial.println(key_display);
          break;

        case 3 :
          P.setFont(0, 0);
          P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
          P.setPause(0, 1000);
          snprintf(szMesg, MAX_MESG, "Alarm 1");
          //Serial.println(key_display);
          break;

        case 4 :
          P.setFont(0, 0);
          P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
          P.setPause(0, 1000);
          snprintf(szMesg, MAX_MESG, "Alarm 2");
          //Serial.println(key_display);
          break;

        case 5 :
          P.setFont(0, 0);
          P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
          P.setPause(0, 1000);
          snprintf(szMesg, MAX_MESG, "Text");
          //Serial.println(key_display);
          break;

        case 6:
          P.displayClear(0);
          P.setFont(0,0);
          P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          P.setPause(0,0);
          snprintf(szMesg, MAX_MESG, "%s",text.c_str()); // menampilkan semua text
          //key_display = 0;
          break;
        case 7:
        P.displayClear(0);
        P.setFont(0,0);
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        P.setPause(0,0);
        snprintf(szMesg, MAX_MESG, "Alarm Set Success!!");
        key_display = 1;
        break;
      }
    }
    P.displayReset(0);
  }  
}