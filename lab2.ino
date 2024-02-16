#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "wifi name";
const char* password = "wifi password";

String serverName = "https://192.168.1.33";

const int BUTTON_PIN = 0;
const int LED_PIN = 2;

int lastState = HIGH;
int currentState;

void setup() 
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() 
{
  currentState = digitalRead(BUTTON_PIN);

  if (lastState != currentState)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      WiFiClient client;
      HTTPClient http;    
    
      String serverPath = currentState == HIGH ? serverName + "?LED=ON" : serverName + "?LED=OFF";

      http.begin(client, serverPath.c_str());
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }

      else 
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
  }

  lastState = currentState;
}
