#include <WiFi.h>
#include <ArduinoHttpClient.h>
const int buttonPin = A2;
const int ledPin = A0;
const int buzzerPin = 4;

// WIFI SETTINGS 
const char* ssid     = "IOT-MPSK";
const char* password = "wlzjlzns";

// GOOGLE SCRIPT URL
String url = "https://script.google.com/macros/s/AKfycbzIlwvIoNn7SmkJthgTUDMSKM2QHDOrHHHjyfzOJ58NzkH7R3qOarxYrEo7nBk1yAnp/exec";

// VARIABLES --------------------
bool lastButtonState = HIGH;
bool waitingToStart = false;
bool outputOn = false;

unsigned long pressTime = 0;
unsigned long outputStartTime = 0;

unsigned long debounceTime = 0;
const unsigned long debounceDelay = 50; // ms

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendButtonPress() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected — cannot send.");
    return;
  }

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"button\":\"pressed\"}";
  int response = http.POST(json);

  Serial.print("POST Response: ");
  Serial.println(response);

  http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);

  connectToWiFi();
  Serial.println("Setup complete.");
  Serial.println("Waiting for button input...");
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Debounce
  if (reading != lastButtonState) {
    debounceTime = millis();
  }

  if (millis() - debounceTime > debounceDelay) {

    // Detect a NEW press (HIGH → LOW)
    if (reading == LOW && lastButtonState == HIGH) {
      Serial.println("BUTTON PRESSED!");

      digitalWrite(ledPin, HIGH);
      tone(buzzerPin, 1000, 100); // short beep

      sendButtonPress();  // SEND ONLY ONCE PER PRESS
    }

    // Detect release (LOW → HIGH)
    if (reading == HIGH && lastButtonState == LOW) {
      digitalWrite(ledPin, LOW);
    }
  }

  lastButtonState = reading;
}
