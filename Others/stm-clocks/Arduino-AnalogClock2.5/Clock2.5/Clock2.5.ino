/***************************************************
 Title: Analog Clock for Arduino Uno
 Author: Ceez
 Version: 2.4
 Update: 2016/09/09

 Modules:
 - DS1307 RTC
 - TFT display 240x320 using driver ILI9341
 - Arduino Uno or Arduino ProMini

 Description:
 - Update Reset function and auto reset at at 5s
 - Display an analog clock on the TFT display
 - Display Word Clock right below the analog clock
 - Config time with using Serial communication
    + Format: u2015/7/24 01:57:01u
        'u' to mark the start, end of update command
    + Format: [Y/M/D/h/m/s] [value]
        update hour/minute seperately: h 23
        e.g: Y 2015
             h 23
             D 29

 Wiring:
 - RTC module     Arduino Uno
       SCL    -    A5
       SDA    -    A4
       VCC    -    5v
       GND    -    GND

 - TFT display     Arduino Uno
       SDO    -    Digital 12 (MISO)
       LED    -    3.3v
       SCK    -    Digital 13
       SDI    -    Digital 11 (MOSI)
       D/C    -    Digital 9
       RESET  -    3.3v or RST
       CS     -    Digital 10
       GND    -    GND
       VCC    -    3.3v (Read note below)
       
       
 Note:

 - Since Arduino signal output at 5v and TFT display
 accept input at 3v3, we need to use a levelshifter to
 convert 5v signal to 3v signal in between Arduino and
 TFT display on these pins: SCK, SDI, D/C, CS. You
 don't need to worry about that if you use Arduino
 ProMini at 3.3v, but DS1307 should run at 5v according
 to DS1307 datasheet to play nice with backup battery.
 Anyway, it could run without problems with 3v3.

 - Require library DS1307RTC, Time, Adafruit_ILI9341.

 - Won't work with LGPD4535 TFT Shield, because it use 
 A4 and A5 for CS and Reset of the TFT, the same pin for
 SDA and SCL of DS1307. Thus, when it updates time from
 DS1307, it also resets the TFT and make it WHITE SCREEN.
 
 /****************************************************/
 
/*
#define sclk 13  // Don't change
#define mosi 11  // Don't change
#define cs   10
#define dc   9
*/

#define TME tmElements_t

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <DS3232RTC.h>
#include "struct.h"
#include <fonts/FreeMono9pt7b.h>

#define LOAD_FONT2

#define BLACK           0x0000      /*   0,   0,   0 */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DARKGREEN       0x03E0      /*   0, 128,   0 */
#define DARKCYAN        0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY       0xC618      /* 192, 192, 192 */
#define DARKGREY        0x7BEF      /* 128, 128, 128 */
#define BLUE            0x001F      /*   0,   0, 255 */
#define GREEN           0x07E0      /*   0, 255,   0 */
#define CYAN            0x07FF      /*   0, 255, 255 */
#define RED             0xF800      /* 255,   0,   0 */
#define MAGENTA         0xF81F      /* 255,   0, 255 */
#define YELLOW          0xFFE0      /* 255, 255,   0 */
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define PINK            0xF81F

#define CREF_BACKGROUND BLACK
#define FACE            ORANGE
#define CREF_SECOND     RED
#define CREF_MINUTE     CYAN
#define CREF_HOUR       CYAN
#define NUMERIC_POINT   WHITE
#define CREF_HELLO      RED
#define CREF_TEXT       DARKCYAN
#define CREF_TEXT_BRAND ORANGE
#define CREF_TIME       MAGENTA
#define CREF_DATE       GREENYELLOW

#define MINUTE_HAND  0
#define HOUR_HAND  1

// CONFIG YOUR CHANGE HERE
#define Xo 159   // center point
#define Yo 119   // center point
#define RADIUS 90  // radius of the clock face

#define S_LEN  75  // second hand
#define S_TAIL  18

#define M_LEN  72  // minute hand
#define M_TAIL  15
#define M_SIDE  5

#define H_LEN  50  // hour hand
#define H_TAIL  12
#define H_SIDE  7

#define TEXT_SIZE 1

char digit_ch[3]={'0','0',NULL}; // a buffer for convert number to char array

#define SCLK 13  // Don't change
#define MOSI 11  // Don't change
#define MISO 12  // Don't change
#define CS 10
#define DC 9
#define RST -1 // connect to Reset of arduino or 3v3

// If it reports error here, change this constructor to fit which your library you are using.
//Adafruit_ILI9341 tft = Adafruit_ILI9341(CS,  DC, MOSI, SCLK, RST, MISO); // Invoke standard library from adafruit
Adafruit_ILI9341 tft = Adafruit_ILI9341(CS, DC, RST); // use HW SPI

DS3232RTC DS3231;

HAND_SET o_hands;
HAND_SET n_hands;

TME o_tme;
TME n_tme;
  
//dummy function
void setup() {
}
void(* resetFunc) (void) = 0; //declare reset function @ address 0
/** -------- Functions -------- **/

// convert minute from INT to CHAR
void conv_num2char(int min)
{
  char digit[10] = {'0','1','2','3','4','5','6','7','8','9'};
  char* ret;
  digit_ch[0] = '0';
  digit_ch[1] = '0';
  //digit_ch[2]=NULL;
  if (min > 59) return;
  digit_ch[0]=digit[min/10]; 
  digit_ch[1]= digit[min - min/10*10];
  digit_ch[2]=NULL;
//  ret[2[= NULL;
  //return ret;
}


/****    ANALOG WORK    ****/
void drawBrand()
{
  tft.setTextColor(CREF_TEXT_BRAND);
  tft.setTextSize(1);
  tft.setCursor(Xo-25, 70);
  tft.print("OMEGA");
}

// Draw Clock Face
void drawFace()
{
  int i = 0, angle = 0;
  float x, y;

  // Draw outer frame
  tft.drawCircle(Xo, Yo, RADIUS + 21, FACE);
  tft.drawCircle(Xo, Yo, RADIUS + 20, FACE);

  // Draw inner frame
  tft.drawCircle(Xo, Yo, RADIUS + 12, FACE);
  tft.drawCircle(Xo, Yo, RADIUS + 11, FACE);
  tft.drawCircle(Xo, Yo, RADIUS + 10, FACE);

  //Draw Numeric point

  for (i = 0; i <= 12; i++) {
    x = Xo + RADIUS * cos(angle * M_PI / 180);
    y = Yo + RADIUS * sin(angle * M_PI / 180);
    tft.drawCircle(x, y, 2, NUMERIC_POINT);
    angle += 30;
  }

  for (i = 0; i < 360; i += 6) {
    tft.drawPixel(Xo + RADIUS * cos(i * M_PI / 180), Yo + RADIUS * sin(i * M_PI / 180), NUMERIC_POINT);
  }
}

void calc_SecondHand(TME t, LINE &ps )
{
  float angle; // in radian
  //  int Xa, Ya, Xb, Yb;
  angle =  t.Second * 0.1044 - 1.566;
  ps.a.x = Xo + (S_LEN) * cos(angle);
  ps.a.y = Yo + (S_LEN) * sin(angle);
  angle += 3.142; // +180 degree
  ps.b.x = Xo + (S_TAIL) * cos(angle);
  ps.b.y = Yo + (S_TAIL) * sin(angle);
}

// type = 0 -> Minute
// type = 1 -> Hour
void calc_HourMinHand(uint8_t hour_hand, TME t, HAND_POINTS &ps)
{
  float angle;
  if (hour_hand == HOUR_HAND) // 1 == hour, 0 == minute
    angle = t.Hour * 0.524 + t.Minute * 0.0087 - 1.571; // (theta + h*30 + (m*30/60))* M_PI/180
  else
    angle = t.Minute * 0.1044 - 1.571;  // (theta + n_tme.Minute*6)*M_PI/180

  ps.a.x = Xo + ((hour_hand) ? H_LEN : M_LEN) * cos(angle);
  ps.a.y = Yo + ((hour_hand) ? H_LEN : M_LEN) * sin(angle);
  angle += 3.142; //+180 degree to get the tail
  ps.b.x = Xo + ((hour_hand) ? H_TAIL : M_TAIL) * cos(angle);
  ps.b.y = Yo + ((hour_hand) ? H_TAIL : M_TAIL) * sin(angle);
  angle += 1.571; //+90 degree to get the side point
  ps.e.x = Xo + ((hour_hand) ? H_SIDE : M_SIDE) * cos(angle);
  ps.e.y = Yo + ((hour_hand) ? H_SIDE : M_SIDE) * sin(angle);
  angle += 3.142; //+180 degree to get other side point
  ps.f.x = Xo + ((hour_hand) ? H_SIDE : M_SIDE) * cos(angle);
  ps.f.y = Yo + ((hour_hand) ? H_SIDE : M_SIDE) * sin(angle);
}

void draw_NewHand(HAND_POINTS ps, int color)
{
  tft.drawLine(ps.a.x, ps.a.y, ps.e.x, ps.e.y, color);
  tft.drawLine(ps.a.x, ps.a.y, ps.f.x, ps.f.y, color);
  tft.drawLine(ps.b.x, ps.b.y, ps.e.x, ps.e.y, color);
  tft.drawLine(ps.b.x, ps.b.y, ps.f.x, ps.f.y, color);
}

void draw_NewMinuteHand()
{
  draw_NewHand(n_hands.Min, CREF_MINUTE);
}

void draw_NewHourHand()
{
  draw_NewHand(n_hands.Hour, CREF_HOUR);
}

void draw_NewSecondHand()
{
  tft.drawLine(n_hands.Sec.a.x, n_hands.Sec.a.y, n_hands.Sec.b.x, n_hands.Sec.b.y, CREF_SECOND);
  tft.fillCircle(n_hands.Sec.b.x, n_hands.Sec.b.y, 2, CREF_SECOND);
}

void cdraw_SecondHand()
{

  tft.fillCircle(o_hands.Sec.b.x, o_hands.Sec.b.y, 2, CREF_BACKGROUND); 
  tft.fillCircle(n_hands.Sec.b.x, n_hands.Sec.b.y, 2, CREF_SECOND);
  tft.drawLine(o_hands.Sec.a.x, o_hands.Sec.a.y, o_hands.Sec.b.x, o_hands.Sec.b.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Sec.a.x, n_hands.Sec.a.y, n_hands.Sec.b.x, n_hands.Sec.b.y, CREF_SECOND);
  tft.fillCircle(n_hands.Sec.b.x, n_hands.Sec.b.y, 2, CREF_SECOND);
}

void cdraw_MinuteHand()
{
  tft.drawLine(o_hands.Min.b.x, o_hands.Min.b.y, o_hands.Min.f.x, o_hands.Min.f.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Min.b.x, n_hands.Min.b.y, n_hands.Min.f.x, n_hands.Min.f.y, CREF_MINUTE);
  
  tft.drawLine(o_hands.Min.b.x, o_hands.Min.b.y, o_hands.Min.e.x, o_hands.Min.e.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Min.b.x, n_hands.Min.b.y, n_hands.Min.e.x, n_hands.Min.e.y, CREF_MINUTE);

  
  tft.drawLine(o_hands.Min.a.x, o_hands.Min.a.y, o_hands.Min.e.x, o_hands.Min.e.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Min.a.x, n_hands.Min.a.y, n_hands.Min.e.x, n_hands.Min.e.y, CREF_MINUTE);
  
  tft.drawLine(o_hands.Min.a.x, o_hands.Min.a.y, o_hands.Min.f.x, o_hands.Min.f.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Min.a.x, n_hands.Min.a.y, n_hands.Min.f.x, n_hands.Min.f.y, CREF_MINUTE);
}

void cdraw_HourHand()
{
  tft.drawLine(o_hands.Hour.b.x, o_hands.Hour.b.y, o_hands.Hour.f.x, o_hands.Hour.f.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Hour.b.x, n_hands.Hour.b.y, n_hands.Hour.f.x, n_hands.Hour.f.y, CREF_HOUR);
  
  tft.drawLine(o_hands.Hour.b.x, o_hands.Hour.b.y, o_hands.Hour.e.x, o_hands.Hour.e.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Hour.b.x, n_hands.Hour.b.y, n_hands.Hour.e.x, n_hands.Hour.e.y, CREF_HOUR);

  
  tft.drawLine(o_hands.Hour.a.x, o_hands.Hour.a.y, o_hands.Hour.e.x, o_hands.Hour.e.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Hour.a.x, n_hands.Hour.a.y, n_hands.Hour.e.x, n_hands.Hour.e.y, CREF_HOUR);
  
  tft.drawLine(o_hands.Hour.a.x, o_hands.Hour.a.y, o_hands.Hour.f.x, o_hands.Hour.f.y, CREF_BACKGROUND);
  tft.drawLine(n_hands.Hour.a.x, n_hands.Hour.a.y, n_hands.Hour.f.x, n_hands.Hour.f.y, CREF_HOUR);
}

void calc_Hands(HAND_SET &hs, TME t)
{
  calc_SecondHand(t, hs.Sec);
  calc_HourMinHand(MINUTE_HAND, t, hs.Min);
  calc_HourMinHand(HOUR_HAND, t, hs.Hour);
}
/****    FINISH ANALOG WORK    ****/


// Print Date in format DD-MM-YYYY
void printDate(int d, int m, int y, int color)
{
  tft.setTextColor(color); //CREF_DATE);
  tft.setCursor(Xo-48, Yo+60);
  tft.print(d); tft.print('-');
  tft.print(m); tft.print('-');
  tft.print(y+1970); //1970 is the first year
}

void TextHour(int number, int color)
{
  tft.setCursor(Xo-38, Yo+35);
  conv_num2char(number);
  tft.setTextColor(color);
  tft.print(digit_ch);
}

void TextMinute(int number, int color)
{
  tft.setCursor(Xo-10, Yo+35);
  conv_num2char(number);
  tft.setTextColor(color);
  tft.print(digit_ch);
}

void TextSecond(int number, int color)
{
  tft.setCursor(Xo+20, Yo+35);
  conv_num2char(number);
  tft.setTextColor(color);
  tft.print(digit_ch);
}

void showTimeColon()
{
  tft.setTextColor(CREF_TIME);
  tft.setCursor(Xo-18, Yo+35);
  tft.print(":");
  tft.setCursor(Xo+12, Yo+35);
  tft.print(":");
}

void serialUpdateTime(TME tm)
{
  DS3231.write(tm);   // set the time, also start RTC
  DS3231.read(tm);
  Serial.print("\nSuccess!\nNew date/time is "); //use print for human-readable ASCII text
  Serial.print(tm.Year + 1970); //use print for human-readable ASCII text
  Serial.print("/");
  Serial.print(tm.Month);
  Serial.print("/");
  Serial.print(tm.Day);
  Serial.print(" ");
  Serial.print(tm.Hour);
  Serial.print(":");
  Serial.print(tm.Minute);
  Serial.print(":");
  Serial.print(tm.Second);
  Serial.print("\n++++");
  Serial.print(0); //null character to comply with null terminate C string
}

void serialshowTime()
{
  TME tm;
  DS3231.read(tm);
  Serial.print("\nCurrent date/time is "); //use print for human-readable ASCII text
  Serial.print(tm.Year + 1970); //use print for human-readable ASCII text
  Serial.print("/");
  Serial.print(tm.Month);
  Serial.print("/");
  Serial.print(tm.Day);
  Serial.print(" ");
  Serial.print(tm.Hour);
  Serial.print(":");
  Serial.print(tm.Minute);
  Serial.print(":");
  Serial.print(tm.Second);
  Serial.print("\n");
  Serial.print(0); //null character to comply with null terminate C string
}

// Doing set-up
void initSetting()
{
  tft.begin();
  tft.setRotation(1); // Lanscape
  tft.fillScreen(CREF_BACKGROUND);
  tft.setTextSize(TEXT_SIZE);
  
  tft.setFont(&FreeMono9pt7b);
  
  Serial.begin(9600);

  DS3231.squareWave(SQWAVE_1_HZ);
  DS3231.read(n_tme);
  
  calc_Hands(n_hands, n_tme);

  drawFace();
  drawBrand();

  draw_NewHourHand();      // draw hour hand
  draw_NewMinuteHand();
  draw_NewSecondHand();
}

/** -------- Main thing goes here -------- **/
void loop()
{
  // infinity loop - WinAVR C++ style, workaround for the problem that
  // variable s keep losing value after each loop thus it cannot get
  // old value of s to draw a black second hand overlap the old second hand  
  
  initSetting();
    copyHandSet(o_hands, n_hands);
    copyTME(o_tme, n_tme);
    calc_Hands(n_hands, n_tme);
    DS3231.read(n_tme);

  for ( ; ; )
  {
    calc_Hands(n_hands, n_tme);
    
    if (o_tme.Second != n_tme.Second) // second change or time ticked
    {
      cdraw_SecondHand();
      
      TextSecond(o_tme.Second, CREF_BACKGROUND);
      draw_NewSecondHand();       // draw new second hand
      TextSecond(n_tme.Second, CREF_TIME);
      
      if (o_tme.Minute != n_tme.Minute) // minute change or minute and hour change
      {
        TextMinute(o_tme.Minute, CREF_BACKGROUND);
        TextMinute(n_tme.Minute, CREF_TIME);
        cdraw_HourHand();
        cdraw_MinuteHand();
      }

      // draw hour, minute, second hands even no changes to overlap the "clear stuff"
      printDate(n_tme.Day, n_tme.Month, n_tme.Year, CREF_DATE);
      showTimeColon(); // draw time ':'
      drawBrand();

      if (o_tme.Hour != n_tme.Hour) // minute change or minute and hour change
      {
        TextHour(o_tme.Hour, CREF_BACKGROUND);
        TextHour(n_tme.Hour, CREF_TIME);
      }
      
      TextHour(n_tme.Hour, CREF_TIME);
      TextMinute(n_tme.Minute, CREF_TIME);
      TextSecond(n_tme.Second, CREF_TIME);
      draw_NewMinuteHand();
      draw_NewHourHand();
      draw_NewSecondHand();
      tft.fillCircle(Xo, Yo-1, 3, FACE);              // draw center dot
    }

    if (o_tme.Minute != n_tme.Minute)       // re-draw face each minute
    {
      drawFace();
    }
    // done with analog clock

    // date change
    if (o_tme.Day != n_tme.Day || o_tme.Month != n_tme.Month || o_tme.Year != n_tme.Year)
    {
      printDate(o_tme.Day, o_tme.Month, o_tme.Year, CREF_BACKGROUND);
      printDate(n_tme.Day, n_tme.Month, n_tme.Year, CREF_DATE);
      //o_tme.Day = n_tme.Day; do not need to copy as it is copied in next loop
    }

    //done drawing, copy the struct to old struct so we clear them after
    copyHandSet(o_hands, n_hands);
    copyTME(o_tme, n_tme);
    
	// Wait for some time until time changes
	// This could be better if using external interupt from output squarewave of DS1307
    TME tmp;
    int cnt = 0;
	// keep reading time and compare n_tme with newer time, aka now()
    while (cnt<11)
    {
      DS3231.read(tmp);
      if (tmp.Second != n_tme.Second) 
      {
        DS3231.read(n_tme);
        break; //if there is change on time
      }
      delay(100);
      cnt++;
    }
    
	// Time changed!!!
    
	
    // Because we stop for a second to get serial data, 
	// it might affect on the serial response time and messed up the SCAN_COM function on PC software.
	// 
    // Format: u2015/7/24 01:57:01u
    while (Serial.available() > 0) {
      switch (Serial.read())
      {
      case 'r':
        if (Serial.read() == 's') // if receive "rs" from serial
        {
          DS3231.read(n_tme);
          calc_Hands(n_hands, n_tme);
          resetFunc();  //call reset
        }
        break;
        
      case 'h':
        n_tme.Hour = Serial.parseInt();
        serialUpdateTime(n_tme);
        calc_Hands(n_hands, n_tme);
        cdraw_HourHand();
        break;
        
      case 'm':
        n_tme.Minute = Serial.parseInt();
        serialUpdateTime(n_tme);
        calc_Hands(n_hands, n_tme);
        cdraw_MinuteHand();        
        break;
        
      case 's':
        n_tme.Second = Serial.parseInt();
        serialUpdateTime(n_tme);        
        cdraw_SecondHand();
        break;
        
      case 'D':
        n_tme.Day = Serial.parseInt();
        serialUpdateTime(n_tme);
        break;
        
      case 'M':
        n_tme.Month = Serial.parseInt();
        serialUpdateTime(n_tme);
        break;
        
      case 'Y':
        n_tme.Year = Serial.parseInt() - 1970;
        serialUpdateTime(n_tme);
        break;
        
      case 'u':
        n_tme.Year = Serial.parseInt() - 1970;
        n_tme.Month = Serial.parseInt();   // do it again
        n_tme.Day = Serial.parseInt();     // again...
        n_tme.Hour = Serial.parseInt();
        n_tme.Minute = Serial.parseInt();
        n_tme.Second = Serial.parseInt();
        if (Serial.read() == 'u')       // only set time when receive another 'u'
          serialUpdateTime(n_tme);
        break;
        
      case 'c':
        Serial.write("z");
		// should write a null character on each command, 
		// just like how we say 'over' after finish speaking on the walkie-talkie
		// Serial.write(0);
        break;
        
      case 't':       
        serialshowTime();
      } //end switch
    } //end while 
  } //end for(;;)
}//end loop()
