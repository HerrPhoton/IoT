#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <Arduino_JSON.h>
#include <DHT.h>

const char* ssid = "wifi_name";
const char* password = "wifi_password";

AsyncWebServer server(80);

const int DHT_PIN = 5;

DHT dht(DHT_PIN, DHT11);

float temperature = 0.0;
float humidity = 0.0;

unsigned long previousMillis = 0; 
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function() 
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() 
  {
    if (this.readyState == 4 && this.status == 200) 
    {
      document.getElementById("temperature").innerHTML = JSON.parse(this.responseText).temperature;
      document.getElementById("humidity").innerHTML = JSON.parse(this.responseText).humidity;
    }
  };
  xhttp.open("GET", "/data", true);
  xhttp.send();
}, 10000);

</script>
</html>)rawliteral";

String processor(const String& var)
{
  if (var == "TEMPERATURE")
    return String(temperature);
  
  else if (var == "HUMIDITY")
    return String(humidity);
  
  return String();
}

void setup() 
{
  Serial.begin(9600);
  dht.begin();

  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String msg("{\"temperature\":\"");
  
    msg += String(temperature);
    msg += String("\",\"humidity\":\"");
    msg += String(humidity);
    msg += String("\"}");

    request->send_P(200, "text/json", msg.c_str());
  });

  server.begin();
  Serial.println("Server started");  
}

void loop() 
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;

    float newT = dht.readTemperature();

    if (isnan(newT))
      Serial.println("Failed to read from DHT sensor!");

    else 
    {
      temperature = newT;
      Serial.println(temperature);
    }

    float newH = dht.readHumidity();

    if (isnan(newH))
      Serial.println("Failed to read from DHT sensor!");

    else 
    {
      humidity = newH;
      Serial.println(humidity);
    }
  }
}
