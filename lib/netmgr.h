#pragma once

#define NETMGR_MAX_NETWORKS 20
#define NETMGR_PREFERENCES_NAMESPACE "netmgr"

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

struct Network {
    String ssid;
    String password;
};

enum NetworkManagerStatus {
    NETMGR_STATUS_DISCONNECTED,
    NETMGR_STATUS_SCANNING,
    NETMGR_STATUS_CONNECTING,
    NETMGR_STATUS_CONNECTED,
};

class NetworkManager {
public:
    void init();
    bool add_network(const char *ssid, const char *password);
    bool remove_network(const char *ssid);

    bool connect(const char *ssid);
    void disconnect();
    void scan_and_connect();

    NetworkManagerStatus get_status();
    String get_ssid();
    IPAddress get_ip();
    IPAddress get_gateway();
    IPAddress get_subnet();

    Network *get_network(const char *ssid);
    bool network_exists(const char *ssid);

    void on_scanning(void (*callback)());
    void on_connecting(void (*callback)(const char *ssid));
    void on_connected(void (*callback)());
    void on_disconnected(void (*callback)());

    void reset();

private:
    void (*on_scanning_callback)() = nullptr;
    void (*on_connecting_callback)(const char *ssid) = nullptr;
    void (*on_connected_callback)() = nullptr;
    void (*on_disconnected_callback)() = nullptr;

    Preferences preferences;
    NetworkManagerStatus status;
    Network networks[NETMGR_MAX_NETWORKS];
    int networks_count = 0;

    void load_networks();
    void save_networks();
    void scan_complete();
};