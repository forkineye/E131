/*
* ESP8266_Test.ino - Simple sketch to listen for E1.31 data on an ESP8266 
*                    and print some statistics.
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

#include <ESP8266WiFi.h>
#include <E131.h>

const char ssid[] = "........";         /* Replace with your SSID */
const char passphrase[] = "........";   /* Replace with your WPA2 passphrase */

E131 e131;

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Choose one to begin listening for E1.31 data */
    e131.begin(ssid, passphrase);               /* via Unicast on the default port */
    //e131.beginMulticast(ssid, passphrase, 1); /* via Multicast for Universe 1 */
}

void loop() {
    /* Parse a packet */
    uint16_t num_channels = e131.parsePacket();
    
    /* Process channel data if we have it */
    if (num_channels) {
        Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
                e131.universe,              // The Universe for this packet
                num_channels,               // Number of channels in this packet
                e131.stats.num_packets,     // Packet counter
                e131.stats.packet_errors,   // Packet error counter
                e131.data[0]);              // Dimmer data for Channel 1
    }
}
