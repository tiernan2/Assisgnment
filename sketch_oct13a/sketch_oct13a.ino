#define BLYNK_TEMPLATE_ID "TMPL48f4hWvWj"
#define BLYNK_TEMPLATE_NAME "IoT Assignment"
#define BLYNK_AUTH_TOKEN "CyZEF_aH0S3EpTUus6sfRaTSyrhGQIvL"

#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <rgb_lcd.h>
#include <BlynkSimpleWifi.h>

// ------------------ LCD ------------------
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 128;
const int colorB = 255;

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

// ------------------ Blynk ------------------
char blynkAuth[] = BLYNK_AUTH_TOKEN;

// ------------------ Blynk Remote Button (V2) ------------------
BLYNK_WRITE(V2) {
  int state = param.asInt();
  if (state == 1) {
    pressTime = millis();
    waitingToStart = true;
    outputOn = false;

    Serial.println("Remote Blynk Triggered");
    lcd.clear();
    lcd.print("Remote Trigger");
    Blynk.virtualWrite(V1, "WAITING");
  }
}

// ------------------ Send data to ThingSpeak ------------------
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
