#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "";
const char* password = "";

String serverIP = "http://192.168.70.72/";

WiFiServer server(80);

const int BUTTON_PIN = 0;
const int LED_PIN = 2;

int lastState = HIGH;
int currentState;
bool is_active = true;


void setup() 
{
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Подключаемся к Wi-Fi
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  // Выводим локальный IP-адрес и запускаем сервер
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");
}

void loop() 
{
//*------------------Client---------------------*//
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == LOW && currentState == HIGH)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http; 
      WiFiClient client;  

      String serverPath = is_active ? serverIP + "?LED=OFF" : serverIP + "?LED=ON";
      is_active = !is_active;
      
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

//*------------------Server---------------------*//
  WiFiClient client = server.available();

  if (client.available())
  {
    String request = client.readStringUntil('\r');

    Serial.println(request);
    client.flush();

    if (request.indexOf("GET /?LED=ON") != -1)
      digitalWrite(LED_PIN, HIGH);

    if (request.indexOf("GET /?LED=OFF") != -1)
      digitalWrite(LED_PIN, LOW);
  }  
}
