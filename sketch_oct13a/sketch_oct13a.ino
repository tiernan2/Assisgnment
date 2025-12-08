#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// ------------------ Pins ------------------
const int buttonPin = A2;
const int ledPin = A0;
const int buzzerPin = 4;

// Button logic
bool lastButtonState = HIGH;
bool waitingToStart = false;
bool outputOn = false;

unsigned long pressTime = 0;
unsigned long outputStartTime = 0;

// ThingSpeak
const char* ssid = "IOT-MPSK";
const char* password = "wlzjlzns";
const char* host = "api.thingspeak.com";
const int httpPort = 80;
const char* writeAPIKey = "2TBFXR5AXV5F1RIF";

unsigned long cycleCount = 0;
unsigned long lastUploadTime = 0;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, host, httpPort);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  delay(1000);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendToThingSpeak(unsigned long count) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi dropped — reconnecting...");
    WiFi.begin(ssid, password);
    return;
  }

  String url = "/update?api_key=";
  url += writeAPIKey;
  url += "&field1=";
  url += String(count);

  Serial.print("Sending to ThingSpeak: ");
  Serial.println(count);

  client.beginRequest();
  client.get(url);
  client.sendHeader("Host", host);
  client.sendHeader("Connection", "close");
  client.endRequest();

  int status = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("HTTP Status: ");
  Serial.println(status);
  Serial.print("Response: ");
  Serial.println(response);
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    pressTime = millis();
    waitingToStart = true;
    outputOn = false;

    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  if (waitingToStart && (millis() - pressTime >= 5000)) {
    Serial.println("Button was Pressed! Light is Green, safe to cross");

    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000);

    outputStartTime = millis();
    outputOn = true;
    waitingToStart = false;

    cycleCount++;

    if (millis() - lastUploadTime > 15000) {
      sendToThingSpeak(cycleCount);
      lastUploadTime = millis();
    }
  }

  if (outputOn && (millis() - outputStartTime >= 8000)) {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    outputOn = false;
  }

  lastButtonState = currentButtonState;
}
