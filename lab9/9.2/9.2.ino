#include <Arduino.h>
 
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
 
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
 
#define FIRMWARE_VERSION "0.1"


const char* ssid = "";
const char* password = "";

ESP8266WiFiMulti WiFiMulti;
 

void update_started() 
{
  Serial.println("CALLBACK:  HTTP update process started");
}
 
void update_finished() 
{
  Serial.println("CALLBACK:  HTTP update process finished");
}
 
void update_progress(int cur, int total) 
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}
 
void update_error(int err) 
{
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
 
void setup() {
 
  Serial.begin(9600);
 
  Serial.println();
  Serial.println();
  Serial.println();
 
  for (uint8_t t = 4; t > 0; t--) 
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
 
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
 
  Serial.print(F("Firmware version "));
  Serial.println(FIRMWARE_VERSION);
  delay(2000);
}
 
 
void loop()
{
  if ((WiFiMulti.run() == WL_CONNECTED)) 
  {
    WiFiClient client;
 
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
 
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
 
    ESPhttpUpdate.rebootOnUpdate(false);
 
    Serial.println(F("Update start now!"));
 
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "192.168.43.231", 3000, "/firmware/9.2.ino.bin");
 
    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        Serial.println(F("Retry in 10secs!"));
        delay(10000);
        break;
 
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
 
      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        delay(1000);
        ESP.restart();
        break;
    }
  }
}