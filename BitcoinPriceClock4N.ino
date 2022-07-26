#include <WiFi.h>
#include <WiFiMulti.h>
#include <TFT_eSPI.h>
#include <WiFiUdp.h>
//#include <TimeLib.h>
#include <vector>
#include <Ethernet.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include"bitcoin.h"
#include"font.h"
#include"font2.h"
#include"font3.h"
WiFiMulti wifiMulti;
TFT_eSPI tft = TFT_eSPI();
void getprice();
String formattedDate;
String dayStamp;
String timeStamp;

//==============eSPI==========
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;
String ttime;
int timeZone = 7; //edit
int brightnes[5] = {40, 80, 120, 160, 200};
int b = 1;

String serverName = "https://api.coincap.io/v2/assets/bitcoin";
//String serverName = "https://api.coincap.io/v2/assets/ethereum";

float pricelog[5] = {0, 0, 0, 0, 0};
float priceAverage = 0;
int i = 0;
int inti = 0; //Check for first time running

unsigned long lastTime = 0;
String uptime;
unsigned long timerDelay = 5000;
StaticJsonDocument<200> doc;
const char* ntpServer = "pool.ntp.org";
//const long  gmtOffset_sec = 25200;   //Replace with your GMT offset (seconds)
//const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)

float priceStatus;
float priceUsd;
char buf[600];
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


void setup() {
  Serial.begin(115200);
  wifiMulti.addAP("IT_2G", "aeiou123!");
  wifiMulti.addAP("Ufifo2", "9aaaaaaaaa");
  wifiMulti.addAP("@home", "9aaaaaaaaa");
  wifiMulti.addAP("UfifoII", "9aaaaaaaaa");


  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi hah been connected to : ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    delay(100);
  }
  //String  uptime=String(printLocalTime());
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //===eSPI====
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, brightnes[b]);
  //===end.eSPI====
  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(25200);
}

void loop()
{
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(15000);
  }
  Serial.println("============ *** ============");
  datetime();

  getprice();
  delay(15000);



}

void getprice()
{
  //=========

  // String json1 = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  //char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  //  char json[65];
  // json1.toCharArray(json, 65);

  //=========
  HTTPClient http;

  String serverPath = serverName;
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
    payload.toCharArray(buf, 600);
    DeserializationError error = deserializeJson(doc, buf);

    Serial.print("Coin : ");
    Serial.println(doc["data"]["name"].as<const char*>());
    //    Serial.print("24 Hour Change : ");
    priceUsd = doc["data"]["priceUsd"] ;
    priceStatus = doc["data"]["changePercent24Hr"] ;
    if (inti == 0) {
      pricelog[0] = priceUsd;
      pricelog[1] = priceUsd;
      pricelog[2] = priceUsd;
      pricelog[3] = priceUsd;
      pricelog[4] = priceUsd;
      inti = 1;
    }
    pricelog[i] = priceUsd;
    priceAverage = (pricelog[0] + pricelog[1] + pricelog[2] + pricelog[3] + pricelog[4]) / 5;
    //   Serial.println(priceStatus);
    Serial.print("Price Log 0 : ");
    Serial.println(pricelog[0]);
    Serial.print("Price Log 1 : ");
    Serial.println(pricelog[1]);
    Serial.print("Price Log 2 : ");
    Serial.println(pricelog[2]);
    Serial.print("Price Log 3 : ");
    Serial.println(pricelog[3]);
    Serial.print("Price Log 4 : ");
    Serial.println(pricelog[4]);

    Serial.print("Last 5 time Price Average  : ");
    Serial.println(priceAverage);
    tft.fillScreen(TFT_BLACK);
    Serial.print("TimeStamp : ");
    Serial.println(timeStamp);
    tft.setTextFont(2);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(1);
    tft.setCursor(30, 120);
    tft.println(dayStamp);
    tft.setCursor(160, 120);
    tft.println(timeStamp);

    tft.setTextColor(TFT_ORANGE);
    tft.pushImage(5, 10, 72, 72, Bitcoin);
    tft.setFreeFont(&Rock_Salt_Regular_20);
    tft.drawString("Bitcoin", 0, 85);

    if (priceAverage > priceUsd) {
      tft.setFreeFont(&DSEG14_Classic_Bold_26);
      tft.setTextColor(TFT_RED);
      tft.drawString(String(priceUsd), 85, 15);
      Serial.print("Current Price   : ");
      Serial.println(priceUsd);
      Serial.print("Last 5 trend   : ");
      Serial.println("Red");

    } else {
      tft.setFreeFont(&DSEG14_Classic_Bold_26);
      tft.setTextColor(TFT_GREEN);
      tft.drawString(String(priceUsd), 85, 15);
      Serial.print("Current Price   : ");
      Serial.println(priceUsd);
      Serial.print("Last 5 trend   : ");
      Serial.println("GReen");
    }

    i++;
    if (i > 4) i = 0;

    if (priceStatus > 0) {
      tft.setTextColor(TFT_GREEN);
      tft.setFreeFont(&SansSerif_plain_18);
      tft.drawString(String(priceStatus) + " %", 145, 68);
      Serial.print("24H. Percentage Change : ");
      Serial.println( priceStatus);
      Serial.print("24H. Status :");
      Serial.println("Green");
    }
    if (priceStatus < 0) {
      tft.setTextColor(TFT_RED);
      tft.setFreeFont(&SansSerif_plain_18);
      tft.drawString(String(priceStatus) + " %", 145, 68);
      Serial.print("24H. Change : ");
      Serial.println( priceStatus);
      Serial.print("24H. Status :");
      Serial.println("Red");
    }

  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  

  http.end();
}


void  datetime(void) {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);


}
