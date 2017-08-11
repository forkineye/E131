/*
* Ethernet_Test.ino - Simple sketch to listen for E1.31 data on an 
*                     UNO Ethernet shield and print some statistics.
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
#include <SPI.h>
#include <Ethernet.h>
#include <E131.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0x2F, 0x1E, 0xE3 };

E131 e131;

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Configure via DHCP and listen Unicast on the default port */
    e131.begin(mac);
}

void loop() {
    /* Parse a packet */
    uint16_t num_channels = e131.parsePacket();
    
    /* Process channel data if we have it */
    if (num_channels) {
        Serial.print(F("Universe "));
        Serial.print(e131.universe);
        Serial.print(F(" / "));
        Serial.print(num_channels);
        Serial.print(F(" Channels | Packet#: "));
        Serial.print(e131.stats.num_packets);
        Serial.print(F(" / Errors: "));
        Serial.print(e131.stats.packet_errors);
        Serial.print(F(" / CH1: "));
        Serial.println(e131.data[0]);
    }
}
