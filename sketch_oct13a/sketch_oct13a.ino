#include <WiFi.h>


const int buttonPin = A2;
const int ledPin = A0;
const int buzzerPin = 4;

// WIFI SETTINGS 
const char* ssid     = "IOT-MPSK ";
const char* password = "wlzjlzns";

// VARIABLES --------------------
bool lastButtonState = HIGH;
bool waitingToStart = false;
bool outputOn = false;

unsigned long pressTime = 0;
unsigned long outputStartTime = 0;


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

void loop()
