#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define E131_WEB_PORT 80

typedef struct {
    String ssid;
    String password;
    byte  IP[4];
    byte  Netmask[4];
    byte  Gateway[4];
    boolean dhcp;
    String ntpServerName;
    long Update_Time_Via_NTP_Every;
    long timezone;
    boolean daylight;
    String DeviceName;
    boolean AutoTurnOff;
    boolean AutoTurnOn;
    byte TurnOffHour;
    byte TurnOffMinute;
    byte TurnOnHour;
    byte TurnOnMinute;
    byte LED_R;
    byte LED_G;
    byte LED_B;
} config_t;


class E131Web {
    private:
        static ESP8266WebServer server;
        static boolean firstStart;
        static int AdminTimeOutCounter;
        static boolean AdminEnabled;

        static config_t config;

        static void writeConfig();
        static boolean readConfig();
        static void configureWiFi();

        static void send_config_net_html();
        static void send_config_dmx_html();
        static void send_config_net_vals_html();
        static void send_config_dmx_vals_html();
        static void send_status_net_vals_html();
        static void send_status_dmx_vals_html();

        static void send_connection_state_vals_html();

    public:
        E131Web();
        void begin();
        void check();
};

#endif