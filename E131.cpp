/*
* E131.cpp
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

#include "E131.h"
#include <string.h>

/* E1.17 ACN Packet Identifier */
#ifdef ARDUINO_ARCH_AVR
const PROGMEM byte E131::ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };
#else
const byte E131::ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };
#endif

/* Constructor */
E131::E131() {
#ifdef NO_DOUBLE_BUFFER
    memset(pbuff1.raw, 0, sizeof(pbuff1.raw));
    packet = &pbuff1;
    pwbuff = packet;
#else
    memset(pbuff1.raw, 0, sizeof(pbuff1.raw));
    memset(pbuff2.raw, 0, sizeof(pbuff2.raw));
    packet = &pbuff1;
    pwbuff = &pbuff2;
    memset(pbuff3.raw, 0, sizeof(pbuff3.raw));
    memset(pbuff4.raw, 0, sizeof(pbuff4.raw));
    packetTX 	= &pbuff3;
    pwbuffTX 	= &pbuff4;
#endif

    sequence = 0;
    stats.num_packets = 0;
    stats.sequence_errors = 0;
    stats.packet_errors = 0;
}

uint16_t E131::swapf_uint16(uint16_t x) {
	// return ( ( (x) <<8) | (( (x)    >>8)&0xFF ) );
	return ( htons(x) );
}

uint32_t E131::swapf_uint32(uint32_t x) {
	return ( htonl(x) );
}

#define swap_uint16(x) ( ( (x) <<8) | (( (x)    >>8)&0xFF ) ) // surprisingly this does not work with printf
//					   ( ( 0x89<<8) | (( 0x1289 >>8)&0xFF ) )

void E131::initUnicast() {
    delay(100);
    udp.begin(E131_DEFAULT_PORT);
    if (Serial) {
        Serial.print(F("- Unicast port: "));
        Serial.println(E131_DEFAULT_PORT);
    }
}

void E131::initMulticast(uint16_t universe, uint8_t n) {
    delay(100);
    IPAddress address = IPAddress(239, 255, ((universe >> 8) & 0xff),
            ((universe >> 0) & 0xff));
#ifdef INT_ESP8266
    ip_addr_t ifaddr;
    ip_addr_t multicast_addr;

    ifaddr.addr = static_cast<uint32_t>(WiFi.localIP());
    for (uint8_t i = 1; i < n; i++) {
        multicast_addr.addr = static_cast<uint32_t>(IPAddress(239, 255,
                (((universe + i) >> 8) & 0xff), (((universe + i) >> 0)
                & 0xff)));
        igmp_joingroup(&ifaddr, &multicast_addr);
    }

    udp.beginMulticast(WiFi.localIP(), address, E131_DEFAULT_PORT);
#endif
    if (Serial) {
        Serial.println(F("- Multicast Enabled"));
    }
}

void E131::setRGB(const uint8_t channel,const uint8_t dRed,const uint8_t dGreen,const uint8_t dBlue) {
	// first channel on [1]; [0] has to be 0
	pwbuffTX->dmp.prop_val[channel+1] = dRed;
	pwbuffTX->dmp.prop_val[channel+2] = dGreen;
	pwbuffTX->dmp.prop_val[channel+3] = dBlue;
}

void E131::clearSendBuffer() {
	memset(&pwbuffTX->dmp.prop_val[0], 0, sizeof(pwbuffTX->dmp.prop_val));
}

void E131::fillSendBuffer(const uint8_t fillData) {
	memset(&pwbuffTX->dmp.prop_val[1],fillData,sizeof(pwbuffTX->dmp.prop_val)-1);
}

void E131::setSourceName(const char *source_name) {
	memcpy(pwbuffTX->frame.source_name, source_name, strlen(source_name) );
}
void E131::setSequenceNumber(const int  seq_number) {
	pwbuffTX->frame.seq_number = seq_number;
}
void E131::setData(const int channel, const int dmxVal ) {
	pwbuffTX->dmp.prop_val[channel] = dmxVal;
}

/* Initialize an E1.31 packet using a universe and a number of slots */
int E131::setPacketHeader(const uint16_t universe, const uint16_t num_channels) {
  if (pwbuffTX == NULL || universe < 1 || universe > 63999 || num_channels < 1 || num_channels > 512) {
    // errno = EINVAL;
    return -1;
  }

  // compute packet layer lengths
  uint16_t prop_val_cnt = num_channels + 1;
  uint16_t dmp_length = prop_val_cnt +
    sizeof pwbuffTX->dmp - sizeof pwbuffTX->dmp.prop_val;
  uint16_t frame_length = sizeof pwbuffTX->frame + dmp_length;
  uint16_t root_length = sizeof pwbuffTX->root.flength +
    sizeof pwbuffTX->root.vector + sizeof pwbuffTX->root.cid + frame_length;

  // clear packet
  memset(packet, 0, sizeof *packet);

  // set Root Layer values
  pwbuffTX->root.preamble_size 	= swapf_uint16(_E131_PREAMBLE_SIZE);
  pwbuffTX->root.postamble_size = swapf_uint16(_E131_POSTAMBLE_SIZE);
  memcpy(pwbuffTX->root.acn_pid, _E131_ACN_PID, sizeof pwbuffTX->root.acn_pid);
  pwbuffTX->root.flength 		= swapf_uint16(0x7000 | root_length);
  pwbuffTX->root.vector 		= swapf_uint32(_E131_ROOT_VECTOR);

  // set Framing Layer values
  pwbuffTX->frame.flength 		= swapf_uint16(0x7000 | frame_length);
  pwbuffTX->frame.vector 		= swapf_uint32(_E131_FRAME_VECTOR);
  pwbuffTX->frame.priority 		= E131_DEFAULT_PRIORITY;
  pwbuffTX->frame.options 		= _E131_FRAME_OPTIONS;
  pwbuffTX->frame.universe 		= swapf_uint16(universe);

  // set Device Management Protocol (DMP) Layer values
  pwbuffTX->dmp.flength 		= swapf_uint16(0x7000 | dmp_length);
  pwbuffTX->dmp.vector 			= _E131_DMP_VECTOR;
  pwbuffTX->dmp.type 			= _E131_DMP_TYPE;
  pwbuffTX->dmp.first_addr 		= swapf_uint16(_E131_DMP_FIRST_ADDR);
  pwbuffTX->dmp.addr_inc 		= swapf_uint16(_E131_DMP_ADDR_INC);
  pwbuffTX->dmp.prop_val_cnt 	= swapf_uint16(prop_val_cnt);

  return 0;
}


void E131::dumpPacket(int packetNo) {

	e131_packet_t *dmpbuff;

	if (packetNo==0) {
		dmpbuff = packet;
		Serial.print("\n --------------------------------- packet \n");
	}
	else {
		dmpbuff = pwbuff;
		Serial.print("\n --------------------------------- pwbuff \n");
	}


	  Serial.print("E1.31 Root Layer\n");
	  printf("  Preamble Size .......... %04x\n"	, swapf_uint16(dmpbuff->preamble_size) ); 	// SWAP // uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int
	  printf("  Post-amble Size ........ %04x\n"	, swapf_uint16(dmpbuff->postamble_size) );	// uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int
	  printf("  ACN Packet Identifier .. %s\n"		, dmpbuff->acn_id);				// uint8_t  = 1 bytes = [0-255] or [0x00-0xFF]		 = unsigned char
	  printf("  Flags & Length ......... %04x\n"	, swapf_uint16(dmpbuff->root_flength) );		// uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int
	  printf("  Layer Vector ........... %08x\n"	, swapf_uint32(dmpbuff->root_vector) );		// uint32_t = 4 bytes = unsigned long
	  printf("  Component Identifier ... ");
	  for (size_t pos=0, total=sizeof dmpbuff->cid; pos<total; pos++)
	    printf("%02x", dmpbuff->cid[pos]);
	  printf("\n");

	  Serial.print("E1.31 Framing Layer\n");
	  printf("  Flags & Length ......... %04x\n"	, swapf_uint16(dmpbuff->frame_flength));		// uint16_t
	  printf("  Layer Vector ........... %08x\n"	, swapf_uint32(dmpbuff->frame_vector));		// uint32_t
	  printf("  Source Name ............ %s\n"			, dmpbuff->source_name);		// uint8_t
	  printf("  Packet Priority ........ %02x = %u\n"	, dmpbuff->priority,dmpbuff->priority); // ok // uint8_t
	  printf("  Reserved ............... %04x\n"	, swapf_uint16(dmpbuff->reserved));			// uint16_t
	  printf("  Sequence Number ........ %02x\n"	, dmpbuff->sequence_number);		// ok // uint8_t
	  printf("  Options Flags .......... %02x\n"	, dmpbuff->options);				// uint8_t
	  printf("  DMX Universe Number .... %04x = %u\n"	, swapf_uint16(dmpbuff->universe),swapf_uint16(dmpbuff->universe));// SWAP // uint16_t

	  Serial.print("E1.31 Device Management Protocol (DMP) Layer\n");
	  printf("  Flags & Length ......... %04x\n"	, swapf_uint16(dmpbuff->dmp_flength));			// uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int
	  printf("  Layer Vector ........... %02x\n"	, dmpbuff->dmp_vector);				// uint8_t  = 1 bytes = [0-255] or [0x00-0xFF]		 = unsigned char
	  printf("  Address & Data Type .... %02x\n"	, dmpbuff->type);					// uint8_t  = 1 bytes = [0-255] or [0x00-0xFF]		 = unsigned char
	  printf("  First Address .......... %04x\n"	, swapf_uint16(dmpbuff->first_address));			// uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int
	  printf("  Address Increment ...... %04x\n"	, swapf_uint16(dmpbuff->address_increment));		// uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int
	  printf("  Property Value Count ... %04x\n"	, swapf_uint16(dmpbuff->property_value_count));	// uint16_t = 2 bytes = [0-65535] or [0x0000-0xFFFF] = unsigned int

	  Serial.print("E1.31 DMP Property Values\n ");
	  for (size_t pos=0, total=(dmpbuff->property_value_count); pos<total; pos++)
	    printf(" %02x", dmpbuff->property_values[pos]);

	  Serial.print("\n");

}

void E131::stopUdp(void) {

#ifdef INT_ESP8266
    udp.stop();
#endif

}

void E131::connectMulticast(uint16_t universe) {
	int retval=0;
    delay(100);
    IPAddress address = IPAddress(239, 255, ((universe >> 8) & 0xff),
            ((universe >> 0) & 0xff));
#ifdef INT_ESP8266
    retval = udp.beginMulticast(WiFi.localIP(), address, E131_DEFAULT_PORT);
#endif
    if (Serial) {
        Serial.print(F("udp> Universe: "));
        Serial.print(universe);
        Serial.print(F(" / Multicast address: "));
        Serial.print(address);
        if (retval > 0)
            Serial.print(F(" udp ok\n"));
        else
            Serial.print(F(" ERROR: udp no sockets available\n"));
    }
}

/****** START - Wireless ifdef block ******/
#if defined (INT_ESP8266) || defined (INT_WIFI)

/* WiFi Initialization */
int E131::initWiFi(const char *ssid, const char *passphrase) {
    /* Switch to station mode and disconnect just in case */
    // WiFi.mode(WIFI_STA); // removed so we can operate in STA_AP
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

void E131::begin(e131_listen_t type, uint16_t universe, uint8_t n) {
    if (type == E131_UNICAST)
        initUnicast();
    if (type == E131_MULTICAST)
        initMulticast(universe, n);
}

int E131::begin(const char *ssid, const char *passphrase) {
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

int E131::begin(const char *ssid, const char *passphrase,
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

#endif
/****** END - Wireless ifdef block ******/

/****** START - ESP8266 ifdef block ******/
#if defined (INT_ESP8266)
int E131::beginMulticast(const char *ssid, const char *passphrase,
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

int E131::beginMulticast(const char *ssid, const char *passphrase,
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
#endif
/****** END - ESP8266 ifdef block ******/

/****** START - Ethernet ifdef block ******/
#if defined (INT_ETHERNET)

/* Unicast Ethernet Initializers */
int E131::begin(uint8_t *mac) {
    int retval = 0;

    if (Serial) {
        Serial.println("");
        Serial.println(F("Requesting Address via DHCP"));
        Serial.print(F("- MAC: "));
        for (int i = 0; i < 6; i++)
            Serial.print(mac[i], HEX);
        Serial.println("");
    }

    retval = Ethernet.begin(mac);

    if (Serial) {
        if (retval) {
            Serial.print(F("- IP Address: "));
            Serial.println(Ethernet.localIP());
        } else {
            Serial.println(F("** DHCP FAILED"));
        }
    }

    if (retval)
        initUnicast();

    return retval;
}

void E131::begin(uint8_t *mac, IPAddress ip, IPAddress netmask,
        IPAddress gateway, IPAddress dns) {
    Ethernet.begin(mac, ip, dns, gateway, netmask);
    if (Serial) {
        Serial.println("");
        Serial.println(F("Static Configuration"));
        Serial.println(F("- MAC: "));
        for (int i = 0; i < 6; i++)
            Serial.print(mac[i], HEX);
        Serial.print(F("- IP Address: "));
        Serial.println(Ethernet.localIP());
    }

    initUnicast();
}

/* Multicast Ethernet Initializers */
int E131::beginMulticast(uint8_t *mac, uint16_t universe, uint8_t n) {
    //TODO: Add ethernet multicast support
}

void E131::beginMulticast(uint8_t *mac, uint16_t universe,
        IPAddress ip, IPAddress netmask, IPAddress gateway,
        IPAddress dns, uint8_t n) {
    //TODO: Add ethernet multicast support
}
#endif
/****** END - Ethernet ifdef block ******/

void E131::dumpError(e131_error_t error) {
    switch (error) {
        case ERROR_ACN_ID:
            Serial.print(F("INVALID PACKET ID: "));
            for (int i = 0; i < sizeof(ACN_ID); i++)
                Serial.print(pwbuff->acn_id[i], HEX);
            Serial.println("");
            break;
        case ERROR_PACKET_SIZE:
            Serial.println(F("INVALID PACKET SIZE: "));
            break;
        case ERROR_VECTOR_ROOT:
            Serial.print(F("INVALID ROOT VECTOR: 0x"));
            Serial.println(htonl(pwbuff->root_vector), HEX);
            break;
        case ERROR_VECTOR_FRAME:
            Serial.print(F("INVALID FRAME VECTOR: 0x"));
            Serial.println(htonl(pwbuff->frame_vector), HEX);
            break;
        case ERROR_VECTOR_DMP:
            Serial.print(F("INVALID DMP VECTOR: 0x"));
            Serial.println(pwbuff->dmp_vector, HEX);
    }
}
