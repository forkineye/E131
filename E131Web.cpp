#if defined (ARDUINO_ARCH_ESP8266) // Needed for Arduino's auto-magic build system
/* 
* E131Web.cpp
*
* Project: E131 - E.131 (sACN) library for Arduino
* Copyright (c) 2015 Shelby Merrick
* http://www.forkineye.com
*
* Derived from ESP_WebConfig 
*
*  Copyright (c) 2015 John Lassen. All rights reserved.
*  This is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*  This software is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "E131Web.h"
#include "helpers.h"

/*
Include the HTML, STYLE and Script "Pages"
*/
#include "pages/microajax.js.h"
#include "pages/style.css.h"

#include "pages/root.h"
#include "pages/config_net.h"
#include "pages/config_dmx.h"
#include "pages/status_net.h"
#include "pages/status_dmx.h"

/* Static Defines  */
ESP8266WebServer E131Web::server;
config_t E131Web::config;
E131 *(E131Web::e131);

E131Web::E131Web(E131 *e131, uint16_t port) {
    server = ESP8266WebServer(port);
    this->e131 = e131;
    this->port = port;
}

void E131Web::begin() {
//    EEPROM.begin(512);
//  if (!ReadConfig()) {
        // DEFAULT CONFIG
        config.ssid = "myssid";
        config.passphrase = "mypassphrase";
        config.dhcp = true;
        config.ip[0] = 192; config.ip[1] = 168; config.ip[2] = 1; config.ip[3] = 100;
        config.netmask[0] = 255; config.netmask[1] = 255; config.netmask[2] = 255; config.netmask[3] = 0;
        config.gateway[0] = 192; config.gateway[1] = 168; config.gateway[2] = 1; config.gateway[3] = 1;
        config.universe = 0;
        config.channel_start = 1;
        config.channel_count = 512;

//      WriteConfig();
//        Serial.println("General config applied");
//  }
    
    /*
    if (AdminEnabled)
    {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
    }
    else
    {
        WiFi.mode(WIFI_STA);
    }

    ConfigureWifi();
    */

    /* JavaScript and Stylesheets */
    server.on ("/style.css", []() { server.send(200, "text/plain", PAGE_STYLE_CSS); });
    server.on ("/microajax.js", []() { server.send(200, "text/plain", PAGE_MICROAJAX_JS); });

    /* HTML Pages */
    server.on("/", []() { server.send(200, "text/html", PAGE_ROOT); });
    server.on("/config/net.html", send_config_net_html);
    server.on("/config/dmx.html", send_config_dmx_html);
    server.on("/status/net.html", []()  { server.send(200, "text/html", PAGE_STATUS_NET); });
    server.on("/status/dmx.html", []()  { server.send(200, "text/html", PAGE_STATUS_DMX); });

    /* AJAX Handlers */
    server.on("/config/netvals", send_config_net_vals_html);
    server.on("/config/dmxvals", send_config_dmx_vals_html);
    server.on("/config/connectionstate", send_connection_state_vals_html);
    server.on("/status/netvals", send_status_net_vals_html);
    server.on("/status/dmxvals", send_status_dmx_vals_html);

    server.onNotFound([]() { server.send(404, "text/html", "Page not Found"); });
    server.begin();

    if (Serial) {
        Serial.print(F("- Web Server started on port "));
        Serial.println(port);
    }
}

/* 
void E131Web::check() {
    if (AdminEnabled)
    {
        if (AdminTimeOutCounter > AdminTimeOut)
        {
             AdminEnabled = false;
             Serial.println("Admin Mode disabled!");
             WiFi.mode(WIFI_STA);
        }
    }
  
    server.handleClient();
}
*/

void E131Web::configureWiFi()
{
 /*
    Serial.println("Configuring Wifi");
    WiFi.begin (config.ssid.c_str(), config.password.c_str());
    if (!config.dhcp)
    {
        WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
    }
*/
}

void E131Web::writeConfig()
{
/*
    Serial.println("Writing Config");
    EEPROM.write(0,'C');
    EEPROM.write(1,'F');
    EEPROM.write(2,'G');

    EEPROM.write(16,config.dhcp);
    EEPROM.write(17,config.daylight);
    
    EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte

    EEPROMWritelong(22,config.timezone);  // 4 Byte


    EEPROM.write(26,config.LED_R);
    EEPROM.write(27,config.LED_G);
    EEPROM.write(28,config.LED_B);

    EEPROM.write(32,config.IP[0]);
    EEPROM.write(33,config.IP[1]);
    EEPROM.write(34,config.IP[2]);
    EEPROM.write(35,config.IP[3]);

    EEPROM.write(36,config.Netmask[0]);
    EEPROM.write(37,config.Netmask[1]);
    EEPROM.write(38,config.Netmask[2]);
    EEPROM.write(39,config.Netmask[3]);

    EEPROM.write(40,config.Gateway[0]);
    EEPROM.write(41,config.Gateway[1]);
    EEPROM.write(42,config.Gateway[2]);
    EEPROM.write(43,config.Gateway[3]);


    WriteStringToEEPROM(64,config.ssid);
    WriteStringToEEPROM(96,config.password);
    WriteStringToEEPROM(128,config.ntpServerName);

    EEPROM.write(300,config.AutoTurnOn);
    EEPROM.write(301,config.AutoTurnOff);
    EEPROM.write(302,config.TurnOnHour);
    EEPROM.write(303,config.TurnOnMinute);
    EEPROM.write(304,config.TurnOffHour);
    EEPROM.write(305,config.TurnOffMinute);
    WriteStringToEEPROM(306,config.DeviceName);
   
    EEPROM.commit();
*/
}

boolean E131Web::readConfig() {
    Serial.println("Reading Configuration");
    if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' ) {
        Serial.println("Configurarion Found!");
        config.dhcp =   EEPROM.read(16);
        config.ip[0] = EEPROM.read(32);
        config.ip[1] = EEPROM.read(33);
        config.ip[2] = EEPROM.read(34);
        config.ip[3] = EEPROM.read(35);
        config.netmask[0] = EEPROM.read(36);
        config.netmask[1] = EEPROM.read(37);
        config.netmask[2] = EEPROM.read(38);
        config.netmask[3] = EEPROM.read(39);
        config.gateway[0] = EEPROM.read(40);
        config.gateway[1] = EEPROM.read(41);
        config.gateway[2] = EEPROM.read(42);
        config.gateway[3] = EEPROM.read(43);
        config.ssid = ReadStringFromEEPROM(64);
        config.passphrase = ReadStringFromEEPROM(96);
        return true;        
    } else {
        Serial.println("Configurarion NOT FOUND!!!!");
        return false;
    }
}

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void E131Web::send_config_net_html() {
    if (server.args()) { // Save Settings
        String temp = "";
        config.dhcp = false;
        for ( uint8_t i = 0; i < server.args(); i++ ) {
            if (server.argName(i) == "ssid") config.ssid = server.arg(i);
            if (server.argName(i) == "password") config.passphrase = server.arg(i); 
            if (server.argName(i) == "ip_0") if (checkRange(server.arg(i))) config.ip[0] = server.arg(i).toInt();
            if (server.argName(i) == "ip_1") if (checkRange(server.arg(i))) config.ip[1] = server.arg(i).toInt();
            if (server.argName(i) == "ip_2") if (checkRange(server.arg(i))) config.ip[2] = server.arg(i).toInt();
            if (server.argName(i) == "ip_3") if (checkRange(server.arg(i))) config.ip[3] = server.arg(i).toInt();
            if (server.argName(i) == "nm_0") if (checkRange(server.arg(i))) config.netmask[0] = server.arg(i).toInt();
            if (server.argName(i) == "nm_1") if (checkRange(server.arg(i))) config.netmask[1] = server.arg(i).toInt();
            if (server.argName(i) == "nm_2") if (checkRange(server.arg(i))) config.netmask[2] = server.arg(i).toInt();
            if (server.argName(i) == "nm_3") if (checkRange(server.arg(i))) config.netmask[3] = server.arg(i).toInt();
            if (server.argName(i) == "gw_0") if (checkRange(server.arg(i))) config.gateway[0] = server.arg(i).toInt();
            if (server.argName(i) == "gw_1") if (checkRange(server.arg(i))) config.gateway[1] = server.arg(i).toInt();
            if (server.argName(i) == "gw_2") if (checkRange(server.arg(i))) config.gateway[2] = server.arg(i).toInt();
            if (server.argName(i) == "gw_3") if (checkRange(server.arg(i))) config.gateway[3] = server.arg(i).toInt();
            if (server.argName(i) == "dhcp") config.dhcp = true;
        }
//        server.send ( 200, "text/html", PAGE_WaitAndReload ); 
//        writeConfig();
//        configureWiFi();
    } else {
        server.send(200, "text/html", PAGE_CONFIG_NET); 
    }
}

//
//   FILL THE PAGE WITH VALUES
//

void E131Web::send_config_net_vals_html() {
    String values ="";
    values += "ssid|" + (String)config.ssid + "|input\n";
    values += "password|" + (String)config.passphrase + "|input\n";
    values += "ip_0|" + (String)config.ip[0] + "|input\n";
    values += "ip_1|" + (String)config.ip[1] + "|input\n";
    values += "ip_2|" + (String)config.ip[2] + "|input\n";
    values += "ip_3|" + (String)config.ip[3] + "|input\n";
    values += "nm_0|" + (String)config.netmask[0] + "|input\n";
    values += "nm_1|" + (String)config.netmask[1] + "|input\n";
    values += "nm_2|" + (String)config.netmask[2] + "|input\n";
    values += "nm_3|" + (String)config.netmask[3] + "|input\n";
    values += "gw_0|" + (String)config.gateway[0] + "|input\n";
    values += "gw_1|" + (String)config.gateway[1] + "|input\n";
    values += "gw_2|" + (String)config.gateway[2] + "|input\n";
    values += "gw_3|" + (String)config.gateway[3] + "|input\n";
    values += "dhcp|" + (String)(config.dhcp ? "checked" : "") + "|chk\n";
    server.send(200, "text/plain", values);
}

//
//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
//
void E131Web::send_connection_state_vals_html() {
    String state = "N/A";
    String Networks = "";
    if (WiFi.status() == 0) state = "Idle";
    else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
    else if (WiFi.status() == 2) state = "SCAN COMPLETED";
    else if (WiFi.status() == 3) state = "CONNECTED";
    else if (WiFi.status() == 4) state = "CONNECT FAILED";
    else if (WiFi.status() == 5) state = "CONNECTION LOST";
    else if (WiFi.status() == 6) state = "DISCONNECTED";

     int n = WiFi.scanNetworks();
 
     if (n == 0) {
         Networks = "<font color='#FF0000'>No networks found!</font>";
     } else {
        Networks = "Found " +String(n) + " Networks<br>";
        Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
        Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
        for (int i = 0; i < n; ++i) {
            int quality=0;
            if(WiFi.RSSI(i) <= -100) {
                    quality = 0;
            } else if(WiFi.RSSI(i) >= -50) {
                    quality = 100;
            } else {
                quality = 2 * (WiFi.RSSI(i) + 100);
            }
            Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
        }
        Networks += "</table>";
    }
   
    String values = "";
    values += "connectionstate|" + state + "|div\n";
    values += "networks|" + Networks + "|div\n";
    server.send(200, "text/plain", values);
}

void E131Web::send_config_dmx_html() {
    if (server.args() > 0) {  // Save Settings
        config.multicast = false;
        String temp = "";
        for (uint8_t i = 0; i < server.args(); i++) {
            if (server.argName(i) == "universe") config.universe = server.arg(i).toInt(); 
            if (server.argName(i) == "channel_start") config.channel_start = server.arg(i).toInt();
            if (server.argName(i) == "channel_count") config.channel_count = server.arg(i).toInt();
            if (server.argName(i) == "multicast") config.multicast = true; 
        }
//      WriteConfig();
    }
    server.send(200, "text/html", PAGE_CONFIG_DMX);
}

void E131Web::send_config_dmx_vals_html() {
    String values = "";
    values += "universe|" + (String)config.universe + "|input\n";
    values += "channel_start|" + (String)config.channel_start + "|input\n";
    values += "channel_count|" + (String)config.channel_count + "|input\n";
    values += "multicast|" + (String)(config.multicast ? "checked" : "") + "|chk\n";
    server.send(200, "text/plain", values);
}

//
// FILL WITH INFOMATION
// 

void E131Web::send_status_net_vals_html() {
    String values = "";
    values += "x_ssid|" + (String)WiFi.SSID() + "|div\n";
    values += "x_ip|" + (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] + "|div\n";
    values += "x_gateway|" + (String)WiFi.gatewayIP()[0] + "." + (String)WiFi.gatewayIP()[1] + "." + (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] + "|div\n";
    values += "x_netmask|" + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1] + "." + (String) WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] + "|div\n";
    values += "x_mac|" + GetMacAddress() + "|div\n";
    server.send(200, "text/plain", values);
}

//
// FILL WITH INFOMATION
// 

void E131Web::send_status_dmx_vals_html() {
    String values = "";
    values += "universe|" + (String)e131->universe + "|div\n";
    values += "num_packets|" + (String)e131->stats.num_packets + "|div\n";
    values += "sequence_errors|" + (String)e131->stats.sequence_errors + "|div\n";
    values += "packet_errors|" + (String)e131->stats.packet_errors + "|div\n";
    server.send ( 200, "text/plain", values);
}

#endif