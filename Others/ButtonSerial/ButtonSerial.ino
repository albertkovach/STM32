

#define BtnUpPin  PB12
#define BtnOkPin  PB13
#define BtnDwPin  PB14

int BtnUpState = 0;
int BtnUpOldState = 1;
int BtnOkState = 0;
int BtnOkOldState = 1;
int BtnDwState = 0;
int BtnDwOldState = 1;



void setup() {
	pinMode(BtnUpPin, INPUT_PULLUP);
	pinMode(BtnOkPin, INPUT_PULLUP);
	pinMode(BtnDwPin, INPUT_PULLUP);
	
	Serial.begin(9600);
}



void loop() {  
	SerialHandler();
	delay(50);
}


void SerialHandler() {
	BtnUpState = digitalRead(BtnUpPin);
	BtnOkState = digitalRead(BtnOkPin);
	BtnDwState = digitalRead(BtnDwPin);	
	
    if (Serial.available() > 0) {
        Serial.println(BtnUpState);
        Serial.println(BtnOkState);
        Serial.println(BtnDwState);
		Serial.println("=====");
    }
	
}


