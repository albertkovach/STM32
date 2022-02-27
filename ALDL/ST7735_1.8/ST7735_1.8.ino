/*   
  Red 1.8 TFT PSI 128*160 v1.1 ST7735
      Подключение к Leonardo
    == PIN === Conn === Define
     * LED   * 3.3V  *   ---
     * SCK   *  13   *   ---
     * SDA   *  11   *   ---
     * A0    *  10   *    9
     * RESET *   9   *    8
     * CS    *  11   *   10
     * GND   * GND   *   ---
     * VCC   *  5V   *   ---

ICSP conn pinout:
RST * SCK  * MISO
GND * MOSI *  5V

Hardware SPI Pins:
 * Arduino Uno   SCK=13, SDA=11
 * Arduino Nano  SCK=13, SDA=11
 * Arduino Due   SCK=76, SDA=75
 * Arduino Mega  SCK=52, SDA=51

SPI pin names can be confusing. These are the alternative names for the SPI pins:
MOSI = SDA = SDO = DIN = R/W = DI = SI = MTSR = D1 = SDI
SCLK = CLK = E = SCK = SCL = D0
CS = CE = RS = SS
DC = A0 = DO = DOUT = SO = MRST
RESET = RST


Reference page for GFX Library: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf

Color is expressed in 16 bit with Hexadecimal value.
To select a particular color, go here and copy the "Hexadecimal 16 bit color depth value":
https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html

Common colors:
 * BLACK    0x0000
 * BLUE     0x001F
 * RED      0xF800
 * GREEN    0x07E0
 * CYAN     0x07FF
 * MAGENTA  0xF81F
 * YELLOW   0xFFE0
 * WHITE    0xFFFF

A way to select a color is to write: "ST7735_BLACK", or "ST7735_BLUE", etc.
Or just write the code for the color. Either way, it works.


List of custom fonts: https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts

Note about custom font:
 * Text background color is not supported for custom fonts. For these reason you would need to draw a filled 
   rectangle before drawing the text. But this would cause the text to flicker, so I don't recommend using custom fonts
   for components that refresh continuously.
 * Using custom fonts slows down the arduino loop, so the refresh rate is lesser than using the standard font.
*/


#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Fonts/FreeSerif18pt7b.h>

#define TFT_CS     9 //10
#define TFT_RST    10 //8
#define TFT_DC     11 //9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int Btn1Pin = 2;
int Btn2Pin = 3;
int Btn1State = 0;
int Btn2State = 0;
int Btn1OldState = 0;
int Btn2OldState = 0;





void setup(void) {
  pinMode(Btn1Pin, INPUT_PULLUP);
  pinMode(Btn2Pin, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
}


void loop() {
  Btn1State = digitalRead(Btn1Pin);
  Btn2State = digitalRead(Btn2Pin);
  
  if (Btn1State != Btn1OldState) {
    Btn1Event();
  }

  if (Btn2State != Btn2OldState) {
    Btn2Event();
  }

  Btn1OldState = Btn1State;
  Btn2OldState = Btn2State;
}


void Btn1Event() {
  tft.setFont();
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_WHITE, ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.print("Btn1State: ");
  tft.println(Btn1State);
  checksum_check();
}

void Btn2Event() {
  tft.setFont();
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_WHITE, ST7735_BLACK);
  tft.setCursor(0, 15);
  tft.print("Btn2State: ");
  tft.println(Btn2State);
  tft.print("....");
  tft.print(millis());
}

void checksum_check() {
  byte M1Cmd[] = {0xF4,0x57,0xB2,0xB5, 0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9};
  byte CheckSumm;

  CheckSumm = checksum_generate(M1Cmd, sizeof(M1Cmd));
  
  tft.setFont();
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_WHITE, ST7735_BLACK);
  
  tft.setCursor(0, 50);
  //tft.print((char*)M1Cmd);
  tft.println(CheckSumm, HEX);

  int linecount = 7;
  int bytecounter = 0;
        int j=0;
        while (j<sizeof(M1Cmd)) {
            tft.print(M1Cmd[j], HEX);
            j++;
            bytecounter++;
            if (bytecounter >= linecount) {  // check if time for new line
                bytecounter = 0;             // Reset byte count for next line
                tft.println("");             // add a new line
            }
            else {
                tft.print(" ");              // or add a space
            }
        }
}


byte checksum_generate(byte *buf, int len) {
  //  Checksum is 2's complement of sum of all bytes in the message
  #ifdef RETARDED
  retardptr(buf,"checksum buf");
  #endif
  int x = 0;
  unsigned int sum = 0;
  for(x=0;x<len;x++) sum += buf[x];
  return ( 256 - ( sum % 256 ) );
}
