E131 - E1.31 (sACN) library for Arduino
=======================================
This library is to simplify the validation and handling of E1.31 sACN (DMX over Ethernet) traffic.  It supports both Unicast and Multicast configurations and exposes the full E1.31 packet to the user.  Currently, development is targeted for the ESP8266 WiFi module and traditional Ethernet shields.  

### Supported Hardware
- ESP8266
- Arduino + Ethernet Shield

### API / Usage
#### Notes
- Double-buffering of packet data is disabled on AVR architectures due to memory constraints.  Make sure to check the return value of ```parsePacket()``` as your buffer may be trashed if a bad packet made its way in.
- Other architectures (i.e. ESP8266) double buffer the packets, so ```e131.data``` can always be considered pristine.  Double buffering can be disabled by defining ```NO_DOUBLE_BUFFER``` in ```E131.h```.
- WiFi connection attempts will timeout after 10 seconds if a successful connection has not been established.  ```WIFI_CONNECT_TIMEOUT``` can be changed in ```E131.h```.

#### Initializers
These are to initialize the network stack and should be in the ```setup()``` function of your sketch.

##### Generic UDP only Initializer
This initializer only sets up the UDP listener.  It is up to the user to setup their network connection.  The initializers below this one will setup the network connection for you.
- ```void begin(e131_listen_t type, uint16_t universe = 1)```: valid types are ```E131_UNICAST``` and ```E131_MULTICAST```.  ```universe``` is optional and only used for Multicast configuration.

##### Unicast WiFi Initializers
- ```int begin(const char *ssid, const char *passphrase)```: returns ```WiFi.status()```
- ```int begin(const char *ssid, const char *passphrase, IPAddress ip, IPAddress netmask, IPAddress gateway, IPAddress dns)```: returns ```WiFi.status()```

##### Multicast WiFi Initializers (ESP8266 Only)
- ```int beginMulticast(const char *ssid, const char *passphrase, uint16_t universe)```: returns ```WiFi.status()```
- ```int beginMulticast(const char *ssid, const char *passphrase, uint16_t universe, IPAddress ip, IPAddress netmask, IPAddress gateway, IPAddress dns)```: returns ```WiFi.status()```

##### Unicast Ethernet Initializers
- ```int begin(uint8_t *mac)```: DHCP initializer. Returns 0 if DHCP failed, 1 for success
- ```void begin(uint8_t *mac, IPAddress ip, IPAddress netmask, IPAddress gateway, IPAddress dns)```: Static initializer

#### Loop Handlers
These are non-blocking handlers to be used in the ```loop()``` function of your sketch.
- ```int parsePacket()```: Checks and parses new packets, returns number of DMX Channels in packet as ```uint16_t```

#### Exposed Data Structures
- ```byte *data```: Pointer to DMX channel data from packet.  Always valid if double-buffering is enabled (see notes above)
- ```uint16_t universe```: DMX Universe of last valid packet
- ```e131_packet_t *packet```: Pointer to last packet. Always valid if double-buffering is enabled (see notes above)
- ```e131_stats_t stats```: E1.31 Statistics

### Resources:
- Latest code: http://github.com/forkineye/E131
- Other Stuff: http://forkineye.com