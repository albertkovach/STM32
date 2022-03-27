#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define tft_CS     PA4
#define tft_RST    PA1
#define tft_DC     PA0
#define tft_LED    PB9

Adafruit_ST7735 tft = Adafruit_ST7735(tft_CS, tft_DC, tft_RST);

#define BtnUpPin  PB14
#define BtnOkPin  PB13
#define BtnDwPin  PB12


bool tftledMode = 1;
bool tftdimUp = 1;
int tftdimValue = 0;


int BtnUpState = 0;
int BtnUpOldState = 1;
int BtnOkState = 0;
int BtnOkOldState = 1;
int BtnDwState = 0;
int BtnDwOldState = 1;

bool ScrollBack = true;


unsigned int SelectedScreen = 10000;
unsigned int OldSelectedScreen;

int Screen1levelAmount = 5;
int Screen1levelDigit = 10000;
int Screen1levelSelected = 1;

int Screen2levelAmount = 10;
int Screen2levelDigit = 100;
int Screen2levelSelected = 0;

int Screen3levelAmount = 15;
int Screen3levelDigit = 1;
int Screen3levelSelected = 0;

int SelectedScreenDecode;

bool Screen1levelUpClick = false;
bool Screen2levelUpClick = false;
bool Screen3levelUpClick = false;

bool Screen1levelOkClick = false;
bool Screen2levelOkClick = false;
bool Screen3levelOkClick = false;

bool Screen1levelDwClick = false;
bool Screen2levelDwClick = false;
bool Screen3levelDwClick = false;

bool ScreenNeedFillBlack1level = true;
bool ScreenNeedFillBlack2level = false;
bool ScreenNeedFillBlack3level = false;

int GUIMenuElementSize = 20;
int GUIoffset = 5;


void setup(void) {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  
	pinMode(BtnUpPin, INPUT_PULLUP);
	pinMode(BtnOkPin, INPUT_PULLUP);
	pinMode(BtnDwPin, INPUT_PULLUP);
}

void loop() {
	UpdateScreen();
}


void UpdateScreen() {
	BtnHandler();
	ShowNum();
	fps(0.25, 0, 120);
}


void ShowNum() {
	if (SelectedScreen != OldSelectedScreen) {
		tft.setFont();
		tft.setTextSize(2);

		int16_t x1, y1;
		uint16_t w, h;
		tft.getTextBounds(String(SelectedScreen), 0, 0, &x1, &y1, &w, &h);
		tft.fillRect(0, 0, w, h, 0x0000);
		
		tft.setCursor(0, 0);
		tft.print(SelectedScreen);
		OldSelectedScreen = SelectedScreen;
	}
}



/*void RefreshScreen() {
	Screen1levelSelected = SelectedScreen / Screen1levelDigit;
	SelectedScreenDecode = SelectedScreen - (Screen1levelSelected * Screen1levelDigit);
	
	if (SelectedScreenDecode == 0) {
		// 1st level
			switch (Screen1levelSelected) {
				case 1:
					ScreenECMroad();
					break;
				case 2:
					ScreenTCMroad();
					break;
			}
	} else {
		SelectedScreenDecode = SelectedScreenDecode - (SelectedScreenDecode / Screen2levelDigit * Screen2levelDigit);
		if (SelectedScreenDecode == 0) {
			// 2nd level
			switch (Screen1levelSelected) {
				case 1:
					ScreenCMsMenu(ECMmenuHeader);
					break;
				case 2:
					ScreenCMsMenu(TCMmenuHeader);
					break;
			}
		} else {
			// 3rd level
			if (Screen1levelSelected == 1) {
				switch (Screen2levelSelected) {
					case 1:
						ScreenLevelUp(1);
						break;
					case 2:
						ScreenDataList(ECMsensHeader, ArrayECMsens, ArrayECMsensLen);
						break;
					case 3:
						ScreenDataList(ECMbitmasksHeader, ArrayECMbitmasks, ArrayECMbitmasksLen);
						break;
					case 4:
						ScreenDataList(ECMerrorsHeader, ArrayECMerrors, ArrayECMerrorsLen);
						break;
				}
			} else if (Screen1levelSelected == 2) {
				switch (Screen2levelSelected) {
					case 1:
						ScreenLevelUp(1);
						break;
					case 2:
						ScreenDataList(TCMsensHeader, ArrayTCMsens, ArrayTCMsensLen);
						break;
					case 3:
						ScreenDataList(TCMbitmasksHeader, ArrayTCMbitmasks, ArrayTCMbitmasksLen);
						break;
					case 4:
						ScreenDataList(TCMerrorsHeader, ArrayTCMerrors, ArrayTCMerrorsLen);
						break;
				}
			}
		}
	}

}




void ScreenLevelUp(byte LevelToReturn) {
	switch (LevelToReturn) {
		case 1:
			SelectedScreen = SelectedScreen / Screen1levelDigit * Screen1levelDigit;
			Screen2levelSelected = 0;
			Screen3levelSelected = 0;
			break;
		case 2:
			SelectedScreen = SelectedScreen / Screen2levelDigit * Screen2levelDigit;
			Screen3levelSelected = 0;
			break;
	}
	tft.fillScreen(ST77XX_BLACK);
}*/







void BtnUpClickEvent() {
	// up
	Screen1levelSelected = SelectedScreen / Screen1levelDigit;
	SelectedScreenDecode = SelectedScreen - (Screen1levelSelected * Screen1levelDigit);
	// SelectedScreenDecode m.11.22 -> 11.22
	
	if (SelectedScreenDecode == 0) {
		// 1st level
		if (ScrollBack == true) {
			if (Screen1levelSelected == Screen1levelAmount) {
				SelectedScreen = SelectedScreen - (Screen1levelSelected * Screen1levelDigit) + Screen1levelDigit;
				Screen1levelSelected =  1;
			} else {
				if (Screen1levelSelected < Screen1levelAmount) {
					SelectedScreen = SelectedScreen + Screen1levelDigit;
					Screen1levelSelected = Screen1levelSelected + 1;
				}
			}
		} else {
			if (Screen1levelSelected < Screen1levelAmount) {
				SelectedScreen = SelectedScreen + Screen1levelDigit;
				Screen1levelSelected = Screen1levelSelected + 1;
			}
		}
		
		if (ScreenNeedFillBlack1level) tft.fillScreen(ST77XX_BLACK);
		Screen1levelUpClick = true;

	} else {
		Screen2levelSelected = SelectedScreenDecode / Screen2levelDigit;
		SelectedScreenDecode = SelectedScreenDecode - (Screen2levelSelected * Screen2levelDigit);
		// SelectedScreenDecode 11.22 -> last 22
		
		if (SelectedScreenDecode == 0) {
			// 2nd level
			
			if (ScrollBack == true) {
				if (Screen2levelSelected == Screen2levelAmount) {
					SelectedScreen = SelectedScreen - (Screen2levelSelected * Screen2levelDigit) + Screen2levelDigit;
					Screen2levelSelected =  1;
				} else {
					if (Screen2levelSelected < Screen2levelAmount) {
						SelectedScreen = SelectedScreen + Screen2levelDigit;
						Screen2levelSelected = Screen2levelSelected + 1;
					}
				}
			} else {
				if (Screen2levelSelected < Screen2levelAmount) {
					SelectedScreen = SelectedScreen + Screen2levelDigit;
					Screen2levelSelected = Screen2levelSelected + 1;
				}
			}
			
			if (ScreenNeedFillBlack2level) tft.fillScreen(ST77XX_BLACK);
			Screen2levelUpClick = true;
		
		} else {
			// 3rd level
			Screen3levelSelected = SelectedScreenDecode;
			
			if (ScrollBack == true) {
				if (Screen3levelSelected == Screen3levelAmount) {
					SelectedScreen = SelectedScreen - (Screen3levelSelected * Screen3levelDigit) + Screen3levelDigit;
					Screen3levelSelected =  1;
				} else {
					if (Screen3levelSelected < Screen3levelAmount) {
						SelectedScreen = SelectedScreen + Screen3levelDigit;
						Screen3levelSelected = Screen3levelSelected + 1;
					}
				}
			} else if (Screen3levelSelected < Screen3levelAmount) {
					SelectedScreen = SelectedScreen + Screen3levelDigit;
					Screen3levelSelected = Screen3levelSelected + 1;
			}
			
			if (ScreenNeedFillBlack3level) tft.fillScreen(ST77XX_BLACK);
			Screen3levelUpClick = true;
		}
	}	
}



void BtnOkClickEvent() { 
	// ok
	Screen1levelSelected = SelectedScreen / Screen1levelDigit;
	SelectedScreenDecode = SelectedScreen - (Screen1levelSelected * Screen1levelDigit);
	
	if (SelectedScreenDecode == 0) {
		// 1st level
		SelectedScreen = SelectedScreen + Screen2levelDigit;
		Screen2levelSelected = 1;
		tft.fillScreen(ST77XX_BLACK);
		Screen1levelOkClick = true;
	} else {
		SelectedScreenDecode = SelectedScreenDecode - (SelectedScreenDecode / Screen2levelDigit * Screen2levelDigit);

		if (SelectedScreenDecode == 0) {
			// 2nd level
			SelectedScreen = SelectedScreen + Screen3levelDigit;
			Screen3levelSelected = 1;
			tft.fillScreen(ST77XX_BLACK);
			Screen2levelOkClick = true;
		} else {
			// 3rd level
			Screen3levelOkClick = true;
		}
	}	
}



void BtnDwClickEvent() {
	// down
	Screen1levelSelected = SelectedScreen / Screen1levelDigit;
	SelectedScreenDecode = SelectedScreen - (Screen1levelSelected * Screen1levelDigit);

	if (SelectedScreenDecode == 0) {
		// 1st level

		if (ScrollBack = true) {
			if (Screen1levelSelected == 1) {
				SelectedScreen = Screen1levelDigit * Screen1levelAmount;
				Screen1levelSelected = Screen1levelAmount;
			} else {
				if (Screen1levelSelected > 1) {
					SelectedScreen = SelectedScreen - Screen1levelDigit;
					Screen1levelSelected = Screen1levelSelected - 1;
				}
			}
		} else {
			if (Screen1levelSelected > 1) {
				SelectedScreen = SelectedScreen - Screen1levelDigit;
				Screen1levelSelected = Screen1levelSelected - 1;
			}
		}
		
		if (ScreenNeedFillBlack1level) tft.fillScreen(ST77XX_BLACK);
		Screen1levelDwClick = true;

	} else {
		Screen2levelSelected = SelectedScreenDecode / Screen2levelDigit;
		SelectedScreenDecode = SelectedScreenDecode - (Screen2levelSelected * Screen2levelDigit);

		if (SelectedScreenDecode == 0) {
			// 2nd level
			
			if (ScrollBack = true) {
				if (Screen2levelSelected == 1) {
					SelectedScreen = SelectedScreen + (Screen2levelDigit * (Screen2levelAmount-1));
					Screen2levelSelected = Screen2levelAmount;
				} else {
					if (Screen2levelSelected > 1) {
						SelectedScreen = SelectedScreen - Screen2levelDigit;
						Screen2levelSelected = Screen2levelSelected - 1;
					}
				}
			} else {
				if (Screen2levelSelected > 1) {
					SelectedScreen = SelectedScreen - Screen2levelDigit;
					Screen2levelSelected = Screen2levelSelected - 1;
				}
			}
			
			if (ScreenNeedFillBlack2level) tft.fillScreen(ST77XX_BLACK);
			Screen2levelDwClick = true;
		
		} else {
			// 3rd level
			Screen3levelSelected = SelectedScreenDecode;
			
			if (ScrollBack = true) {
				if (Screen3levelSelected == 1) {
					SelectedScreen = SelectedScreen + (Screen3levelDigit * (Screen3levelAmount-1));
					Screen3levelSelected = Screen3levelAmount;
				} else {
					if (Screen3levelSelected > 1) {
						SelectedScreen = SelectedScreen - Screen3levelDigit;
						Screen3levelSelected = Screen3levelSelected - 1;
					}
				}
			} else {
				if (Screen3levelSelected > 1) {
					SelectedScreen = SelectedScreen - Screen3levelDigit;
					Screen3levelSelected = Screen3levelSelected - 1;
				}
			}

			if (ScreenNeedFillBlack3level) tft.fillScreen(ST77XX_BLACK);
			Screen3levelDwClick = true;
		}
	}
}





void BtnHandler() {
	BtnUpState = digitalRead(BtnUpPin);
	BtnOkState = digitalRead(BtnOkPin);
	BtnDwState = digitalRead(BtnDwPin);

	if (BtnUpState == 0) {
		if (BtnUpState != BtnUpOldState) {
			BtnUpClickEvent();
			BtnUpOldState = BtnUpState;
		}
	} else {
		BtnUpOldState = 1;
	}
	
	if (BtnOkState == 0) {
		if (BtnOkState != BtnOkOldState) {
			BtnOkClickEvent();
			BtnOkOldState = BtnOkState;
		}
	} else {
		BtnOkOldState = 1;
	}
	
	if (BtnDwState == 0) {
		if (BtnDwState != BtnDwOldState) {
			BtnDwClickEvent();
			BtnDwOldState = BtnDwState;
      }
	} else {
		BtnDwOldState = 1;
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
	  tft.setTextSize(1);
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