#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secrets.h"
#include <WiFi.h>
#include <Wire.h>

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT    32
#define OLED_RESET       -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = SSID;
const char* password = WIFI_PASSWORD;

const int httpsPort = 443;
const String url1 = "https://moneroblocks.info/api/get_stats";
const String url2 = "https://api.bitfinex.com/v1/pubticker/xmrusd";

WiFiClient client;
HTTPClient http;

void setup()
{
    Serial.begin(115200);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("Connecting to WiFi...");
    display.display();

    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    display.println("Connected to: ");
    display.print(ssid);
    display.display();
    delay(1500);
    display.clearDisplay();
    display.display();
}

void loop()
{
    request();
}

void request()
{
    double low, high, price;
    int height, hashrate, difficulty;

    display.clearDisplay();
    Serial.print("Connecting to ");
    Serial.println(url2);

    http.begin(url2);
    int httpCode2 = http.GET();
    StaticJsonDocument<2000> doc_url2;
    DeserializationError error_url2 = deserializeJson(doc_url2, http.getString());

    if (error_url2) {
        Serial.print(F("deserializeJson Failed"));
        Serial.println(error_url2.f_str());
        delay(2500);
        return;
    }

    low = doc_url2["low"];
    high = doc_url2["high"];
    price = doc_url2["ask"];    

    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode2);
    http.end();

    String monero[6];
    String names[6] = {"Price: ", "Low: ", "High: ", "Heigh: ", "Rate: ", "Diff: "};
    
    monero[0] = String(price) + "$";
    monero[1] = String(low) + "$";
    monero[2] = String(high) + "$";
    monero[3] = String(height);
    monero[4] = String(hashrate / pow(1000, 3)) + "Gh/s";
    monero[5] = String(difficulty / pow(1000, 3)) + "Gh/s";

    for (uint8_t i = 0; i < 3; i++) {
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.clearDisplay();
        center(names[i], 0, 5);
        center(monero[i], 0, 15);
        display.display();
        delay(5000);
    }
}

void center(const String buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); 
    display.setCursor((x - w / 2) + (128 / 2), y);
    display.print(buf);
}
