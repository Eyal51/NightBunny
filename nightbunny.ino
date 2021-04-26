#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <FastLED.h>
#include <ESP8266WebServer.h>

FASTLED_USING_NAMESPACE

#ifndef STASSID
#define STASSID "wifiname"
#define STAPSK  "wifipassword"
#endif

#define DATA_PIN    D5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    16
#define FAST        50
#define SLOW        300

CRGB leds[NUM_LEDS];
ESP8266WebServer server(80);

int refresh = 20;
unsigned long runtime;
int ledtime = 0;
int hours = 0;
int minutes = 0;
int seconds;
int day;
int remain = 0;
int BRIGHTNESS = 60;
String output = "";
int pos = 0;

uint8_t gHue = 186; // rotating "base color" used by many of the patterns
bool dir = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3 * 3600, 60000);

const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  server.on("/", []() {
    server.send(200, "text/plain", output);
    });
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
    });

  bool led = 1;
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, led); // turn ir off
  Serial.begin(115200);
  output += "Starting";
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int counter = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, led);
    led = !led;
    counter++;
    delay(500);
    if (counter > 500) { ESP.restart(); }
  }
//  digitalWrite(LED_BUILTIN, LOW);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  server.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  
  // get NTP time
  if (hours == 0 and minutes == 0) { 
    output += "\nGetting NTP time...";
    getTime(); 
    delay(500);
    }
  
  uint8_t gHue = 230; // rotating "base color" used by many of the patterns
  bool dir = 0;
}

typedef void (*SimplePatternList[])();
SimplePatternList patterns = { rainbow, sleepLight, empty, circle };
uint8_t currentpattern = 0;

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  // keep track of time
  unsigned long delta = millis() - runtime;
  if (delta >= 1000) {
    seconds += delta/1000; 
    remain += delta % 1000;
    if (remain >= 1000) {
      remain -= 1000;
      seconds++;
    }
    if (seconds >= 60) {
      seconds -= 60;
      minutes++;
    }
    if (minutes >= 60) {
      minutes -= 60;
      hours++;
      output += "\nTime: " + String(hours) + ":" + String(minutes) + ":" + String(seconds);
      output += "\ncurrentpattern = " + String(currentpattern);
    }
    if (hours == 24) {
      hours == 0;
      // sanity check - get time every 24H
      getTime();
    }
    runtime = millis();
    // sanity check - reset every week
    if (runtime > 604800 * 1000) { ESP.restart(); }
  }
  // start led time conditions
  if ((hours >= 0) and (hours <= 5)) {  // breathe purple in the night
    BRIGHTNESS = 2;
    refresh = SLOW;
    currentpattern = 1;
  }
  else if ((hours == 6) and (minutes < 15)) {
    BRIGHTNESS = 2;
    refresh = SLOW;
    currentpattern = 1;
  }
  else if ((hours == 6) and (minutes > 14) and (minutes < 40)) {  // ok to wake with pride
    BRIGHTNESS = 50;
    refresh = FAST;
    currentpattern = 0;
  }
  else if ((hours == 12) or (hours == 13))  {  // breathe red in the noon
    BRIGHTNESS = 10;
    refresh = SLOW;
    currentpattern = 1;
  }
  else if ((hours == 14) and (minutes > 30) and (minutes < 45)) {  // wake with pride from noon nap
    BRIGHTNESS = 100;
    refresh = FAST;
    currentpattern = 0;
  }
  else if (hours >= 18) {  //  go to sleep with purple breath
    BRIGHTNESS = 2;
    refresh = SLOW;
    currentpattern = 1;
  }
  else {
    currentpattern = 2;  // clear all pixel data
  }

  // led update counter
  if (millis() - ledtime >= refresh) {
    patterns[currentpattern]();
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();   
    ledtime = millis();
  }
}

void getTime() {
  timeClient.update();
  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  day = timeClient.getDay() + 1;
  seconds = timeClient.getSeconds();
  output += "\n" + String(hours) + ":" + String(minutes) + ", Day:" + String(day);
  }

void sleepLight() {
  if (dir) { gHue++; }
  else { gHue--; }
  if ((gHue == 220) or (gHue == 240)) {
    dir = !dir;
    }
  if (gHue < 219) { gHue = 220; }
  if (gHue > 241) { gHue = 240; }
  fill_rainbow( leds, NUM_LEDS, gHue, 1);
  }

void rainbow() {
  gHue++;
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  }

void empty() {
  FastLED.clear();
  }

void circle() {
  for (int i = 0; i < NUM_LEDS; i++)
    leds[(i + pos) % NUM_LEDS] = CHSV(235, 255, i * (255 / NUM_LEDS));
  pos++;
  pos %= NUM_LEDS;
  
}
