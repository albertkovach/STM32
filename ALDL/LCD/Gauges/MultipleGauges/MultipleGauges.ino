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

int GaugesConfigData[9][2];
/* Configurations:
	0: CenterPosX
	1: CenterPosY
	2: FaceR
	3: StartAngle
	4: EndAngle
	5: AngleLength
	6: Parts
	7: PartsLong
	8: DesiredHandAngle */
// 0-360; 135-180; 135-225

float GaugesSelfErasingData[3][2];
/* Configurations:
	0: HandAngleTop
	1: HandAngleLeft
	2: HandAngleRight */


int GaugesColors[7][2];
/* Configurations:
	0: GAUGE_FACE
	1: GAUGE_MARKS
	2: GAUGE_MARKS_LONG
	3: GAUGE_MAIN_HAND
	4: GAUGE_SECONDARY_HAND
	5: GAUGE_ERASER
	6: GAUGE_CENTRAL_POINT */




int HandDirections[2];
float p = 3.1415926;





void setup(void) {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  
  
  GaugesInitConfig();
  GaugesDrawFace();
}


void loop() {
	fps(0.25, 0, 120);
	GaugesRotate();
}




void GaugesInitConfig() {
	/* Configurations:
	0: CenterPosX
	1: CenterPosY
	2: FaceR
	3: StartAngle
	4: AngleLength
	5: Parts
	6: PartsLong
	7: DesiredHandAngle */
	
	/* Colors:
	0: GAUGE_FACE
	1: GAUGE_MARKS
	2: GAUGE_MARKS_LONG
	3: GAUGE_MAIN_HAND
	4: GAUGE_SECONDARY_HAND
	5: GAUGE_ERASER
	6: GAUGE_CENTRAL_POINT */
	
	GaugesConfigData [0][0] = 52;
	GaugesConfigData [1][0] = 52;
	GaugesConfigData [2][0] = 50;
	GaugesConfigData [3][0] = 135;
	GaugesConfigData [4][0] = 225;
	GaugesConfigData [5][0] = 25;
	GaugesConfigData [6][0] = 5;
	GaugesConfigData [7][0] = 135;
	
	GaugesConfigData [0][1] = 120;
	GaugesConfigData [1][1] = 100;
	GaugesConfigData [2][1] = 30;
	GaugesConfigData [3][1] = 135;
	GaugesConfigData [4][1] = 270;
	GaugesConfigData [5][1] = 30;
	GaugesConfigData [6][1] = 5;
	GaugesConfigData [7][1] = 135;
	
	
	GaugesColors [0][0] = AR_YELLOW;
	GaugesColors [1][0] = AR_YELLOW;
	GaugesColors [2][0] = RED;
	GaugesColors [3][0] = AR_YELLOW;
	GaugesColors [4][0] = 0;
	GaugesColors [5][0] = BLACK;
	GaugesColors [6][0] = AR_YELLOW;
	
	GaugesColors [0][1] = AR_YELLOW;
	GaugesColors [1][1] = AR_YELLOW;
	GaugesColors [2][1] = RED;
	GaugesColors [3][1] = AR_YELLOW;
	GaugesColors [4][1] = 0;
	GaugesColors [5][1] = BLACK;
	GaugesColors [6][1] = AR_YELLOW;
}

void GaugesDrawFace() {
	GaugeDraw(1, 0);
	GaugeDraw(1, 1);
}

void GaugesRotate() {
	IncrementHand(3, 1, 0);
	IncrementHand(3, 2, 1);
	GaugeDraw(0, 0);
	GaugeDraw(0, 1);
}



void GaugeDraw(bool FirstRun, int GaugeNum) {
	/* GaugesConfigData[x][GaugeNum]
	Configurations:
	0: CenterPosX
	1: CenterPosY
	2: FaceR
	3: StartAngle
	4: AngleLength
	5: Parts
	6: PartsLong
	7: DesiredHandAngle */
	
	/*GaugesSelfErasingData[x][GaugeNum];
	Configurations:
	0: HandAngleTop
	1: HandAngleLeft
	2: HandAngleRight */
	
	/* GaugesColors[6][GaugeNum]
	Configurations:
	0: GAUGE_FACE
	1: GAUGE_MARKS
	2: GAUGE_MARKS_LONG
	3: GAUGE_MAIN_HAND
	4: GAUGE_SECONDARY_HAND
	5: GAUGE_ERASER
	6: GAUGE_CENTRAL_POINT */

	int i, AuxR, AngleEnd, PartAngle, LongPartAngle, AngleDiff;
	AngleEnd = GaugesConfigData[3][GaugeNum] + GaugesConfigData[4][GaugeNum];
	PartAngle = GaugesConfigData[4][GaugeNum] / GaugesConfigData[5][GaugeNum];
	
	if (FirstRun == 1) {
		AuxR = GaugesConfigData[2][GaugeNum];
		for (i = GaugesConfigData[3][GaugeNum]; i < AngleEnd; i += 1) {
			tft.drawPixel(GaugesConfigData[0][GaugeNum] + AuxR * cos(i * p / 180),
						  GaugesConfigData[1][GaugeNum] + AuxR * sin(i * p / 180),
						  GaugesColors[0][GaugeNum]);}
		AuxR = GaugesConfigData[2][GaugeNum] + 1;
		for (i = GaugesConfigData[3][GaugeNum]; i < AngleEnd; i += 1) {
			tft.drawPixel(GaugesConfigData[0][GaugeNum] + AuxR * cos(i * p / 180),
						  GaugesConfigData[1][GaugeNum] + AuxR * sin(i * p / 180),
						  GaugesColors[0][GaugeNum]);}
	
	} else {
		AuxR = GaugesConfigData[2][GaugeNum]-10;
		
		// Erase last position
		int txrb, tyrb;  // right bottom
		int txru, tyru;  // right up
		int txlb, tylb;  // left bottom
		int txlu, tylu;  // left up
		int f;     //direction factor
		bool mode; // 0 - fill except, 1 - fill last
		
		if (GaugesConfigData [7][GaugeNum] > GaugesSelfErasingData[0][GaugeNum]) {	// CW
			AngleDiff = GaugesConfigData [7][GaugeNum] - GaugesSelfErasingData[0][GaugeNum];
			if (AngleDiff <= 2) {
				f = -1;
				mode = 0;
			} else {
				mode = 1;}
		} else {	// CCW
			AngleDiff = GaugesSelfErasingData[0][GaugeNum] - GaugesConfigData [7][GaugeNum];
			if (AngleDiff <= 2) {
				f = 1;
				mode = 0;
			} else {
				mode = 1;}}

		if (mode == 0) {
			txrb = GaugesConfigData[0][GaugeNum]+(3*f)*cos((GaugesSelfErasingData[2][GaugeNum]+1)*p/180);
			tyrb = GaugesConfigData[1][GaugeNum]+(3*f)*sin((GaugesSelfErasingData[2][GaugeNum]+1)*p/180);
			txru = GaugesConfigData[0][GaugeNum]+(AuxR+6)*cos((GaugesSelfErasingData[0][GaugeNum]+(1*f))*p/180);
			tyru = GaugesConfigData[1][GaugeNum]+(AuxR+6)*sin((GaugesSelfErasingData[0][GaugeNum]+(1*f))*p/180);
			txlb = GaugesConfigData[0][GaugeNum]+(7*f)*cos((GaugesSelfErasingData[2][GaugeNum]+5)*p/180);
			tylb = GaugesConfigData[1][GaugeNum]+(7*f)*sin((GaugesSelfErasingData[2][GaugeNum]+5)*p/180);
			txlu = GaugesConfigData[0][GaugeNum]+(AuxR+6)*cos((GaugesSelfErasingData[0][GaugeNum]+(5*f))*p/180);
			tylu = GaugesConfigData[1][GaugeNum]+(AuxR+6)*sin((GaugesSelfErasingData[0][GaugeNum]+(5*f))*p/180);
		} else {
			txrb = GaugesConfigData[0][GaugeNum]+4*cos(GaugesSelfErasingData[2][GaugeNum]*p/180);
			tyrb = GaugesConfigData[1][GaugeNum]+4*sin(GaugesSelfErasingData[2][GaugeNum]*p/180);
			txru = GaugesConfigData[0][GaugeNum]+(AuxR+6)*cos((GaugesSelfErasingData[0][GaugeNum]+4)*p/180);
			tyru = GaugesConfigData[1][GaugeNum]+(AuxR+6)*sin((GaugesSelfErasingData[0][GaugeNum]+4)*p/180);
			txlb = GaugesConfigData[0][GaugeNum]+4*cos(GaugesSelfErasingData[1][GaugeNum]*p/180);
			tylb = GaugesConfigData[1][GaugeNum]+4*sin(GaugesSelfErasingData[1][GaugeNum]*p/180);
			txlu = GaugesConfigData[0][GaugeNum]+(AuxR+6)*cos((GaugesSelfErasingData[0][GaugeNum]-4)*p/180);
			tylu = GaugesConfigData[1][GaugeNum]+(AuxR+6)*sin((GaugesSelfErasingData[0][GaugeNum]-4)*p/180);}
		
		tft.fillTriangle(txrb, tyrb, txru, tyru, txlb, tylb, GaugesColors[5][GaugeNum]);
		tft.fillTriangle(txru, tyru, txlu, tylu, txlb, tylb, GaugesColors[5][GaugeNum]);


		// Hand angles calc and draw
		GaugesSelfErasingData[0][GaugeNum] = GaugesConfigData [7][GaugeNum];
		if (GaugesSelfErasingData[0][GaugeNum] < 90) {
			GaugesSelfErasingData[1][GaugeNum] = 360 - 90 + GaugesSelfErasingData[0][GaugeNum];
		} else {
			GaugesSelfErasingData[1][GaugeNum] = GaugesSelfErasingData[0][GaugeNum] - 90;}
		
		if (360-GaugesSelfErasingData[0][GaugeNum] < 90) {
			GaugesSelfErasingData[2][GaugeNum] = 90 - (360-GaugesSelfErasingData[0][GaugeNum]);
		} else {
			GaugesSelfErasingData[2][GaugeNum] = GaugesSelfErasingData[0][GaugeNum] + 90;}
		
		tft.fillTriangle(GaugesConfigData[0][GaugeNum]+2*cos(GaugesSelfErasingData[1][GaugeNum]*p/180),
						 GaugesConfigData[1][GaugeNum]+2*sin(GaugesSelfErasingData[1][GaugeNum]*p/180), 
						 GaugesConfigData[0][GaugeNum]+AuxR*cos(GaugesSelfErasingData[0][GaugeNum]*p/180),
						 GaugesConfigData[1][GaugeNum]+AuxR*sin(GaugesSelfErasingData[0][GaugeNum]*p/180), 
						 GaugesConfigData[0][GaugeNum]+2*cos(GaugesSelfErasingData[2][GaugeNum]*p/180),
						 GaugesConfigData[1][GaugeNum]+2*sin(GaugesSelfErasingData[2][GaugeNum]*p/180),
						 GaugesColors[3][GaugeNum]);
		tft.fillCircle(GaugesConfigData[0][GaugeNum], GaugesConfigData[1][GaugeNum], 2, GaugesColors[6][GaugeNum]);


		// Marks draw
		int PartsLongCount = GaugesConfigData[6][GaugeNum];
		for (i = GaugesConfigData[3][GaugeNum]; i < AngleEnd+5; i += PartAngle) {
			if (PartsLongCount == GaugesConfigData[6][GaugeNum]) {
				AuxR = GaugesConfigData[2][GaugeNum]-10;
				tft.drawLine(GaugesConfigData[0][GaugeNum]+AuxR*cos(i*p/180),
							 GaugesConfigData[1][GaugeNum]+AuxR*sin(i*p/180),
							 GaugesConfigData[0][GaugeNum]+GaugesConfigData[2][GaugeNum]*cos(i*p/180),
							 GaugesConfigData[1][GaugeNum]+GaugesConfigData[2][GaugeNum]*sin(i*p/180),
							 GaugesColors[2][GaugeNum]);
				PartsLongCount = 1;
			} else {
				AuxR = GaugesConfigData[2][GaugeNum]-6;
				tft.drawLine(GaugesConfigData[0][GaugeNum]+AuxR*cos(i*p/180),
							 GaugesConfigData[1][GaugeNum]+AuxR*sin(i*p/180),
							 GaugesConfigData[0][GaugeNum]+GaugesConfigData[2][GaugeNum]*cos(i*p/180),
							 GaugesConfigData[1][GaugeNum]+GaugesConfigData[2][GaugeNum]*sin(i*p/180),
							 GaugesColors[1][GaugeNum]);
				PartsLongCount = PartsLongCount + 1;
			}
		}
	}

	
}




void IncrementHand(int Mode, int Step, int GaugeNum) {
	// Testing class
		/* Modes:
		1: Only CW
		2: Only CCW
		3: CW - CCW */
	int GaugeStartAngle = GaugesConfigData[3][GaugeNum];
	int GaugeLenghtAngle = GaugesConfigData[4][GaugeNum];
	int GaugeMaxAngle = GaugeStartAngle + GaugeLenghtAngle;
	int GaugeHandAngle = GaugesConfigData[7][GaugeNum];
	// через map
	
	if (Mode == 1) { // Only CW
		GaugeHandAngle = GaugeHandAngle + Step;
		if (GaugeHandAngle > GaugeMaxAngle) {
			GaugeHandAngle = GaugeStartAngle;
		}
	} else if (Mode == 2) { //
		GaugeHandAngle = GaugeHandAngle - Step;
		if (GaugeHandAngle < GaugeStartAngle) {
			GaugeHandAngle = GaugeMaxAngle;
		}
	} else if (Mode == 3) { // CW - CCW
		if (HandDirections[GaugeNum] == 1) { //CW
			GaugeHandAngle = GaugeHandAngle + Step;
			if (GaugeHandAngle >= GaugeMaxAngle) {
				GaugeHandAngle = GaugeHandAngle - Step;
				HandDirections[GaugeNum] = 0;
			}
		} else { //CCW
			GaugeHandAngle = GaugeHandAngle - Step;
			if (GaugeHandAngle <= GaugeStartAngle) {
				GaugeHandAngle = GaugeHandAngle + Step;
				HandDirections[GaugeNum] = 1;
			}
		}
	}

	GaugesConfigData[7][GaugeNum] = GaugeHandAngle;
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