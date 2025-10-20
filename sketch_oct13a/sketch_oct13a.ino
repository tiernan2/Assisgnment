const int buttonPin = A2; //Assigns the Button to port A2
const int ledPin = A0; //Assigns the LED to port A0
const int buzzerPin = 4; //Assigns the buzzer to port D4

bool lastButtonState = HIGH; //boolean values must be either true or false.(The output is either on or its not, no inbetween)
bool waitingToStart = false;
bool outputOn = false;

unsigned long pressTime = 0;  //ensures a positive number is stored
unsigned long outputStartTime = 0;

void setup() { //This is the defualt start position. there is no light or buzzer on
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW); // led off
  noTone(buzzerPin); // buzzer off

  Serial.begin(9600); //
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);

  // Detect button press (HIGH to LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    pressTime = millis();
    waitingToStart = true; //shows the code is waiting for a follow up action
    outputOn = false;

    digitalWrite(ledPin, LOW); //led still off
    noTone(buzzerPin); // buzzer still off
  }

  // After 5 seconds from button press, turn on LED and buzzer
  if (waitingToStart && (millis() - pressTime >= 5000)) { // waits 5 seconds
    Serial.println("Light is Green, safe to cross"); //text appears in the serial monitor to indicate the start of the cycle
    digitalWrite(ledPin, HIGH); //led comes on
    tone(buzzerPin, 1000); // buzzer comes on
    outputStartTime = millis();
    outputOn = true;
    waitingToStart = false; // no longer waiting to start
  }

  // Turn off LED and buzzer after 8 seconds ON
  if (outputOn && (millis() - outputStartTime >= 8000)) { 
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    outputOn = false;
  }

  lastButtonState = currentButtonState; // this code detects breaks between the button being pressed 
}
