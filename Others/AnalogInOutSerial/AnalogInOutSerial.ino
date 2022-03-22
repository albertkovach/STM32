#define analogInPin     PA2
int sensorValue = 0;


void setup() {
  pinMode(analogInPin, INPUT_ANALOG);
  Serial.begin(9600);
}

void loop() {
    sensorValue = analogRead(analogInPin);

    Serial.print("sensor = ");
    Serial.println(sensorValue);
    Serial.println("================");
    delay (100);
}
