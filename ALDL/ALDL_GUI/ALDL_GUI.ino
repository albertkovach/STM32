#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Fonts/FreeSans9pt7b.h>


#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

Adafruit_ST7735 TFT = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//	https://githubmemory.com/repo/newdigate/rgb565_colors
//	https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
//  ArylideYellow 0xE68D
#define Grey 0xAD55
#define Yellow 0xE68D



bool ScrollBack = true;

int Btn1Pin = 2;
int Btn1State = 0;
int Btn1OldState = 1;

int Btn2Pin = 3;
int Btn2State = 0;
int Btn2OldState = 1;

int Btn3Pin = 4;
int Btn3State = 0;
int Btn3OldState = 1;



float RPM = 0;
float TPS = 0;
float Speed = 0;
float EngineTemp = 0;
float SparkAdv = 0;
int Integrator = 95;
float InjBPW = 0;
float Batt = 14.8;
float MAT = 0;
float FuelConsmp = 0;

int RefreshRate = 5;




unsigned int SelectedScreen = 10000;

int Screen1levelAmount = 2;
int Screen1levelDigit = 10000;
int Screen1levelSelected = 1;

int Screen2levelAmount = 4;
int Screen2levelDigit = 100;
int Screen2levelSelected = 0;

int Screen3levelAmount = 12;
int Screen3levelDigit = 1;
int Screen3levelSelected = 0;

int SelectedScreenDecode;
bool Screen3levelClick = false;

bool ScreenNeedFillBlack1level = true;
bool ScreenNeedFillBlack2level = true;
bool ScreenNeedFillBlack3level = false;

int GUIMenuElementSize = 20;
int GUIoffset = 5;



String ECMsensHeader = "  ===== ECM sensors =====";
String ArrayECMsens[] =  {"< Exit >","Speed","RPM","Coolant temp:","Manifold air:","MAP","Barometric","Lambda"};
int ArrayECMsensLen = 8;

String ECMbitmasksHeader = "  ===== ECM bitmasks =====";
String ArrayECMbitmasks[] =  {"< Exit >","Bit 1","Bit 2","Bit 3","Bit 4"};
int ArrayECMbitmasksLen = 5;

String ECMerrorsHeader = "  ===== ECM errors =====";
String ArrayECMerrors[] =  {"< Exit >","Code 1","Code 2","Code 3","Code 4","Code 5"};
int ArrayECMerrorsLen = 6;


String TCMsensHeader = "  ===== TCM sensors =====";
String ArrayTCMsens[] =  {"< Exit >","01","02","03"};
int ArrayTCMsensLen = 4;

String TCMbitmasksHeader = "  ===== TCM bitmasks =====";
String ArrayTCMbitmasks[] =  {"< Exit >","Bit 1","Bit 2","Bit 3","Bit 4"};
int ArrayTCMbitmasksLen = 5;

String TCMerrorsHeader = "  ===== TCM errors =====";
String ArrayTCMerrors[] =  {"< Exit >","Code 1","Code 2","Code 3","Code 4","Code 5"};
int ArrayTCMerrorsLen = 6;



void setup() {
	pinMode(Btn1Pin, INPUT_PULLUP);
	pinMode(Btn2Pin, INPUT_PULLUP);
	pinMode(Btn3Pin, INPUT_PULLUP);

	TFTinit();
}



void loop() {  
	BtnHandler();
	Increment();
	RefreshScreen();
}









void TFTinit() {
	TFT.initR(INITR_BLACKTAB);
	TFT.fillScreen(ST77XX_BLACK);
	TFT.setTextWrap(false);
	
	TFT.setRotation(3);
	TFT.setFont();
	TFT.setTextSize(1);
	TFT.setTextColor(Yellow, ST7735_BLACK);
	TFT.cp437(true);
}


void RefreshScreen() {
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
					ScreenECMmenu();
					break;
				case 2:
					ScreenTCMmenu();
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
						ScreenArray(ECMsensHeader, ArrayECMsens, ArrayECMsensLen);
						break;
					case 3:
						ScreenArray(ECMbitmasksHeader, ArrayECMbitmasks, ArrayECMbitmasksLen);
						break;
					case 4:
						ScreenArray(ECMerrorsHeader, ArrayECMerrors, ArrayECMerrorsLen);
						break;
				}
			} else if (Screen1levelSelected == 2) {
				switch (Screen2levelSelected) {
					case 1:
						ScreenLevelUp(1);
						break;
					case 2:
						ScreenArray(TCMsensHeader, ArrayTCMsens, ArrayTCMsensLen);
						break;
					case 3:
						ScreenArray(TCMbitmasksHeader, ArrayTCMbitmasks, ArrayTCMbitmasksLen);
						break;
					case 4:
						ScreenArray(TCMerrorsHeader, ArrayTCMerrors, ArrayTCMerrorsLen);
						break;
				}
			}
		}
	}

}







void ScreenECMroad() {
	TFT.setFont();
	TFT.setTextSize(2);

  // "1250 RPM  18%"
  TFT.setCursor(0, 0);
  if (RPM < 10) { TFT.print("   "); }
  if (RPM >= 10 && RPM < 100) { TFT.print("  "); }
  if (RPM >= 100 && RPM < 1000) { TFT.print(" "); }
  TFT.print(RPM, 0);
  TFT.print(" RPM");
  if (TPS < 10) { TFT.print("   "); }
  if (TPS >= 10 && TPS < 100) { TFT.print("  "); }
  if (TPS >= 100) { TFT.print(" "); }
  TFT.print(TPS, 0);
  TFT.print("%");

  // RPM BAR
  /*int RPMbarWidth = RPM / 55 * 1.6; //160 = 100, 5500 = 100; 1.6 = 55
  TFT.drawRect(0, 20, 160, 10, Grey);
  TFT.fillRect(1, 21, 158, 8, ST7735_BLACK);
  TFT.fillRect(1, 21, RPMbarWidth-2, 8, Yellow);*/

  // "98 C 110 kmh"
  TFT.setCursor(0, 40);
  if (EngineTemp < 10) { TFT.print("  "); }
  if (EngineTemp >= 10 && EngineTemp < 100) { TFT.print(" "); }
  if (EngineTemp >= 100) { TFT.print(""); }
  TFT.print(EngineTemp, 0);
  TFT.print(" C");
  //TFT.drawChar(37, 40, 0xF8, Yellow, ST7735_BLACK, 2);
  if (Speed < 10) { TFT.print("   "); }
  if (Speed >= 10 && Speed < 100) { TFT.print("  "); }
  if (Speed >= 100) { TFT.print(" "); }
  TFT.print(Speed, 0);
  TFT.print(" kmh");
  //TFT.print(" 98 C 110 kmh");

  // "ad 22.5 d"
  TFT.drawLine(0, 60, 160, 60, Grey);
  TFT.setCursor(0, 66);
  TFT.print(" ad");
  if (SparkAdv < 10) { TFT.print("  "); TFT.print(SparkAdv, 1); }
  if (SparkAdv >= 10 && SparkAdv < 60) { TFT.print(" "); TFT.print(SparkAdv, 1); }
  if (SparkAdv >= 60) { TFT.print("  ERR"); }
  TFT.print(" deg");


  TFT.drawLine(0, 86, 160, 86, Grey);
  TFT.setCursor(0, 92);
  TFT.print(" ");
  if (Integrator < 100) { TFT.print(" "); }
  TFT.print(Integrator);
  if (FuelConsmp < 10) { TFT.print("    "); }
  if (FuelConsmp >= 10 && FuelConsmp < 100) { TFT.print("   "); }
  if (FuelConsmp >= 100) { TFT.print("  "); }
  TFT.print(FuelConsmp, 1);
  TFT.print(" l");
}

void ScreenTCMroad() {
	TFT.setTextSize(1);
	TFT.setCursor(20, 0);

	TFT.print("Transmition road");
}





void ScreenECMmenu() {
	TFT.setTextSize(1);
	TFT.setCursor(20, 0);
	TFT.print("===== TCM data =====");

	TFT.setCursor(0, GUIMenuElementSize+GUIoffset);

	TFT.print("Exit");
	TFT.setCursor(0, GUIMenuElementSize*2+GUIoffset);

	TFT.print("Sensors");
	TFT.setCursor(0, GUIMenuElementSize*3+GUIoffset);

	TFT.print("Bitmasks");
	TFT.setCursor(0, GUIMenuElementSize*4+GUIoffset);

	TFT.print("Errors");
	
	
	
	TFT.drawLine(0, (Screen2levelSelected)*GUIMenuElementSize+GUIMenuElementSize, 160, (Screen2levelSelected)*GUIMenuElementSize+GUIMenuElementSize, Grey);
	TFT.drawLine(0, (Screen2levelSelected)*GUIMenuElementSize, 160, (Screen2levelSelected)*GUIMenuElementSize, Grey);
}


void ScreenTCMmenu() {
	TFT.setTextSize(1);
	TFT.setCursor(20, 0);
	TFT.print("===== TCM data =====");

	TFT.setCursor(0, GUIMenuElementSize+GUIoffset);

	TFT.print("Exit");
	TFT.setCursor(0, GUIMenuElementSize*2+GUIoffset);

	TFT.print("Sensors");
	TFT.setCursor(0, GUIMenuElementSize*3+GUIoffset);

	TFT.print("Bitmasks");
	TFT.setCursor(0, GUIMenuElementSize*4+GUIoffset);

	TFT.print("Errors");
	
	
	
	TFT.drawLine(0, (Screen2levelSelected)*GUIMenuElementSize+GUIMenuElementSize, 160, (Screen2levelSelected)*GUIMenuElementSize+GUIMenuElementSize, Grey);
	TFT.drawLine(0, (Screen2levelSelected)*GUIMenuElementSize, 160, (Screen2levelSelected)*GUIMenuElementSize, Grey);
}





void ArraysHandler() {
	
}


void Carousel(String strong[]) {
	int  x1, y1;
	int w, h;
	//TFT.getTextBounds(strong[Screen3levelSelected-3], 0, 0, &x1, &y1, &w, &h);
	
	bool FillEmptySpace = true;

	if(Screen3levelSelected > 2){
		TFT.print(strong[Screen3levelSelected-3]);
		if (FillEmptySpace) TFT.print("           ");
	} else {
		if (FillEmptySpace) TFT.print("                    ");
	}
	
	TFT.setCursor(0, GUIMenuElementSize*2+GUIoffset);
	
	if(Screen3levelSelected >= 2) {
		TFT.print(strong[Screen3levelSelected-2]);
		if (FillEmptySpace) TFT.print("           ");
	} else {
		if (FillEmptySpace) TFT.print("                    ");
	}
	
	
	TFT.setCursor(0, GUIMenuElementSize*3+GUIoffset);
	TFT.print(strong[Screen3levelSelected-1]);
	if (FillEmptySpace) TFT.print("                  ");
	TFT.setCursor(0, GUIMenuElementSize*4+GUIoffset);
	
	
	if(Screen3levelSelected <= Screen3levelAmount-1){
		TFT.print(strong[Screen3levelSelected]);
		if (FillEmptySpace) TFT.print("           ");
	} else {
		if (FillEmptySpace) TFT.print("                    ");
	}
		
	TFT.setCursor(0, GUIMenuElementSize*5+GUIoffset);
	
	if(Screen3levelSelected < Screen3levelAmount-1){
		TFT.print(strong[Screen3levelSelected+1]);
		if (FillEmptySpace) TFT.print("           ");
	} else {
		if (FillEmptySpace) TFT.print("                    ");
	}
	
	
	TFT.drawLine(0, 3*GUIMenuElementSize+GUIMenuElementSize, 160, 3*GUIMenuElementSize+GUIMenuElementSize, Grey);
	TFT.drawLine(0, 3*GUIMenuElementSize, 160, 3*GUIMenuElementSize, Grey);
}



void ScreenArray(String Header, String Array[], int ArrayLenght) {
	Screen3levelAmount = ArrayLenght;
	//Screen3levelAmount = sizeof(Array) / sizeof(String);

	TFT.setCursor(0, 0);
	TFT.print(Header);
	TFT.setCursor(0, GUIMenuElementSize+GUIoffset);
	
	Carousel(Array);
	
	if (Screen3levelClick == true && Screen3levelSelected == 1) {
		Screen3levelClick = false;
		ScreenLevelUp(2);
	} else {
		Screen3levelClick = false;
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
	TFT.fillScreen(ST77XX_BLACK);
}




void Btn1ClickEvent() {
	// ok
	Screen1levelSelected = SelectedScreen / Screen1levelDigit;
	SelectedScreenDecode = SelectedScreen - (Screen1levelSelected * Screen1levelDigit);
	
	if (SelectedScreenDecode == 0) {
		// 1st level
		SelectedScreen = SelectedScreen + Screen2levelDigit;
		Screen2levelSelected = 1;
		TFT.fillScreen(ST77XX_BLACK);
	} else {
		SelectedScreenDecode = SelectedScreenDecode - (SelectedScreenDecode / Screen2levelDigit * Screen2levelDigit);

		if (SelectedScreenDecode == 0) {
			// 2nd level
			SelectedScreen = SelectedScreen + Screen3levelDigit;
			Screen3levelSelected = 1;
			TFT.fillScreen(ST77XX_BLACK);
		} else {
			// 3rd level
			Screen3levelClick = true;
		}
	}
	
}




void Btn2ClickEvent() { 
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
		
		if (ScreenNeedFillBlack1level) TFT.fillScreen(ST77XX_BLACK);

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
			
			if (ScreenNeedFillBlack2level) TFT.fillScreen(ST77XX_BLACK);
		
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
			
			if (ScreenNeedFillBlack3level) TFT.fillScreen(ST77XX_BLACK);
		}
	}
	
	
}




void Btn3ClickEvent() {
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
		
		if (ScreenNeedFillBlack1level) TFT.fillScreen(ST77XX_BLACK);

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
			
			if (ScreenNeedFillBlack2level) TFT.fillScreen(ST77XX_BLACK);
		
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

			if (ScreenNeedFillBlack3level) TFT.fillScreen(ST77XX_BLACK);
		}
	}

}









void Increment() {
  RPM = RPM + 150;
  if (RPM > 5500) RPM = 0;

  TPS = TPS + 5;
  if (TPS > 100) TPS = 0;

  Speed = Speed + 10;
  if (Speed > 170) Speed = 0;

  EngineTemp = EngineTemp + 15;
  if (EngineTemp > 130) EngineTemp = 0;

  SparkAdv = SparkAdv + 8.5;
  if (SparkAdv > 70) SparkAdv = 0;

  Integrator = Integrator + 15;
  if (Integrator > 180) Integrator = 95;

  FuelConsmp = FuelConsmp + 5;
  if (FuelConsmp > 110) FuelConsmp = 0;

}


void BtnHandler() {
	Btn1State = digitalRead(Btn1Pin);
	Btn2State = digitalRead(Btn2Pin);
	Btn3State = digitalRead(Btn3Pin);

	if (Btn1State == 0) {
		if (Btn1State != Btn1OldState) {
			Btn1ClickEvent();
			Btn1OldState = Btn1State;
		}
	} else {
		Btn1OldState = 1;
	}
	
	if (Btn2State == 0) {
		if (Btn2State != Btn2OldState) {
			Btn2ClickEvent();
			Btn2OldState = Btn2State;
		}
	} else {
		Btn2OldState = 1;
	}
	
	if (Btn3State == 0) {
		if (Btn3State != Btn3OldState) {
			Btn3ClickEvent();
			Btn3OldState = Btn3State;
      }
	} else {
		Btn3OldState = 1;
	}

}
