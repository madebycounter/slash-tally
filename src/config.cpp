#include "config.h"

#include "util.h"

Config::Config(String ns, Preferences *prefs) : ns(ns), prefs(prefs) {
}

void Config::load() {
    prefs->begin(ns.c_str(), false);
    wifi_ssid = prefs->getString("wifi_ssid", "");
    wifi_password = prefs->getString("wifi_password", "");
    camera = prefs->getString("camera", "in 1");
    brightness = prefs->getInt("brightness", 64);
    prefs->end();
}

void Config::save() {
    prefs->begin(ns.c_str(), false);
    prefs->putString("wifi_ssid", wifi_ssid);
    prefs->putString("wifi_password", wifi_password);
    prefs->putString("camera", camera);
    prefs->putInt("brightness", brightness);
    prefs->end();
}

void Config::reset() {
    prefs->begin(ns.c_str(), false);
    prefs->clear();
    prefs->end();
    load();
}

void Config::print() {
    Serial.println("Config state:");
    Serial.print("  wifi_ssid: ");
    Serial.println(wifi_ssid);
    Serial.print("  wifi_password: ");
    Serial.println(wifi_password);
    Serial.print("  camera: ");
    Serial.println(camera);
    Serial.print("  brightness: ");
    Serial.println(brightness);
}

void Config::handleRequest(AsyncWebServerRequest *request) {
    if (request->hasParam("wifi_ssid")) {
        wifi_ssid = url_decode(request->getParam("wifi_ssid")->value());
    }

    if (request->hasParam("wifi_password")) {
        wifi_password = url_decode(request->getParam("wifi_password")->value());
    }

    if (request->hasParam("camera")) {
        camera = url_decode(request->getParam("camera")->value());
    }

    if (request->hasParam("brightness")) {
        String brightness_str = url_decode(request->getParam("brightness")->value());
        int brightness_int = brightness_str.toInt();

        if (brightness_int >= 0 && brightness_int < 256) {
            brightness = brightness_int;
        }
    }

    save();
    print();

    request->send(200, "text/plain", "OK");
}

void Config::handleStateRequest(AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"wifi_ssid\":\"" + wifi_ssid + "\",";
    json += "\"wifi_password\":\"" + wifi_password + "\",";
    json += "\"camera\":\"" + camera + "\",";
    json += "\"brightness\":" + String(brightness);
    json += "}";
    request->send(200, "application/json", json);
}