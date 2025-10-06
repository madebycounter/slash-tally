#include "netmgr.h"

void NetworkManager::init() {
    status = NETMGR_STATUS_DISCONNECTED;

    WiFi.mode(WIFI_STA);

    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
            status = NETMGR_STATUS_CONNECTED;

            if (on_connected_callback) {
                on_connected_callback();
            }
        }

        if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            status = NETMGR_STATUS_DISCONNECTED;

            if (on_disconnected_callback) {
                on_disconnected_callback();
            }
        }

        if (event == ARDUINO_EVENT_WIFI_SCAN_DONE) {
            scan_complete();
        }
    });

    load_networks();
}

bool NetworkManager::add_network(const char *ssid, const char *password) {
    if (network_exists(ssid) || networks_count >= NETMGR_MAX_NETWORKS) {
        return false;
    }

    networks[networks_count].ssid = String(ssid);
    networks[networks_count].password = String(password);
    networks_count++;

    save_networks();

    return true;
}

bool NetworkManager::remove_network(const char *ssid) {
    for (int i = 0; i < networks_count; i++) {
        if (networks[i].ssid == String(ssid)) {
            for (int j = i; j < networks_count - 1; j++) {
                networks[j] = networks[j + 1];
            }

            networks_count--;
            save_networks();
            return true;
        }
    }

    return false;
}

bool NetworkManager::connect(const char *ssid) {
    Network *network = get_network(ssid);

    if (network) {
        status = NETMGR_STATUS_CONNECTING;

        if (on_connecting_callback) {
            on_connecting_callback(network->ssid.c_str());
        }

        WiFi.begin(network->ssid.c_str(), network->password.c_str());
        return true;
    }

    return false;
}

void NetworkManager::disconnect() {
    status = NETMGR_STATUS_DISCONNECTED;
    WiFi.disconnect(true);
}

void NetworkManager::scan_and_connect() {
    status = NETMGR_STATUS_SCANNING;
    WiFi.scanDelete();
    WiFi.scanNetworks(true);

    if (on_scanning_callback) {
        on_scanning_callback();
    }
}

NetworkManagerStatus NetworkManager::get_status() {
    return status;
}

String NetworkManager::get_ssid() {
    return WiFi.SSID();
}

IPAddress NetworkManager::get_ip() {
    return WiFi.localIP();
}

IPAddress NetworkManager::get_gateway() {
    return WiFi.gatewayIP();
}

IPAddress NetworkManager::get_subnet() {
    return WiFi.subnetMask();
}

Network *NetworkManager::get_network(const char *ssid) {
    for (int i = 0; i < networks_count; i++) {
        if (networks[i].ssid == String(ssid)) {
            return &networks[i];
        }
    }

    return nullptr;
}

bool NetworkManager::network_exists(const char *ssid) {
    for (int i = 0; i < networks_count; i++) {
        if (networks[i].ssid == String(ssid)) {
            return true;
        }
    }

    return false;
}

void NetworkManager::on_scanning(void (*callback)()) {
    on_scanning_callback = callback;
}

void NetworkManager::on_connecting(void (*callback)(const char *ssid)) {
    on_connecting_callback = callback;
}

void NetworkManager::on_connected(void (*callback)()) {
    on_connected_callback = callback;
}

void NetworkManager::on_disconnected(void (*callback)()) {
    on_disconnected_callback = callback;
}

void NetworkManager::reset() {
    networks_count = 0;
    save_networks();
}

void NetworkManager::load_networks() {
    preferences.begin(NETMGR_PREFERENCES_NAMESPACE, true);
    networks_count = preferences.getInt("networks_count", 0);

    if (networks_count > NETMGR_MAX_NETWORKS) {
        networks_count = NETMGR_MAX_NETWORKS;
    }

    for (int i = 0; i < networks_count; i++) {
        String ssid_key = "ssid_" + String(i);
        String pass_key = "pass_" + String(i);
        networks[i].ssid = preferences.getString(ssid_key.c_str(), "");
        networks[i].password = preferences.getString(pass_key.c_str(), "");
    }

    preferences.end();
}

void NetworkManager::save_networks() {
    preferences.begin(NETMGR_PREFERENCES_NAMESPACE, false);
    preferences.putInt("networks_count", networks_count);

    for (int i = 0; i < networks_count; i++) {
        String ssid_key = "ssid_" + String(i);
        String pass_key = "pass_" + String(i);
        preferences.putString(ssid_key.c_str(), networks[i].ssid);
        preferences.putString(pass_key.c_str(), networks[i].password);
    }

    preferences.end();
}

void NetworkManager::scan_complete() {
    int n = WiFi.scanComplete();

    if (n == WIFI_SCAN_RUNNING || n < 0) {
        return;
    }

    int best_network_index = -1;
    int best_rssi = -1000;

    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);

        for (int j = 0; j < networks_count; j++) {
            if (networks[j].ssid == ssid) {
                if (rssi > best_rssi) {
                    best_rssi = rssi;
                    best_network_index = j;
                }

                break;
            }
        }
    }

    if (best_network_index != -1) {
        connect(networks[best_network_index].ssid.c_str());
    } else {
        status = NETMGR_STATUS_DISCONNECTED;

        if (on_disconnected_callback) {
            on_disconnected_callback();
        }
    }
}
