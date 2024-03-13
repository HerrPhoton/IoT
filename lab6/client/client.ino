#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define BUFFER_SIZE 300

const char* ssid = "";
const char* password = "";

const char *mqtt_server = "192.168.147.231";
const int mqtt_port = 1883;

WiFiClient wifi_client;
PubSubClient mqqt_client(wifi_client);

char buffer[BUFFER_SIZE];

const uint8_t GREEN_LED = 16;
const uint8_t YELLOW_LED = 5;
const uint8_t RED_LED = 4;


void callback(char* topic, byte* payload, unsigned int length) 
{
  StaticJsonDocument<BUFFER_SIZE> doc; 
  deserializeJson(doc, payload);

  digitalWrite(GREEN_LED, doc["green"]);
  digitalWrite(YELLOW_LED, doc["yellow"]);
  digitalWrite(RED_LED, doc["red"]);
}

void setup() 
{
  Serial.begin(9600);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.println("");
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  Serial.print("Connecting to MQTT server");
  mqqt_client.setServer(mqtt_server, 1883);

  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);

  while (!mqqt_client.connected())
  {
    mqqt_client.connect(clientId.c_str());
    Serial.print(".");
    delay(500);
  }

  mqqt_client.setCallback(callback);
  mqqt_client.subscribe("/traffic_light");

  Serial.println("");
  Serial.println("Connected!");
}

void loop() 
{
  mqqt_client.loop();
}