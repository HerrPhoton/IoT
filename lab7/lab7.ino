#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define BUFFER_SIZE 300

const char* ssid = "";
const char* password = "";

const char *mqtt_server = "";
const int mqtt_port = 1883;

WiFiClient wifi_client;
PubSubClient mqqt_client(wifi_client);

char buffer[BUFFER_SIZE];

const int DHT_PIN = 5;
DHT dht(DHT_PIN, DHT11);

void setup() 
{
  Serial.begin(9600);

  pinMode(DHT_PIN, INPUT);
  dht.begin();

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
  mqqt_client.setServer(mqtt_server, mqtt_port);

  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);

  while (!mqqt_client.connected())
  {
    mqqt_client.connect(clientId.c_str());
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Connected!");
}

void loop()
{
  mqqt_client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
      Serial.println("Failed to read from DHT sensor!");

  else
  {   
    String msg("{");
    
    msg += String("\"temperature\":") + String(temperature);
    msg += String(",\"humidity\":") + String(humidity);
    msg += String("}");

    snprintf(buffer, BUFFER_SIZE, "%s", msg.c_str());
    mqqt_client.publish("/dht", buffer);

    delay(1000);
  }

}
