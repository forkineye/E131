/*
* E131Async.h
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

#ifndef E131ASYNC_H_
#define E131ASYNC_H_

#include <ESPAsyncUDP.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <lwip/ip_addr.h>
#include <lwip/igmp.h>
#include <Arduino.h>
#include "E131common.h"
#include "RingBuf.h"

/* Status structure */
typedef struct {
    uint32_t    num_packets;
    uint32_t    sequence_errors;
    uint32_t    packet_errors;
    IPAddress   last_clientIP;
    uint16_t    last_clientPort;
} e131_stats_t;

class E131Async {
 private:
    /* Constants for packet validation */
    static const uint8_t ACN_ID[];
    static const uint32_t VECTOR_ROOT = 4;
    static const uint32_t VECTOR_FRAME = 2;
    static const uint8_t VECTOR_DMP = 2;

    e131_packet_t   *sbuff;     /* Pointer to scratch packet buffer */
    uint8_t         sequence;   /* Sequence tracker */
    AsyncUDP        udp;        /* AsyncUDP */

    /* Internal Initializers */
    int initWiFi(const char *ssid, const char *passphrase);
    void initUnicast();
    void initMulticast(uint16_t universe, uint8_t n = 1);

 public:
    e131_stats_t  stats;    /* Statistics tracker */
    RingBuf       *pbuff;   /* Ring Buffer of universe packet buffers */

    E131Async(uint8_t buffers = 1);

    /* Generic UDP listener, no physical or IP configuration */
    void begin(e131_listen_t type, uint16_t universe = 1, uint8_t n = 1);

    /* Unicast WiFi Initializers */
    int begin(const char *ssid, const char *passphrase);
    int begin(const char *ssid, const char *passphrase,
            IPAddress ip, IPAddress netmask, IPAddress gateway, IPAddress dns);

    /* Multicast WiFi Initializers */
    int beginMulticast(const char *ssid, const char *passphrase,
            uint16_t universe, uint8_t n = 1);
    int beginMulticast(const char *ssid, const char *passphrase,
            uint16_t universe, IPAddress ip, IPAddress netmask,
            IPAddress gateway, IPAddress dns, uint8_t n = 1);

    /* Packet parser callback */
    void parsePacket(AsyncUDPPacket _packet);

    /* Diag functions */
    void dumpError(e131_error_t error);
};


#endif /* E131ASYNC_H_ */
