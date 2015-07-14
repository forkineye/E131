E131 - E1.31 (sACN) library for Arduino
=======================================
This library is to simplify the validation and handling of E1.31 sACN (DMX over Ethernet) traffic.  It supports both Unicast and Multicast configurations and exposes the full E1.31 packet to the user.  Currently, development is targeted for the ESP8266 WiFi module.  

### Supported Hardware
- ESP8266 - *in test*
- Arduino + Ethernet Shield - *planned* (unicast until Arduino 1.66 is released)
- Arduino + WiFi Shield - *planned* (unicast only)

### API / Usage
#### Initializers
These are to initialize the network stack and should be in the '''setup()''' function of your sketch.

##### Unicast WiFi Initializers
'''int begin(const char *ssid)''': returns '''WiFi.status()'''
'''int begin(const char *ssid, uint16_t port)returns '''WiFi.status()'''
'''int begin(const char *ssid, const char *passphrase)''': returns '''WiFi.status()'''
'''int begin(const char *ssid, const char *passphrase, uint16_t port)''': returns '''WiFi.status()'''

##### Multicast WiFi Initializers (ESP8266 Only)
'''int beginMulticast(const char *ssid, uint16_t universe)''': returns '''WiFi.status()'''
'''int beginMulticast(const char *ssid, const char *passphrase, uint16_t universe)''': returns '''WiFi.status()'''

#### Loop Handlers
These are non-blocking handlers to be used in the '''loop()''' function of your sketch.
''' int parsePacket()''': Checks and parses new packets, returns number of DMX Channels in packet as '''uint16_t'''

#### Exposed Data Structures
'''byte *data''': Pointer to DMX channel data from last valid packet
'''uint16_t universe''': DMX Universe of last valid packet
'''e131_packet_t *packet''': Pointer to last valid E1.31 packet
'''e131_stats_t stats''': E1.31 Statistics

### Latest Development:
GitHub: http://github.com/forkineye/e131
Stuff: http://forkineye.com

**This is currently a work in progress and incomplete**