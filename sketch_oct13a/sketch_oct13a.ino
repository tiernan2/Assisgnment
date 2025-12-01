#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <ArduinoHttpClient.h>

// ------------------ Pins ------------------
const int buttonPin = A2;
const int ledPin = A0;
const int buzzerPin = 4;

// ------------------ Button/Buzzer Logic ------------------
bool lastButtonState = HIGH;
bool waitingToStart = false;
bool outputOn = false;

unsigned long pressTime = 0;
unsigned long outputStartTime = 0;

// ------------------ ThingSpeak ------------------
const char* ssid = "IOT-MPSK";       // Remove trailing space
const char* password = "ffcvqjjt";   
const char* host = "api.thingspeak.com";
const int httpsPort = 443;
const char* writeAPIKey = "TME46SAY6T8Z2UX3";

// Track number of cycles
unsigned long cycleCount = 0;

// Track last upload time for 15-second rate limit
unsigned long lastUploadTime = 0;

// WiFi client
WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, host, httpsPort);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);

  Serial.begin(9600);
  delay(1000);

  // Connect to WiFi
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

// ------------------ Send data to ThingSpeak ------------------
void sendToThingSpeak(unsigned long count) {
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

  // Detect button press (HIGH → LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    pressTime = millis();
    waitingToStart = true;
    outputOn = false;

    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  // After 5 seconds → turn ON LED + buzzer and increment cycle
  if (waitingToStart && (millis() - pressTime >= 5000)) {
    Serial.println("Button was Pressed! Light is Green, safe to cross");

    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000);

    outputStartTime = millis();
    outputOn = true;
    waitingToStart = false;

    // Increment cycle counter
    cycleCount++;

    // Send cycle count to ThingSpeak (rate-limited)
      if (millis() - lastUploadTime > 15000) { // 15-second ThingSpeak limit
      sendToThingSpeak(cycleCount);
      lastUploadTime = millis();
    }
  }

  // Turn off LED and buzzer after 8 seconds
  if (outputOn && (millis() - outputStartTime >= 8000)) {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    outputOn = false;
  }

  lastButtonState = currentButtonState;
}
