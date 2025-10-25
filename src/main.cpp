#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "button.h"
#include "config.h"
#include "ui.h"

const unsigned long WIFI_JOIN_TIMEOUT = 10000;
String program = "";
String preview = "";
bool transitioning = false;
int bitrate = 0;
float framerate = 0;
bool streaming = false;
bool hold = false;

Preferences preferences;
Config config("tally", &preferences);
AsyncWebServer server(80);
Button btn(39, INPUT_PULLUP, true);
WiFiClient client;

String get_mac_address() {
    uint8_t m[6];
    esp_efuse_mac_get_default(m);

    char mac_str[13];
    sprintf(mac_str, "%02X%02X%02X%02X%02X%02X", m[0], m[1], m[2], m[3], m[4], m[5]);

    return String(mac_str);
}

bool wifi_connect() {
    if (config.wifi_ssid == "") {
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifi_ssid, config.wifi_password);

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_JOIN_TIMEOUT) {
        delay(50);
    }

    return WiFi.status() == WL_CONNECTED;
}

void wifi_access_point() {
    String ssid = "TallyLight_" + get_mac_address();
    IPAddress local_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid);
}

void state_api_handler(AsyncWebServerRequest* request) {
    if (request->hasParam("program")) {
        program = request->getParam("program")->value();
    }

    if (request->hasParam("preview")) {
        preview = request->getParam("preview")->value();
    }

    if (request->hasParam("transitioning")) {
        transitioning = request->getParam("transitioning")->value() == "true";
    }

    if (request->hasParam("bitrate")) {
        bitrate = request->getParam("bitrate")->value().toInt();
    }

    if (request->hasParam("framerate")) {
        framerate = request->getParam("framerate")->value().toFloat();
    }

    if (request->hasParam("streaming")) {
        streaming = request->getParam("streaming")->value() == "Live";
    }

    Serial.println("State change:");
    Serial.print("  program: ");
    Serial.println(program);
    Serial.print("  preview: ");
    Serial.println(preview);
    Serial.print("  transitioning: ");
    Serial.println(transitioning);

    ui_heartbeat();

    request->send(200, "text/plain", "OK");
}

void signal_api_handler(AsyncWebServerRequest* request) {
    SignalType type = UI_SIGNAL_SOLID;

    if (request->hasParam("type")) {
        String type_name = request->getParam("type")->value();

        if (type_name == "arrow_up") {
            type = UI_SIGNAL_ARROW_UP;
        } else if (type_name == "arrow_down") {
            type = UI_SIGNAL_ARROW_DOWN;
        } else if (type_name == "arrow_left") {
            type = UI_SIGNAL_ARROW_LEFT;
        } else if (type_name == "arrow_right") {
            type = UI_SIGNAL_ARROW_RIGHT;
        } else if (type_name == "repeat") {
            type = UI_SIGNAL_REPEAT;
        } else if (type_name == "negative") {
            type = UI_SIGNAL_NEGATIVE;
        } else if (type_name == "positive") {
            type = UI_SIGNAL_POSITIVE;
        } else {
            type = UI_SIGNAL_SOLID;
        }
    }

    ui_heartbeat();
    ui_signal(type);
}

void begin_hold() {
    hold = true;

    String url = "http://companion.lan/api/location/3/0/0/press";
    HTTPClient http;

    http.begin(client, url.c_str());
    int code = http.POST("");
    String resp = http.getString();
    http.end();
}

void end_hold() {
    hold = false;

    String url = "http://companion.lan/api/location/3/0/1/press";
    HTTPClient http;

    http.begin(client, url.c_str());
    int code = http.POST("");
    String resp = http.getString();
    http.end();
}

void webserver_init() {
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/api/config", [](AsyncWebServerRequest* request) {
        config.handleRequest(request);
    });

    server.on("/api/config_state", [](AsyncWebServerRequest* request) {
        config.handleStateRequest(request);
    });

    server.on("/api/state", state_api_handler);
    server.on("/api/signal", signal_api_handler);

    server.on("/api/restart", [](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", "OK");

        request->onDisconnect([]() {
            delay(1000);
            ESP.restart();
        });
    });

    server.begin();
}

void setup() {
    Serial.begin(9600);

    Serial.println("Loading config...");
    config.load();
    config.print();

    Serial.println("Initializing UI...");
    ui_init();
    ui_set_brightness(config.brightness);

    btn.onPress([]() {
        if (hold) {
            end_hold();
        } else {
            begin_hold();
        }
    });

    Serial.println("Initializing SPIFFS...");

    if (!SPIFFS.begin()) {
        Serial.println("Failed to initialize SPIFFS");
    }

    Serial.println("Connecting to WiFi...");
    if (!wifi_connect()) {
        Serial.println("Could not connect, starting access point");
        wifi_access_point();
    } else {
        Serial.print("Successfully connected to ");
        Serial.println(config.wifi_ssid);
        Serial.print("Local IP is ");
        Serial.println(WiFi.localIP());
    }

    Serial.println("Starting web server...");
    webserver_init();

    end_hold();
}

void loop() {
    ui_set_brightness(config.brightness);
    ui_update(program, preview, config.camera, transitioning, bitrate, framerate, streaming, hold);

    btn.update();

    delay(1);
}
