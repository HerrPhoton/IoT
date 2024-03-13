#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Sanya";
const char* password = "012345678";

const char *mqtt_server = "mqtt.by";
const int mqtt_port = 1883;
const char *mqtt_user = "HerrPhoton";
const char *mqtt_password = "d77imues";

WiFiClient wifi_client;
PubSubClient mqqt_client(wifi_client);

char msg[2];

const uint8_t BUTTON_PIN = 0;

const uint8_t GREEN_LED = 16;
const uint8_t YELLOW_LED = 5;
const uint8_t RED_LED = 4;

struct TrafficLight
{
  uint8_t g;
  uint8_t y;
  uint8_t r;
};

struct State
{
  std::vector<int> ids;
  std::vector<int> delays;
};

TrafficLight traffic_light = {LOW, LOW, LOW};

State regulated = {
  {1, 0, 1, 0, 1, 0, 1, 2, 3, 4, 3, 4, 3, 4, 5}, 
  {6000, 1000, 1000, 1000, 1000, 1000, 1000, 3000, 6000, 1000, 1000, 1000, 1000, 1000, 1000, 3000}
  };

State unregulated = {
  {6, 7}, 
  {1000, 1000}
  };

State* state_ptr = &regulated;
int cur_i = 0;

std::tuple<TrafficLight, TrafficLight> id2state(int id)
{
  switch(id)
  {
    case 0:
      return {TrafficLight{LOW, LOW, LOW}, TrafficLight{LOW, LOW, HIGH}};

    case 1:
      return {TrafficLight{HIGH, LOW, LOW}, TrafficLight{LOW, LOW, HIGH}};

    case 2:
      return {TrafficLight{LOW, HIGH, LOW}, TrafficLight{LOW, HIGH, HIGH}};

    case 3:
      return {TrafficLight{LOW, LOW, HIGH}, TrafficLight{HIGH, LOW, LOW}};

    case 4:
      return {TrafficLight{LOW, LOW, HIGH}, TrafficLight{LOW, LOW, LOW}};
    
    case 5:
      return {TrafficLight{LOW, HIGH, HIGH}, TrafficLight{LOW, HIGH, LOW}};

    case 6:
      return {TrafficLight{LOW, HIGH, LOW}, TrafficLight{LOW, HIGH, LOW}};

    case 7:
      return {TrafficLight{LOW, LOW, LOW}, TrafficLight{LOW, LOW, LOW}};
  }

  return {TrafficLight{LOW, LOW, LOW}, TrafficLight{LOW, LOW, LOW}};
}

void change_state(int id)
{
  std::tuple<TrafficLight, TrafficLight> state = id2state(id);

  digitalWrite(GREEN_LED, std::get<0>(state).g);
  digitalWrite(YELLOW_LED, std::get<0>(state).y);
  digitalWrite(RED_LED, std::get<0>(state).r);

  traffic_light.g = std::get<1>(state).g;
  traffic_light.y = std::get<1>(state).y;
  traffic_light.r = std::get<1>(state).r;
}

void ICACHE_RAM_ATTR isr()
{
  state_ptr = state_ptr == &regulated ? &unregulated : &regulated;
  cur_i = 0;

  Serial.print("Intersection mode: ");

  if (state_ptr == &regulated)
    Serial.println("regulated");
  else
    Serial.println("unregulated");
}

void setup() 
{
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN, isr, RISING);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

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

  Serial.println("Connecting to MQTT server");
  mqqt_client.setServer(mqtt_server, 1883);

  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);

  while (!mqqt_client.connected())
  {
    mqqt_client.connect(clientId.c_str(), mqtt_user, mqtt_password);
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Connected!");
}

void loop() 
{
  mqqt_client.loop();
  
  int id = cur_i++ % (state_ptr->ids).size();
  change_state(state_ptr->ids[id]);
  delay(state_ptr->delays[id]);

  msg[0] = (char)traffic_light.g;
  mqqt_client.publish("/user/HerrPhoton/green", msg);

  msg[0] = (char)traffic_light.y;
  mqqt_client.publish("/user/HerrPhoton/yellow", msg);

  msg[0] = (char)traffic_light.r;
  mqqt_client.publish("/user/HerrPhoton/red", msg);
}
