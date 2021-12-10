#include <WiFi.h>
#include "aWOT.h"
#include "StaticFiles.h"
#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_LEDS 100

#define DATA_PIN 22
CRGB leds[NUM_LEDS];

#define WIFI_SSID "Archer_c5"
#define WIFI_PASSWORD "kopo2008"
WiFiServer server(80);
Application app;
String ledStatus;
String ledFeedback;
String volstatus;
bool ledOn;
String request;
DynamicJsonDocument doc(512);
DynamicJsonDocument ledarray(5120);

void postHandler(Request &req, Response &res) {
    request=req.readString();
    deserializeJson(doc, request);
    int l = doc["led"];
    CRGB rgbstr(doc["r"],doc["g"],doc["b"]);
 

    ledarray["r"][l-1]=doc["r"];
    ledarray["g"][l-1]=doc["g"];
    ledarray["b"][l-1]=doc["b"];


    leds[l-1] = CRGB(rgbstr);
    DynamicJsonDocument ledfb(128);
    JsonArray r = ledfb.createNestedArray("r");
    JsonArray g = ledfb.createNestedArray("g");
    JsonArray b = ledfb.createNestedArray("b");
    ledFeedback="";
 
    r.add(leds[l-1].r);
    g.add(leds[l-1].g);
    b.add(leds[l-1].b);
 
    serializeJson(ledfb, ledFeedback);
    res.set("Content-Type", "application/json");
    res.print(ledFeedback);
// CRGB color = CRGB(leds[i].r,leds[i].g,leds[i].b);
    FastLED.show();
 }
  void getStatus(Request &req, Response &res) {
    volstatus="";
    serializeJson(ledarray, volstatus);
    res.set("Content-Type", "application/json");
    res.print(volstatus);
 //   Serial.println(volstatus);
// res.sendStatus(200);
 
 }


 void getLeds(Request &req, Response &res) {
 
  ledarray.clear();
  JsonArray r = ledarray.createNestedArray("r");
  JsonArray g = ledarray.createNestedArray("g");
  JsonArray b = ledarray.createNestedArray("b");
  ledStatus="";
  for (int i=0;i<NUM_LEDS;i++)  
    {
   
      r.add(leds[i].r);
      g.add(leds[i].g);
      b.add(leds[i].b);
    }
  
  res.set("Content-Type", "application/json");
  serializeJson(ledarray, ledStatus);
  res.print(ledStatus);
 
//res.sendStatus(200);

}
void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
 
 
 fill_solid( leds, NUM_LEDS, CRGB( 0, 0, 0) );
  FastLED.show();




  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
   app.post("/led", &postHandler);
   app.get("/init", &getLeds);
   app.post("/status", &getStatus);
  app.use(staticFiles());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
  }
}