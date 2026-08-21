#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

class Config {
private:
    String ns;
    Preferences* prefs;

public:
    Config(String ns, Preferences* prefs);

    void load();
    void save();
    void reset();
    void handleRequest(AsyncWebServerRequest* request);
    void handleStateRequest(AsyncWebServerRequest* request);
    void print();

    String id;
    String wifi_ssid;
    String wifi_password;
    int camera;
    int score;
    int brightness;

    String cam_1;
    String cam_2;
    String cam_3;
    String cam_4;
    String cam_5;
    String cam_6;
    String cam_7;
    String cam_8;
};
