#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

int pin_in = A0;
int pin_en = 16;
int pin_d1 = 5;
int pin_d2 = 4;
int pin_d4 = 0;
int pin_d8 = 2;

const int numberOfPorts = 8;

float temperatures[16];

std::unique_ptr<ESP8266WebServer> server;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  Serial.println("Initializing");
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  Serial.println("Connected. Starting web server.");

  server.reset(new ESP8266WebServer(WiFi.localIP(), 80));

  server->on("/", [](){
    String output = "[";
    for(int i=1;i<=numberOfPorts;i++) {
      if(i>1) output += ",";
      output += "{\"sensor\":"+String(i)+",\"temperature\":"+String(temperatures[i])+"}";
    }
    output += "]";
    server->send(200, "application/json", output);
  });

  server->begin();
  
  pinMode(pin_en, OUTPUT);
  pinMode(pin_d1, OUTPUT);
  pinMode(pin_d2, OUTPUT);
  pinMode(pin_d4, OUTPUT);
  pinMode(pin_d8, OUTPUT);

  
  Serial.println("All ready.");
}

int i = 0, zero = 0;

void loop() {
  setPort(i);
  delay(500);
  int temp = analogRead(pin_in);
  //Serial.println(temp);
  if(i == 0) {
    zero = temp;
  } else {
    temp -= zero;
    temperatures[i] = ((float)temp) * 100 / 1024;
    Serial.print(i);
    Serial.print(" -> ");
    Serial.println( temperatures[i] );
  }
  delay(500);
  i++;
  if(i > numberOfPorts) {
    i = 0;
  }

  server->handleClient();
}

void setPort(byte i) {
  digitalWrite(pin_d1, i & 0b0001);
  digitalWrite(pin_d2, i & 0b0010);
  digitalWrite(pin_d4, i & 0b0100);
  digitalWrite(pin_d8, i & 0b1000);
}

