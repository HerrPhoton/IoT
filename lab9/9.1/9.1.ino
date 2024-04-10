#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


const char* ssid = "";
const char* password = "";


void setup() 
{
  Serial.begin(9600);
  Serial.println("Booting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() 
  {
    String type;

    if (ArduinoOTA.getCommand() == U_FLASH) 
      type = "sketch";

    else
      type = "filesystem";

    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() 
  {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) 
  {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);

    switch(error)
    {
      case OTA_AUTH_ERROR:
        Serial.println("Auth Failed");
        break;

      case OTA_BEGIN_ERROR:
        Serial.println("Begin Failed");
        break;

      case OTA_CONNECT_ERROR:
        Serial.println("Connect Failed");
        break;

      case OTA_RECEIVE_ERROR:
        Serial.println("Receive Failed");
        break;

      case OTA_END_ERROR:
        Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  ArduinoOTA.handle();
}
