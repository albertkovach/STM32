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

int NextPageTime = 50;

/*
typedef struct POINT {uint16_t x; uint16_t y;};
typedef struct LINE {POINT a;POINT b;};
typedef struct HAND_POINTS {POINT a;POINT b; POINT e; POINT f;};*/

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


#define Xo 45   // center point
#define Yo 45   // center point
#define RADIUS 40  // radius of the clock face

#define TEXT_SIZE 1


float p = 3.1415926;
float HandAngleTop;
float HandAngleLeft;
float HandAngleRight;
int HandAngle = 135;




void setup(void) {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  
  GaugeFaceDraw(1, 52, 52, 50, 0, 360, 5, 150);
  //GaugeFaceDraw(1, 52, 52, 50, 135, 180, 5, 150);
}


void loop() {
	UpdateScreen();
	
	IncrementHand();
}


void UpdateScreen() {
	
	//drawFace();
	
	//RotateHand();
	
	
	
	fps(1, 0, 120);
}

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
	/*
	for (i = 0; i <= 12; i++) {
		x = Xo + RADIUS * cos(angle * p / 180);
		y = Yo + RADIUS * sin(angle * p / 180);
		tft.drawCircle(x, y, 2, NUMERIC_POINT);
		angle += 30;
	}

	for (i = 0; i < 360; i += 6) {
		tft.drawPixel(Xo + RADIUS * cos(i * p / 180), Yo + RADIUS * sin(i * p / 180), NUMERIC_POINT);
	}*/
  
  
  
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



void IncrementHand() {
	int MinAngle = 135;
	int MaxAngle = 180;
	
	
	HandAngle = HandAngle + 5;
	if (HandAngle > MaxAngle) {
		HandAngle = MinAngle;
	}
	GaugeFaceDraw(0, 52, 52, 50, 135, 180, 5, HandAngle);
	//GaugeFaceDraw(0, 52, 52, 50, 0, 360, 5, HandAngle);
	delay(12);
}



void GaugeFaceDraw(const bool FirstRun, const int X, const int Y, const int R, const int AngleStart, const int AngleAbs, const int Parts, int HandAngle) {
	
	int i, AuxR, AngleEnd, PartAngle, AngleDiff;
	AngleEnd = AngleStart + AngleAbs;
	PartAngle = AngleAbs / Parts;
	
	//tft.fillCircle(X, Y, R, BLACK);
	
	if (FirstRun == 1) {
		
		AuxR = R;
		for (i = AngleStart; i < AngleEnd; i += 1) {
			tft.drawPixel(X + AuxR * cos(i * p / 180), Y + AuxR * sin(i * p / 180), NUMERIC_POINT);}
		AuxR = R + 1;
		for (i = AngleStart; i < AngleEnd; i += 1) {
			tft.drawPixel(X + AuxR * cos(i * p / 180), Y + AuxR * sin(i * p / 180), NUMERIC_POINT);}
	
	} else {

		AuxR = R-10;
		
		
		if (HandAngle > HandAngleTop) {
			AngleDiff = HandAngle - HandAngleTop;
			if (AngleDiff <= 2) {
				//	CW
				tft.drawLine(X+3*cos((HandAngleLeft-1)*p/180), 		 Y+3*sin((HandAngleLeft-1)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop-1)*p/180), Y+(AuxR+6)*sin((HandAngleTop-1)*p/180), BLACK);
				tft.drawLine(X+4*cos((HandAngleLeft-2)*p/180), 		 Y+4*sin((HandAngleLeft-2)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop-2)*p/180), Y+(AuxR+6)*sin((HandAngleTop-2)*p/180), BLACK);
				tft.drawLine(X+5*cos((HandAngleLeft-3)*p/180), 		 Y+5*sin((HandAngleLeft-3)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop-3)*p/180), Y+(AuxR+6)*sin((HandAngleTop-3)*p/180), BLACK);
				tft.drawLine(X+6*cos((HandAngleLeft-4)*p/180), 		 Y+6*sin((HandAngleLeft-4)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop-4)*p/180), Y+(AuxR+6)*sin((HandAngleTop-4)*p/180), BLACK);
				tft.drawLine(X+7*cos((HandAngleLeft-5)*p/180),		 Y+7*sin((HandAngleLeft-5)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop-5)*p/180), Y+(AuxR+6)*sin((HandAngleTop-5)*p/180), BLACK);
			} else {
				tft.fillTriangle(X+5*cos(HandAngleLeft*p/180), 		 Y+5*sin(HandAngleLeft*p/180), 
								 X+(AuxR+6)*cos(HandAngleTop*p/180), Y+(AuxR+6)*sin(HandAngleTop*p/180), 
								 X+5*cos(HandAngleRight*p/180),		 Y+5*sin(HandAngleRight*p/180), BLACK);
								 
				tft.fillTriangle(X+(AuxR+6)*cos((HandAngleTop-6)*p/180), 		 Y+(AuxR+6)*sin((HandAngleTop-6)*p/180), 
								 X+2*cos(HandAngleTop*p/180), 					 Y+2*sin(HandAngleTop*p/180), 
								 X+(AuxR+6)*cos((HandAngleTop+6)*p/180),		 Y+(AuxR+6)*sin((HandAngleTop+6)*p/180), BLACK);
			}
		} else {
			AngleDiff = HandAngle - HandAngleTop;
			if (AngleDiff <= 2) {
				//	CCW
				tft.drawLine(X+3*cos((HandAngleRight+1)*p/180), 	 Y+3*sin((HandAngleRight+1)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop+1)*p/180), Y+(AuxR+6)*sin((HandAngleTop+1)*p/180), BLACK);
				tft.drawLine(X+4*cos((HandAngleRight+2)*p/180), 	 Y+4*sin((HandAngleRight+2)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop+2)*p/180), Y+(AuxR+6)*sin((HandAngleTop+2)*p/180), BLACK);
				tft.drawLine(X+5*cos((HandAngleRight+3)*p/180), 	 Y+5*sin((HandAngleRight+3)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop+3)*p/180), Y+(AuxR+6)*sin((HandAngleTop+3)*p/180), BLACK);
				tft.drawLine(X+6*cos((HandAngleRight+4)*p/180), 	 Y+6*sin((HandAngleRight+4)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop+4)*p/180), Y+(AuxR+6)*sin((HandAngleTop+4)*p/180), BLACK);
				tft.drawLine(X+7*cos((HandAngleRight+5)*p/180), 	 Y+7*sin((HandAngleRight+5)*p/180), 
							 X+(AuxR+6)*cos((HandAngleTop+5)*p/180), Y+(AuxR+6)*sin((HandAngleTop+5)*p/180), BLACK);
			} else {
				tft.fillTriangle(X+5*cos(HandAngleLeft*p/180), 		 Y+5*sin(HandAngleLeft*p/180), 
								 X+(AuxR+6)*cos(HandAngleTop*p/180), Y+(AuxR+6)*sin(HandAngleTop*p/180), 
								 X+5*cos(HandAngleRight*p/180),		 Y+5*sin(HandAngleRight*p/180), BLACK);
								 
				tft.fillTriangle(X+(AuxR+6)*cos((HandAngleTop-6)*p/180), 		 Y+(AuxR+6)*sin((HandAngleTop-6)*p/180), 
								 X+2*cos(HandAngleTop*p/180), 					 Y+2*sin(HandAngleTop*p/180), 
								 X+(AuxR+6)*cos((HandAngleTop+6)*p/180),		 Y+(AuxR+6)*sin((HandAngleTop+6)*p/180), BLACK);
			}
		}
		
		//tft.fillTriangle(X+2*cos(HandAngleLeft*p/180), Y+2*sin(HandAngleLeft*p/180), X+AuxR*cos(HandAngleTop*p/180), Y+AuxR*sin(HandAngleTop*p/180), X+2*cos(HandAngleRight*p/180), Y+2*sin(HandAngleRight*p/180), BLACK);
		//tft.drawTriangle(X+2*cos(HandAngleLeft*p/180), Y+2*sin(HandAngleLeft*p/180), X+AuxR*cos(HandAngleTop*p/180), Y+AuxR*sin(HandAngleTop*p/180), X+2*cos(HandAngleRight*p/180), Y+2*sin(HandAngleRight*p/180), BLACK);

		HandAngleTop = HandAngle;
		
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
		
		tft.fillTriangle(X+2*cos(HandAngleLeft*p/180), Y+2*sin(HandAngleLeft*p/180), X+AuxR*cos(HandAngleTop*p/180), Y+AuxR*sin(HandAngleTop*p/180), X+2*cos(HandAngleRight*p/180), Y+2*sin(HandAngleRight*p/180), WHITE);

		tft.fillCircle(X, Y, 2, WHITE);



		AuxR = R-7;
		for (i = AngleStart; i < AngleEnd+5; i += PartAngle) {
			tft.drawLine(X+AuxR*cos(i*p/180), Y+AuxR*sin(i*p/180), X+R*cos(i*p/180), Y+R*sin(i*p/180), NUMERIC_POINT);
		}
	}

	
}


void GaugeSetHand() {
	
	
}



static inline void fps(const int seconds, const int x, const int y){

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





/*
void calc_SecondHand()
{
  float angle; // in radian
  //  int Xa, Ya, Xb, Yb;
  angle =  0.785;
  ps.a.x = Xo + (Hand_LEN) * cos(angle);
  ps.a.y = Yo + (Hand_LEN) * sin(angle);
  angle += 3.142; // +180 degree
  ps.b.x = Xo + (Hand_TAIL) * cos(angle);
  ps.b.y = Yo + (Hand_TAIL) * sin(angle);
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





void calc_Hands(HAND_SET &hs, TME t)
{
  calc_SecondHand(t, hs.Sec);
}

*/


















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
