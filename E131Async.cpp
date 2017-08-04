/*
* E131Async.cpp
*
* Project: E131 - E.131 (sACN) library for Arduino
* Copyright (c) 2017 Shelby Merrick
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

#include "E131Async.h"
#include <string.h>

/* E1.17 ACN Packet Identifier */
const byte E131Async::ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };

/* Constructor */
E131Async::E131Async(uint8_t buffers) {
    pbuff = RingBuf_new(sizeof(e131_packet_t), buffers);

    sequence = 0;
    stats.num_packets = 0;
    stats.sequence_errors = 0;
    stats.packet_errors = 0;
}

void E131Async::initUnicast() {
    delay(100);

    if (udp.listen(E131_DEFAULT_PORT)) {
        if (Serial) {
            Serial.print(F("- Unicast port: "));
            Serial.println(E131_DEFAULT_PORT);
        }

        udp.onPacket(std::bind(&E131Async::parsePacket, this,
                std::placeholders::_1));
    }
}

void E131Async::initMulticast(uint16_t universe, uint8_t n) {
    delay(100);

    IPAddress address = IPAddress(239, 255, ((universe >> 8) & 0xff),
        ((universe >> 0) & 0xff));

    if (udp.listenMulticast(address, E131_DEFAULT_PORT)) {
        ip_addr_t ifaddr;
        ip_addr_t multicast_addr;

        ifaddr.addr = static_cast<uint32_t>(WiFi.localIP());
        for (uint8_t i = 1; i < n; i++) {
            multicast_addr.addr = static_cast<uint32_t>(IPAddress(239, 255,
                    (((universe + i) >> 8) & 0xff), (((universe + i) >> 0)
                    & 0xff)));
            igmp_joingroup(&ifaddr, &multicast_addr);
        }

        if (Serial) {
            Serial.print(F("- Universe: "));
            Serial.println(universe);
            Serial.println(F("- Multicast Enabled"));
        }

        udp.onPacket(std::bind(&E131Async::parsePacket, this,
                std::placeholders::_1));
    }
}

/* WiFi Initialization */
int E131Async::initWiFi(const char *ssid, const char *passphrase) {
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

void E131Async::begin(e131_listen_t type, uint16_t universe, uint8_t n) {
    if (type == E131_UNICAST)
        initUnicast();
    if (type == E131_MULTICAST)
        initMulticast(universe, n);
}

int E131Async::begin(const char *ssid, const char *passphrase) {
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

int E131Async::begin(const char *ssid, const char *passphrase,
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

int E131Async::beginMulticast(const char *ssid, const char *passphrase,
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

int E131Async::beginMulticast(const char *ssid, const char *passphrase,
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

/* Main packet parser */
void E131Async::parsePacket(AsyncUDPPacket _packet) {
    e131_error_t error = ERROR_NONE;

    sbuff = reinterpret_cast<e131_packet_t *>(_packet.data());
    if (memcmp(sbuff->acn_id, E131Async::ACN_ID, sizeof(sbuff->acn_id)))
        error = ERROR_ACN_ID;
    if (htonl(sbuff->root_vector) != E131Async::VECTOR_ROOT)
        error = ERROR_VECTOR_ROOT;
    if (htonl(sbuff->frame_vector) != E131Async::VECTOR_FRAME)
        error = ERROR_VECTOR_FRAME;
    if (sbuff->dmp_vector != E131Async::VECTOR_DMP)
        error = ERROR_VECTOR_DMP;
    if (sbuff->property_values[0] != 0)
        error = ERROR_IGNORE;


    if (!error) {
        pbuff->add(pbuff, sbuff);
        if (sbuff->sequence_number != sequence++) {
            stats.sequence_errors++;
            sequence = sbuff->sequence_number + 1;
        }
        stats.num_packets++;
        stats.last_clientIP = _packet.remoteIP();
        stats.last_clientPort = _packet.remotePort();
    } else if (error == ERROR_IGNORE) {
        // Do nothing
    } else {
        if (Serial)
            dumpError(error);
        stats.packet_errors++;
    }
}

void E131Async::dumpError(e131_error_t error) {
    switch (error) {
        case ERROR_ACN_ID:
            Serial.print(F("INVALID PACKET ID: "));
            for (int i = 0; i < sizeof(ACN_ID); i++)
                Serial.print(sbuff->acn_id[i], HEX);
            Serial.println("");
            break;
        case ERROR_PACKET_SIZE:
            Serial.println(F("INVALID PACKET SIZE: "));
            break;
        case ERROR_VECTOR_ROOT:
            Serial.print(F("INVALID ROOT VECTOR: 0x"));
            Serial.println(htonl(sbuff->root_vector), HEX);
            break;
        case ERROR_VECTOR_FRAME:
            Serial.print(F("INVALID FRAME VECTOR: 0x"));
            Serial.println(htonl(sbuff->frame_vector), HEX);
            break;
        case ERROR_VECTOR_DMP:
            Serial.print(F("INVALID DMP VECTOR: 0x"));
            Serial.println(sbuff->dmp_vector, HEX);
    }
}
