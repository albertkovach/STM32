/*   
  Red 1.8 TFT PSI 128*160 v1.1 ST7735

SPI pin names can be confusing. These are the alternative names for the SPI pins:
MOSI = SDA = SDO = DIN = R/W = DI = SI = MTSR = D1 = SDI
SCLK = CLK = E = SCK = SCL = D0
DC = A0 = DO = DOUT = SO = MRST
CS = CE = RS = SS
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
 
 
	LED = 3v3
	SCK = PA5 (SPI1 SCK)
	SDA = PA7 (SPI1 MOSI)
	A0  = PA0
	RST = PA1
	CS  = PA4 (SPI1 NSS)
	GND = GND
	VCC = 5v

*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS     PA4
#define TFT_RST    PA1
#define TFT_DC     PA0
#define TFT_LED    PB9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);



void setup(void) {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
}

void loop() {
	UpdateScreen();
}


void UpdateScreen() {
	fps(0.25, 0, 120);
}



static inline void fps(const float seconds, const int x, const int y){

  static unsigned long lastMillis;
  static unsigned long frameCount;
  static unsigned int framesPerSecond;
  
  unsigned long now = millis();
  frameCount ++;
  
  tft.setCursor(x, y);
  
  if (now - lastMillis >= seconds * 1000) {
    framesPerSecond = frameCount / seconds;
	
	String framesPerSecondStr = String(framesPerSecond);
	int16_t x1, y1;
	uint16_t w, h;
	tft.getTextBounds(framesPerSecondStr, x, y, &x1, &y1, &w, &h);
	tft.fillRect(x, y, w, h, 0x0000);

	tft.print(framesPerSecond);
	frameCount = 0;
	lastMillis = now;
  }
}