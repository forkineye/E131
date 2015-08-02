/*
* ESP8266_NeoPixel.ino - Simple sketch to listen for E1.31 data on an ESP8266
*                        and drive WS2811 LEDs using the NeoPixel Library
*
* == Requires Adafruit_NeoPixel - http://github.com/adafruit/Adafruit_NeoPixel                        
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
#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 170  /* Number of pixels */
#define UNIVERSE 1      /* Universe to listen for */
#define CHANNEL_START 1 /* Channel to start listening at */
#define DATA_PIN 0      /* Pixel output - GPIO0 */

const char ssid[] = "........";         /* Replace with your SSID */
const char passphrase[] = "........";   /* Replace with your WPA2 passphrase */

E131 e131;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, DATA_PIN, NEO_RGB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Choose one to begin listening for E1.31 data */
    e131.begin(ssid, passphrase);                       /* via Unicast on the default port */
    //e131.beginMulticast(ssid, passphrase, UNIVERSE);  /* via Multicast for Universe 1 */

    /* Initialize output */
    pixels.begin();
    pixels.show();
}

void loop() {
    /* Parse a packet and update pixels */
    if(e131.parsePacket()) {
        if (e131.universe == UNIVERSE) {
            for (int i = 0; i < NUM_PIXELS; i++) {
                int j = i * 3 + (CHANNEL_START - 1);
                pixels.setPixelColor(i, e131.data[j], e131.data[j+1], e131.data[j+2]);
            }
            pixels.show();
        }
    }
}
