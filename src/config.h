#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

class Config {
private:
    String ns;
    Preferences *prefs;

public:
    Config(String ns, Preferences *prefs);

    void load();
    void save();
    void reset();
    void handleRequest(AsyncWebServerRequest *request);
    void handleStateRequest(AsyncWebServerRequest *request);
    void print();

    String wifi_ssid;
    String wifi_password;
    String camera;
    int brightness;
};
