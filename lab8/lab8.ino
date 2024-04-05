#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>


ESP8266WebServer server(80);

const uint8_t BUTTON_PIN = 0;
const uint8_t LED_PIN = 2;

int statusCode;
String st;
String content;

uint8_t led_state = LOW; 


bool testWifi()
{
  Serial.print("Waiting for Wifi to connect");
  
  for (int i = 0; i < 20; i++) 
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      return true;
    }
    
    delay(500);
    Serial.print(".");

    digitalWrite(LED_PIN, led_state);
    led_state = led_state == HIGH ? LOW : HIGH;
  }

  Serial.println("");
  Serial.println("Connect timed out, opening AP");

  return false;
}

void setupAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  int n = WiFi.scanNetworks();
  Serial.println("Scan done");

  if (!n)
    Serial.println("no networks found");

  else
  {
    for (int i = 0; i < n; i++)
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }

  Serial.println("");

  st = "<ol>";

  for (int i = 0; i < n; i++)
  {
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
 
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }

  st += "</ol>";

  delay(100);

  WiFi.softAP("Sanya_ESP8266", "123456789");
  launchWeb();
}

void createWebServer()
{
  server.on("/", []()
  {
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    
    content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
    content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
    content += ipStr;
    content += "<p>";
    content += st;
    content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
    content += "</html>";
    
    server.send(200, "text/html", content);
  });

  server.on("/scan", []() 
  {
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
 
    content = "<!DOCTYPE HTML>\r\n<html>go back";
    server.send(200, "text/html", content);
  });

  server.on("/setting", []() 
  {
    String qsid = server.arg("ssid");
    String qpass = server.arg("pass");

    if (qsid.length() > 0 && qpass.length() > 0) 
    {
      Serial.println("clearing eeprom");

      for (int i = 0; i < 96; i++)
        EEPROM.write(i, 0);
         
      for (int i = 0; i < qsid.length(); i++)
        EEPROM.write(i, qsid[i]);

      for (int i = 0; i < qpass.length(); i++)
        EEPROM.write(32 + i, qpass[i]);
        
      EEPROM.commit();
 
      content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
      statusCode = 200;
      
      Serial.println("");

      ESP.reset();
    } 
      
    else 
    {
      content = "{\"Error\":\"404 not found\"}";
      statusCode = 404;
    }

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(statusCode, "application/json", content);
  });
}

void launchWeb()
{
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());

  createWebServer();

  server.begin();
  Serial.println("Server started");
}

void setup_mode()
{  
  Serial.println("Turning on the access point");
  launchWeb();
  setupAP();

  Serial.println("");
  Serial.print("Waiting");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    delay(100);
    server.handleClient();

    digitalWrite(LED_PIN, led_state);
    led_state = led_state == HIGH ? LOW : HIGH;
  }
}

void setup() 
{
  Serial.begin(9600);
  EEPROM.begin(512);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, led_state);

  String esid;
  String epassword;

  for (int i = 0; i < 32; i++)
    esid += char(EEPROM.read(i));

  for (int i = 32; i < 96; ++i)
    epassword += char(EEPROM.read(i));

  if (esid.length() == 0)
  {
    setup_mode();
    return;
  }

  WiFi.begin(esid.c_str(), epassword.c_str());

  if (testWifi()) 
  {
    Serial.println("Succesfully Connected!!!");

    for (int i = 0; i < 10; i++)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(50);
      digitalWrite(LED_PIN, LOW);
      delay(50);
    }

    return;
  }

  setup_mode();
}

void loop() 
{
  if (!digitalRead(BUTTON_PIN))
    setup_mode();
}
