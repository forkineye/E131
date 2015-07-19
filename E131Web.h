/*
* E131Web.h
*
* Project: E131 - E.131 (sACN) library for Arduino
* Copyright (c) 2015 Shelby Merrick
* http://www.forkineye.com
*
*  This program is provided free for you to use in any way that you wish,
*  subject to the laws and regulations where you are using it.  Due diligence
*  is strongly suggested before using this code.  Please give credit where due.
*
*  The Author makes no warranty of any kind, express or implied, with regard
*  to this program or the documentation contained in this document.  The
*  Author shall not be liable in any event for incidental or consequential
*  damages in connection with, or arising out of, the furnishing, performance
*  or use of these programs.
*
*/

#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "E131.h"

#define E131_WEB_PORT 80

/* Configuration structure */
typedef struct {
    uint16_t    universe;
    uint16_t    channel_start;
    uint16_t    channel_count;
    boolean     multicast;
    String      ssid;
    String      passphrase;
    byte        ip[4];
    byte        netmask[4];
    byte        gateway[4];
    boolean     dhcp;
} config_t;

class E131Web {
    private:
        static ESP8266WebServer server;
        static E131 *e131;
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

        uint16_t port;

    public:
        E131Web(E131 *e131, uint16_t port = E131_WEB_PORT);
        void begin();
        inline void handleClient() {
            server.handleClient();
        };
};

#endif