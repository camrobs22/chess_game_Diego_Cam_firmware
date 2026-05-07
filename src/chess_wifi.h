#include <Arduino.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>


const char* WIFI_SSID = "Cameron's iPhone xs Max (2)";
const char* WIFI_PASS = "lmgtyt13";

const char* SERVER_URL = "http://10.104.226.11:3000/api/test";

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");

    // Make HTTP request
    HTTPClient http;

    http.begin(SERVER_URL);

    int httpCode = http.GET();

    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
        String payload = http.getString();

        Serial.println("Server response:");
        Serial.println(payload);
    }

    http.end();
}

void loop() {
}