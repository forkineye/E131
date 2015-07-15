E131 - E1.31 (sACN) library for Arduino
=======================================
This library is to simplify the validation and handling of E1.31 sACN (DMX over Ethernet) traffic.  It supports both Unicast and Multicast configurations and exposes the full E1.31 packet to the user.  Currently, development is targeted for the ESP8266 WiFi module and traditional Ethernet shields.  

### Supported Hardware
- ESP8266 - *in test*
- Arduino + Ethernet Shield - *in test* (unicast until Arduino 1.66 is released)

### API / Usage
#### Notes
- Double-buffering of packet data is disabled on AVR architectures due to memory constraints.  Make sure to check the return value of ```parsePacket()``` as your buffer may be trashed if a bad packet made its way in.
- Other architectures (i.e. ESP8266) double buffer the packets, so ```e131.data``` can always be considered pristine.

#### Initializers
These are to initialize the network stack and should be in the ```setup()``` function of your sketch.

##### Unicast WiFi Initializers
- ```int begin(const char *ssid)```: returns ```WiFi.status()```
- ```int begin(const char *ssid, uint16_t port)```: returns ```WiFi.status()```
- ```int begin(const char *ssid, const char *passphrase)```: returns ```WiFi.status()```
- ```int begin(const char *ssid, const char *passphrase, uint16_t port)```: returns ```WiFi.status()```

##### Multicast WiFi Initializers (ESP8266 Only)
- ```int beginMulticast(const char *ssid, uint16_t universe)```: returns ```WiFi.status()```
- ```int beginMulticast(const char *ssid, const char *passphrase, uint16_t universe)```: returns ```WiFi.status()```

##### Unicast Ethernet Initializers
- ```int begin(uint8_t *mac)```: DHCP initializer. Returns 0 if DHCP failed, 1 for success
- ```int begin(uint8_t *mac, uint16_t port)```: DHCP initializer. Returns 0 if DHCP failed, 1 for success
- ```void begin(uint8_t *mac, IPAddress ip, IPAddress subnet, IPAddress gateway, IPAddress dns)```: Static initializer
- ```void begin(uint8_t *mac, IPAddress ip, IPAddress subnet, IPAddress gateway, IPAddress dns, uint16_t port)```: Static initializer

#### Loop Handlers
These are non-blocking handlers to be used in the ```loop()``` function of your sketch.
- ```int parsePacket()```: Checks and parses new packets, returns number of DMX Channels in packet as ```uint16_t```

#### Exposed Data Structures
- ```byte *data```: Pointer to DMX channel data from packet.  Always valid if double-buffering is enabled (see notes above)
- ```uint16_t universe```: DMX Universe of last valid packet
- ```e131_packet_t *packet```: Pointer to last packet. Always valid if double-buffering is enabled (see notes above)
- ```e131_stats_t stats```: E1.31 Statistics


### Latest Development:
- GitHub: http://github.com/forkineye/E131
- Stuff: http://forkineye.com