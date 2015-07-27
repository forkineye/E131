/*
* Ethernet_FastLED.ino - Simple sketch to listen for E1.31 data on an 
*                        UNO Ethernet shield and drive WS2811 LEDs.
*                        
* == Requires FastLED - http://github.com/FastLED/FastLED
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
#include <FastLED.h>

#define NUM_PIXELS 170
#define DATA_PIN 3

byte mac[] = { 0xDE, 0xAD, 0xBE, 0x2F, 0x1E, 0xE3 };

E131 e131;

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Configure via DHCP and listen Unicast on the default port */
    e131.begin(mac);

    FastLED.addLeds<WS2811, DATA_PIN>((CRGB*)e131.data, NUM_PIXELS);
}

void loop() {
    /* Parse a packet and update pixels */
    if(e131.parsePacket())
        FastLED.show();
}
