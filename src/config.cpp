#include "config.h"

#include "util.h"

Config::Config(String ns, Preferences* prefs) : ns(ns), prefs(prefs) {
}

void Config::load() {
    prefs->begin(ns.c_str(), false);
    wifi_ssid = prefs->getString("wifi_ssid", "");
    wifi_password = prefs->getString("wifi_password", "");
    camera = prefs->getString("camera", "in 1");
    brightness = prefs->getInt("brightness", 64);
    id = prefs->getString("id", random_string(8));
    cam_1 = prefs->getString("cam_1", "");
    cam_2 = prefs->getString("cam_2", "");
    cam_3 = prefs->getString("cam_3", "");
    cam_4 = prefs->getString("cam_4", "");
    score = prefs->getInt("score", 0);
    prefs->end();
}

void Config::save() {
    prefs->begin(ns.c_str(), false);
    prefs->putString("wifi_ssid", wifi_ssid);
    prefs->putString("wifi_password", wifi_password);
    prefs->putString("camera", camera);
    prefs->putInt("brightness", brightness);
    prefs->putString("id", id);
    prefs->putString("cam_1", cam_1);
    prefs->putString("cam_2", cam_2);
    prefs->putString("cam_3", cam_3);
    prefs->putString("cam_4", cam_4);
    prefs->putInt("score", score);
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
    Serial.print("  id: ");
    Serial.println(id);
    Serial.print("  wifi_ssid: ");
    Serial.println(wifi_ssid);
    Serial.print("  wifi_password: ");
    Serial.println(wifi_password);
    Serial.print("  camera: ");
    Serial.println(camera);
    Serial.print("  brightness: ");
    Serial.println(brightness);
    Serial.print("  score: ");
    Serial.println(score);
}

void Config::handleRequest(AsyncWebServerRequest* request) {
    if (request->hasParam("id") && url_decode(request->getParam("id")->value()) != id) {
        request->send(200, "text/plain", "OK - Not for me");
        return;
    }

    if (request->hasParam("new_id")) {
        if (!request->hasParam("id")) {
            request->send(400, "text/plain", "ERROR - Missing ID");
            return;
        }

        id = url_decode(request->getParam("new_id")->value());
    }

    if (request->hasParam("wifi_ssid")) {
        wifi_ssid = url_decode(request->getParam("wifi_ssid")->value());
    }

    if (request->hasParam("wifi_password")) {
        wifi_password = url_decode(request->getParam("wifi_password")->value());
    }

    if (request->hasParam("camera")) {
        camera = url_decode(request->getParam("camera")->value());
    }

    if (request->hasParam("cam_1")) {
        cam_1 = url_decode(request->getParam("cam_1")->value());
    }

    if (request->hasParam("cam_2")) {
        cam_2 = url_decode(request->getParam("cam_2")->value());
    }

    if (request->hasParam("cam_3")) {
        cam_3 = url_decode(request->getParam("cam_3")->value());
    }

    if (request->hasParam("cam_4")) {
        cam_4 = url_decode(request->getParam("cam_4")->value());
    }

    if (request->hasParam("score")) {
        String score_str = url_decode(request->getParam("score")->value());
        score = score_str.toInt();
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

void Config::handleStateRequest(AsyncWebServerRequest* request) {
    String json = "{";
    json += "\"id\":\"" + id + "\",";
    json += "\"wifi_ssid\":\"" + wifi_ssid + "\",";
    json += "\"wifi_password\":\"" + wifi_password + "\",";
    json += "\"camera\":\"" + camera + "\",";
    json += "\"brightness\":" + String(brightness) + ",";
    json += "\"cam_1\":\"" + cam_1 + "\",";
    json += "\"cam_2\":\"" + cam_2 + "\",";
    json += "\"cam_3\":\"" + cam_3 + "\",";
    json += "\"cam_4\":\"" + cam_4 + "\",";
    json += "\"score\":" + String(score);
    json += "}";
    request->send(200, "application/json", json);
}
