#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFi.h>


const char* ssid = "wifi_name";
const char* password = "wifi_password";

const char* serverIP = "http://192.168.147.38/";

WiFiClient client;

const uint8_t GREEN_LED = 18;
const uint8_t YELLOW_LED = 19;
const uint8_t RED_LED = 21;

void setup() 
{
  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
}

void loop() 
{
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http; 
      WiFiClient client;  

      http.begin(client, serverIP);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        StaticJsonDocument<200> doc; 
        deserializeJson(doc, http.getString().c_str());

        digitalWrite(GREEN_LED, doc["green"]);
        digitalWrite(YELLOW_LED, doc["yellow"]);
        digitalWrite(RED_LED, doc["red"]);
      }

      http.end();
    }
}
