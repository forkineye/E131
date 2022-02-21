#include <ESP8266WiFi.h>
#include <E131.h>

#include "wifi.h"
// Create file with the following
// *************************************************************************
// #define SECRET_SSID "";  /* Replace with your SSID */
// #define SECRET_PSK "";   /* Replace with your WPA2 passphrase */
// *************************************************************************

const char ssid[] = SECRET_SSID;         /* Replace with your SSID */
const char passphrase[] = SECRET_PSK;   /* Replace with your WPA2 passphrase */

E131 e131;

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Choose one to begin listening for E1.31 data */
    // e131.begin(ssid, passphrase);               /* via Unicast on the default port */
    e131.beginMulticast(ssid, passphrase, 1); /* via Multicast for Universe 1 */

//    e131.begin(E131_MULTICAST);
}

void loop() {
  
    e131.setData(2,random(0,255));
    e131.sendPacket(1);
    delay(10);
}
