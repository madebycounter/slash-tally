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
#include "util.h"

const unsigned long WIFI_JOIN_TIMEOUT = 10000;
String program = "";
String preview = "";
bool transitioning = false;
bool access_point = false;

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
        ui_spinner(CRGB::Blue);
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
    access_point = true;
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

    // Serial.println("State change:");
    // Serial.print("  program: ");
    // Serial.println(program);
    // Serial.print("  preview: ");
    // Serial.println(preview);
    // Serial.print("  transitioning: ");
    // Serial.println(transitioning);

    ui_heartbeat();

    request->send(200, "text/plain", "OK");
}

void signal_api_handler(AsyncWebServerRequest* request) {
    if (!request->hasParam("id")) {
        request->send(400, "text/plain", "ERROR - Missing ID");
        return;
    }

    if (url_decode(request->getParam("id")->value()) != config.id) {
        request->send(200, "text/plain", "OK - Not for me");
        return;
    }

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

    request->send(200, "text/plain", "OK");
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
}

void loop() {
    ui_set_brightness(config.brightness);
    ui_update(program, preview, config.camera, transitioning, access_point);

    btn.update();

    delay(1);
}
