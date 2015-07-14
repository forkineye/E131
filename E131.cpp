/*
* E131.h
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
#include <limits.h>
#include <string.h>

/* E1.17 ACN Packet Identifier */
const byte E131::ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };

E131::E131() {
    memset(pbuff1.raw, 0, sizeof(pbuff1.raw));
    memset(pbuff2.raw, 0, sizeof(pbuff2.raw));
    packet = &pbuff1;
    pwbuff = &pbuff2;
    sequence = 0;
    stats.num_packets = 0;
    stats.sequence_errors = 0;
}

void E131::initWiFi(const char *ssid, const char *passphrase) {
    /* Switch to station mode and disconnect just in case */
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    if (Serial) {
        Serial.println("");
        Serial.print("Connecting to ");
        Serial.print(ssid);
    }

    if (passphrase != NULL)
        WiFi.begin(ssid, passphrase);
    else
        WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (Serial)
            Serial.print(".");
    }

    if (Serial) {
        Serial.println("");
        Serial.print("Connected with IP: ");
        Serial.println(WiFi.localIP());
    }

}

void E131::initEthernet() {
//TODO: Ethernet support
}

void E131::initUnicast(uint16_t port) {
    delay(100);
    udp.begin(port);
    if (Serial) {
        Serial.print("- Unicast port: ");
        Serial.println(udp.localPort());
    }
}

void E131::initMulticast(uint16_t universe) {
    delay(100);
    IPAddress multicast = IPAddress(239, 255, ((universe >> 8) & 0xff), ((universe >> 0) & 0xff));
    udp.beginMulticast(WiFi.localIP(), multicast, E131_DEF_PORT);
    if (Serial) {
        Serial.print("- Universe: ");
        Serial.println(universe);
        Serial.print("- Multicast address: ");
        Serial.println(multicast);
    }
}

int E131::begin(const char *ssid) {
    return begin(ssid, NULL, E131_DEF_PORT);
}

int E131::begin(const char *ssid, uint16_t port) {
    return begin(ssid, NULL, port);
}

int E131::begin(const char *ssid, const char *passphrase) {
    return begin(ssid, passphrase, E131_DEF_PORT);
}

int E131::begin(const char *ssid, const char *passphrase, uint16_t port) {
    initWiFi(ssid, passphrase);
    initUnicast(port);
    return WiFi.status();
}
               
int E131::beginMulticast(const char *ssid, uint16_t universe) {
    beginMulticast(ssid, NULL, universe);
}

int E131::beginMulticast(const char *ssid, const char *passphrase, uint16_t universe) {
    initWiFi(ssid, passphrase);
    initMulticast(universe);
    return WiFi.status();
}


void E131::dumpError(e131_error_t error) {
    switch (error) {
		case ERROR_ACN_ID:
            Serial.print("INVALID PACKET ID: ");
            for (int i = 0; i < sizeof(ACN_ID); i++)
                Serial.print(pwbuff->acn_id[i], HEX);
            Serial.println("");
            break;
		case ERROR_PACKET_SIZE:
            Serial.println("INVALID PACKET SIZE: ");
            break;
		case ERROR_VECTOR_ROOT:
            Serial.print("INVALID ROOT VECTOR: 0x");
            Serial.println(htonl(pwbuff->root_vector), HEX);
            break;
		case ERROR_VECTOR_FRAME:
            Serial.print("INVALID FRAME VECTOR: 0x");
            Serial.println(htonl(pwbuff->frame_vector), HEX);
            break;
		case ERROR_VECTOR_DMP:
            Serial.print("INVALID DMP VECTOR: 0x");
            Serial.println(pwbuff->dmp_vector, HEX);
    }
}