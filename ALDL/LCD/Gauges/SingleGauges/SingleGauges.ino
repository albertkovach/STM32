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
}



void loop() {
	fps(0.25, 0, 120);
	IncrementHand();
}





int HandDirection = 1;
void IncrementHand() {
	// Testing class
	
	int Mode = 3;
	int Step = 1;
	int GaugeMaxAngle = GaugeStartAngle + GaugeLenghtAngle;
	
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