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
#define AR_YELLOW       0xE68D
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define PINK            0xF81F


#define TFT_CS     PA4
#define TFT_RST    PA1
#define TFT_DC     PA0
#define TFT_LED    PB9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int NextPageTime = 50;




// == Gauge config ==
// ==================
byte GaugeX = 52;
byte GaugeY = 52;
byte GaugeR = 50;
int GaugeStartAngle = 135;
int GaugeLenghtAngle = 225;
int GaugeMaxAngle = GaugeStartAngle + GaugeLenghtAngle;
byte GaugeParts = 25;
byte GaugePartsLong = 5;
int GaugeHandAngle = GaugeStartAngle;
// 0-360; 135-180; 135-225

// Colors
#define GAUGE_FACE				AR_YELLOW
#define GAUGE_MARKS				AR_YELLOW
#define GAUGE_MARKS_LONG		RED
#define GAUGE_MAIN_HAND			AR_YELLOW
#define GAUGE_SECONDARY_HAND	WHITE
#define GAUGE_ERASER			BLACK
#define GAUGE_CENTRAL_POINT		AR_YELLOW


// Global var for prev pos saving
float p = 3.1415926;
float GaugeHandAngleTop;
float GaugeHandAngleLeft;
float GaugeHandAngleRight;










void setup(void) {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  
  GaugeFaceDraw(1, GaugeX, GaugeY, GaugeR, GaugeStartAngle, GaugeLenghtAngle, GaugeParts, GaugePartsLong, GaugeHandAngle);
	GaugeFaceDraw(1, 120, 100, 30, 0, 360, 10, 2, 0);
  //GaugeFaceDraw(1, 52, 52, 50, 135, 180, 5, 150);
}







void loop() {
	UpdateScreen();
	
	IncrementHand();
}


void UpdateScreen() {
	//drawFace();
	//RotateHand();
	
	fps(0.25, 0, 120);
}


/*
void drawFace() {
	tft.setRotation(3);
	tft.setFont();
	tft.setTextSize(1);
	
	int i = 0, angle = 0;
	float x, y;

	// Draw outer frame
	//tft.drawCircle(Xo, Yo, RADIUS + 21, FACE);
	//tft.drawCircle(Xo, Yo, RADIUS + 20, FACE);

	// Draw inner frame
	tft.drawCircle(Xo, Yo, RADIUS + 10, FACE);
	tft.drawCircle(Xo, Yo, RADIUS + 9, FACE);
	tft.drawCircle(Xo, Yo, RADIUS + 8, FACE);

	//Draw Numeric point
	
	for (i = 0; i <= 12; i++) {
		x = Xo + RADIUS * cos(angle * p / 180);
		y = Yo + RADIUS * sin(angle * p / 180);
		tft.drawCircle(x, y, 2, NUMERIC_POINT);
		angle += 30;
	}

	for (i = 0; i < 360; i += 6) {
		tft.drawPixel(Xo + RADIUS * cos(i * p / 180), Yo + RADIUS * sin(i * p / 180), NUMERIC_POINT);
	}
  
  
  
}

void RotateHand() {


	
	tft.drawLine(Xo, Yo, Xo+RADIUS*cos(HandAngleTop*p/180), Yo+RADIUS*sin(HandAngleTop*p/180), BLACK);
	//tft.fillCircle(Xo, Yo, 45, BLACK);
	
	//tft.fillTriangle(Xo+15*cos(HandAngleLeft*p/180), Yo+15*sin(HandAngleLeft*p/180), Xo+46*cos(HandAngleTop*p/180), Yo+46*sin(HandAngleTop*p/180), Xo+15*cos(HandAngleRight*p/180), Yo+15*sin(HandAngleRight*p/180), BLACK);
	
	
	//
	
	HandAngleTop = HandAngleTop + 1;
	
	if (HandAngleTop < 90) {
		HandAngleLeft = 360 - 90 + HandAngleTop;
	} else {
		HandAngleLeft = HandAngleTop - 90;
	}
	
	if (360-HandAngleTop < 90) {
		HandAngleRight = 90 - (360-HandAngleTop);
	} else {
		HandAngleRight = HandAngleTop + 90;
	}
	
	
	//tft.drawTriangle(Xo+2*cos(HandAngleLeft*p/180), Yo+2*sin(HandAngleLeft*p/180), Xo+RADIUS*cos(HandAngleTop*p/180), Yo+RADIUS*sin(HandAngleTop*p/180), Xo+2*cos(HandAngleRight*p/180), Yo+2*sin(HandAngleRight*p/180), WHITE);
	tft.drawLine(Xo, Yo, Xo+RADIUS*cos(HandAngleTop*p/180), Yo+RADIUS*sin(HandAngleTop*p/180), WHITE);
	
	tft.fillCircle(Xo, Yo, 2, WHITE);
	

	
}
*/


int HandDirection = 1;
void IncrementHand() {
	// Testing class
	
	int Mode = 3;
	int Step = 1;
	
	// через map
	
	if (Mode == 1) { // Only CW
		GaugeHandAngle = GaugeHandAngle + Step;
		if (GaugeHandAngle > GaugeMaxAngle) {
			GaugeHandAngle = GaugeStartAngle;
		}
	} else if (Mode == 2) {
		GaugeHandAngle = GaugeHandAngle - Step;
		if (GaugeHandAngle < GaugeStartAngle) {
			GaugeHandAngle = GaugeMaxAngle;
		}
	} else if (Mode == 3) { // CW - CCW
		if (HandDirection == 1) { //CW
			GaugeHandAngle = GaugeHandAngle + Step;
			if (GaugeHandAngle >= GaugeMaxAngle) {
				GaugeHandAngle = GaugeHandAngle - Step;
				HandDirection = 0;
			}
		} else { //CCW
			GaugeHandAngle = GaugeHandAngle - Step;
			if (GaugeHandAngle <= GaugeStartAngle) {
				GaugeHandAngle = GaugeHandAngle + Step;
				HandDirection = 1;
			}
		}
	}



	//tft.fillCircle(GaugeX, GaugeY, GaugeR-2, BLACK);
	
	GaugeFaceDraw(0, GaugeX, GaugeY, GaugeR, GaugeStartAngle, GaugeLenghtAngle, GaugeParts, GaugePartsLong, GaugeHandAngle);
	GaugeFaceDraw(0, 120, 100, 30, 0, 360, 10, 2, GaugeHandAngle);
	//delay(10);
}



void GaugeFaceDraw(const bool FirstRun, const int X, const int Y, const int R, const int AngleStart, const int AngleAbs, const int Parts, const int PartsLong, int GaugeHandAngle) {
	
	int i, AuxR, AngleEnd, PartAngle, LongPartAngle, AngleDiff;
	AngleEnd = AngleStart + AngleAbs;
	PartAngle = AngleAbs / Parts;

	
	if (FirstRun == 1) {
		
		AuxR = R;
		for (i = AngleStart; i < AngleEnd; i += 1) {
			tft.drawPixel(X + AuxR * cos(i * p / 180), Y + AuxR * sin(i * p / 180), GAUGE_FACE);}
		AuxR = R + 1;
		for (i = AngleStart; i < AngleEnd; i += 1) {
			tft.drawPixel(X + AuxR * cos(i * p / 180), Y + AuxR * sin(i * p / 180), GAUGE_FACE);}
	
	} else {

		AuxR = R-10;
		
		// Erase last position
		int txrb, tyrb;  // right bottom
		int txru, tyru;  // right up
		int txlb, tylb;  // left bottom
		int txlu, tylu;  // left up
		int f;     //direction factor
		bool mode; // 0 - fill except, 1 - fill last
		
		if (GaugeHandAngle > GaugeHandAngleTop) {	//  CW
			AngleDiff = GaugeHandAngle - GaugeHandAngleTop;
			if (AngleDiff <= 2) {
				f = -1;
				mode = 0;
			} else {
				mode = 1;}
		} else {									//  CCW
			AngleDiff = GaugeHandAngleTop - GaugeHandAngle;
			if (AngleDiff <= 2) {
				f = 1;
				mode = 0;
			} else {
				mode = 1;}}

		if (mode == 0) {
			txrb = X+(3*f)*cos((GaugeHandAngleRight+1)*p/180);
			tyrb = Y+(3*f)*sin((GaugeHandAngleRight+1)*p/180);
			txru = X+(AuxR+6)*cos((GaugeHandAngleTop+(1*f))*p/180);
			tyru = Y+(AuxR+6)*sin((GaugeHandAngleTop+(1*f))*p/180);
			txlb = X+(7*f)*cos((GaugeHandAngleRight+5)*p/180);
			tylb = Y+(7*f)*sin((GaugeHandAngleRight+5)*p/180);
			txlu = X+(AuxR+6)*cos((GaugeHandAngleTop+(5*f))*p/180);
			tylu = Y+(AuxR+6)*sin((GaugeHandAngleTop+(5*f))*p/180);
		} else {
			txrb = X+4*cos(GaugeHandAngleRight*p/180);
			tyrb = Y+4*sin(GaugeHandAngleRight*p/180);
			txru = X+(AuxR+6)*cos((GaugeHandAngleTop+4)*p/180);
			tyru = Y+(AuxR+6)*sin((GaugeHandAngleTop+4)*p/180);
			txlb = X+4*cos(GaugeHandAngleLeft*p/180);
			tylb = Y+4*sin(GaugeHandAngleLeft*p/180);
			txlu = X+(AuxR+6)*cos((GaugeHandAngleTop-4)*p/180);
			tylu = Y+(AuxR+6)*sin((GaugeHandAngleTop-4)*p/180);}
		
		tft.fillTriangle(txrb, tyrb, txru, tyru, txlb, tylb, GAUGE_ERASER);
		tft.fillTriangle(txru, tyru, txlu, tylu, txlb, tylb, GAUGE_ERASER);



		// Hand angles calc and draw
		GaugeHandAngleTop = GaugeHandAngle;
		if (GaugeHandAngleTop < 90) {
			GaugeHandAngleLeft = 360 - 90 + GaugeHandAngleTop;
		} else {
			GaugeHandAngleLeft = GaugeHandAngleTop - 90;}
		
		if (360-GaugeHandAngleTop < 90) {
			GaugeHandAngleRight = 90 - (360-GaugeHandAngleTop);
		} else {
			GaugeHandAngleRight = GaugeHandAngleTop + 90;}
		
		tft.fillTriangle(X+2*cos(GaugeHandAngleLeft*p/180),   Y+2*sin(GaugeHandAngleLeft*p/180), 
						 X+AuxR*cos(GaugeHandAngleTop*p/180), Y+AuxR*sin(GaugeHandAngleTop*p/180), 
						 X+2*cos(GaugeHandAngleRight*p/180),  Y+2*sin(GaugeHandAngleRight*p/180), GAUGE_MAIN_HAND);
		tft.fillCircle(X, Y, 2, GAUGE_CENTRAL_POINT);



		// Marks draw
		int PartsLongCount = PartsLong;
		for (i = AngleStart; i < AngleEnd+5; i += PartAngle) {
			if (PartsLongCount == PartsLong) {
				AuxR = R-10;
				tft.drawLine(X+AuxR*cos(i*p/180), Y+AuxR*sin(i*p/180), X+R*cos(i*p/180), Y+R*sin(i*p/180), GAUGE_MARKS_LONG);
				PartsLongCount = 1;
			} else {
				AuxR = R-6;
				tft.drawLine(X+AuxR*cos(i*p/180), Y+AuxR*sin(i*p/180), X+R*cos(i*p/180), Y+R*sin(i*p/180), GAUGE_MARKS);
				PartsLongCount = PartsLongCount + 1;
			}
			
		}
		
	}

	
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
	
	tft.fillRect(x, y, 20, 8, 0x0000);

    tft.print(framesPerSecond);
    frameCount = 0;
    lastMillis = now;
  }
}










void TFTcheck() {
  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(NextPageTime*4);

  tftPrintTest();
  delay(NextPageTime);

  testlines(ST77XX_YELLOW);
  delay(NextPageTime);

  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(NextPageTime);

  testdrawrects(ST77XX_GREEN);
  delay(NextPageTime);

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  delay(NextPageTime);

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(NextPageTime);

  testroundrects();
  delay(NextPageTime);

  testtriangles();
  delay(NextPageTime);
}


void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}
