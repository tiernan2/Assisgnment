const int buttonPin = A2;
const int ledPin = A0;
const int buzzerPin = 4;

bool lastButtonState = HIGH;
bool waitingToStart = false;
bool outputOn = false;

unsigned long pressTime = 0;
unsigned long outputStartTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);

  Serial.begin(9600);
  Serial.println("red light");
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);

  // Detect button press (HIGH to LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    pressTime = millis();
    waitingToStart = true;
    outputOn = false;

    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  // After 5 seconds from button press, turn on LED and buzzer
  if (waitingToStart && (millis() - pressTime >= 5000)) {
    Serial.println("Button was pressed!");
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000); // 1kHz tone
    outputStartTime = millis();
    outputOn = true;
    waitingToStart = false;
  }

  // Turn off LED and buzzer after 8 seconds ON
  if (outputOn && (millis() - outputStartTime >= 8000)) {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    outputOn = false;
  }

  lastButtonState = currentButtonState;
}