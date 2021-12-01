/*
* E131.cpp
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

#include "E131.h"
#include <string.h>

/* E1.17 ACN Packet Identifier */
#ifdef ARDUINO_ARCH_AVR
const PROGMEM byte E131::ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };
#else
const byte E131::ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };
#endif

/* Constructor */
E131::E131() {
#ifdef NO_DOUBLE_BUFFER
    memset(pbuff1.raw, 0, sizeof(pbuff1.raw));
    packet = &pbuff1;
    pwbuff = packet;
    data = packet->property_values + 1;
#else
    memset(pbuff1.raw, 0, sizeof(pbuff1.raw));
    memset(pbuff2.raw, 0, sizeof(pbuff2.raw));
    packet = &pbuff1;
    pwbuff = &pbuff2;
#endif

    stats.num_packets = 0;
    stats.packet_errors = 0;
}

void E131::initUnicast() {
    delay(100);
    udp.begin(E131_DEFAULT_PORT);
    if (Serial) {
        Serial.print(F("- Unicast port: "));
        Serial.println(E131_DEFAULT_PORT);
    }
}
#ifndef INT_ETHERNET
void E131::initMulticast(uint16_t universe, uint8_t n) {
    delay(100);
    IPAddress address = IPAddress(239, 255, ((universe >> 8) & 0xff),
            ((universe >> 0) & 0xff));
#ifdef INT_ESP8266
    ip_addr_t ifaddr;
    ip_addr_t multicast_addr;

    ifaddr.addr = static_cast<uint32_t>(WiFi.localIP());
    for (uint8_t i = 1; i < n; i++) {
        multicast_addr.addr = static_cast<uint32_t>(IPAddress(239, 255,
                (((universe + i) >> 8) & 0xff), (((universe + i) >> 0)
                & 0xff)));
        igmp_joingroup(&ifaddr, &multicast_addr);
    }

    udp.beginMulticast(WiFi.localIP(), address, E131_DEFAULT_PORT);

#endif
    if (Serial) {
        Serial.println(F("- Multicast Enabled"));
    }
}
#endif

/****** START - Wireless ifdef block ******/
#if defined (INT_ESP8266) || defined (INT_WIFI)

/* WiFi Initialization */
int E131::initWiFi(const char *ssid, const char *passphrase) {
    /* Switch to station mode and disconnect just in case */
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    if (Serial) {
        Serial.println("");
        Serial.print(F("Connecting to "));
        Serial.print(ssid);
    }

    if (passphrase != NULL)
        WiFi.begin(ssid, passphrase);
    else
        WiFi.begin(ssid);

    uint32_t timeout = millis();
    uint8_t retval = 1;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (Serial)
            Serial.print(".");
        if (millis() - timeout > WIFI_CONNECT_TIMEOUT) {
            retval = 0;
            if (Serial) {
                Serial.println("");
                Serial.println(F("*** Failed to connect ***"));
            }
            break;
        }
    }

    return retval;
}

void E131::begin(e131_listen_t type, uint16_t universe, uint8_t n) {
    if (type == E131_UNICAST)
        initUnicast();
    if (type == E131_MULTICAST)
        initMulticast(universe, n);
}

int E131::begin(const char *ssid, const char *passphrase) {
    if (initWiFi(ssid, passphrase)) {
        if (Serial) {
            Serial.println("");
            Serial.print(F("Connected DHCP with IP: "));
            Serial.println(WiFi.localIP());
        }
        initUnicast();
    }
    return WiFi.status();
}

int E131::begin(const char *ssid, const char *passphrase,
        IPAddress ip, IPAddress netmask, IPAddress gateway, IPAddress dns) {
    if (initWiFi(ssid, passphrase)) {
        WiFi.config(ip, gateway, netmask, dns);
        if (Serial) {
            Serial.println("");
            Serial.print(F("Connected with Static IP: "));
            Serial.println(WiFi.localIP());
        }
        initUnicast();
    }
    return WiFi.status();
}

#endif
/****** END - Wireless ifdef block ******/

/****** START - ESP8266 ifdef block ******/
#if defined (INT_ESP8266)
int E131::beginMulticast(const char *ssid, const char *passphrase,
        uint16_t universe, uint8_t n) {
    if (initWiFi(ssid, passphrase)) {
        if (Serial) {
            Serial.println("");
            Serial.print(F("Connected DHCP with IP: "));
            Serial.println(WiFi.localIP());
        }
        initMulticast(universe, n);
    }
    return WiFi.status();
}

int E131::beginMulticast(const char *ssid, const char *passphrase,
        uint16_t universe, IPAddress ip, IPAddress netmask,
        IPAddress gateway, IPAddress dns, uint8_t n) {
    if (initWiFi(ssid, passphrase)) {
        WiFi.config(ip, dns, gateway, netmask);
        if (Serial) {
            Serial.println("");
            Serial.print(F("Connected with Static IP: "));
            Serial.println(WiFi.localIP());
        }
        initMulticast(universe, n);
    }
    return WiFi.status();
}
#endif
/****** END - ESP8266 ifdef block ******/

/****** START - Ethernet ifdef block ******/
#if defined (INT_ETHERNET)

/* Unicast Ethernet Initializers */
int E131::beginUnicast() {
    initUnicast();
}

int E131::beginMulticast(uint16_t universe, uint8_t n) {
    IPAddress address = IPAddress(239, 255, ((universe >> 8) & 0xff),
                ((universe >> 0) & 0xff));
    
    int retval = udp.beginMulticast(address, E131_DEFAULT_PORT);
    
    if (Serial) {
        if (retval) 
          Serial.println(F("- Multicast Enabled"));
        else
          Serial.println(F("- Failed to enable Multicast"));
    }
    return retval;
}

#endif
/****** END - Ethernet ifdef block ******/

void E131::dumpError(e131_error_t error) {
    switch (error) {
        case ERROR_ACN_ID:
            Serial.print(F("INVALID PACKET ID: "));
            for (int i = 0; i < sizeof(ACN_ID); i++)
                Serial.print(pwbuff->acn_id[i], HEX);
            Serial.println("");
            break;
        case ERROR_PACKET_SIZE:
            Serial.println(F("INVALID PACKET SIZE: "));
            break;
        case ERROR_VECTOR_ROOT:
            Serial.print(F("INVALID ROOT VECTOR: 0x"));
            Serial.println(htonl(pwbuff->root_vector), HEX);
            break;
        case ERROR_VECTOR_FRAME:
            Serial.print(F("INVALID FRAME VECTOR: 0x"));
            Serial.println(htonl(pwbuff->frame_vector), HEX);
            break;
        case ERROR_VECTOR_DMP:
            Serial.print(F("INVALID DMP VECTOR: 0x"));
            Serial.println(pwbuff->dmp_vector, HEX);
    }
}
