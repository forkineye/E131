/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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

#define ACCESS_POINT_NAME  "ESP"                
#define ACCESS_POINT_PASSWORD  "12345678" 
#define AdminTimeOut 180  // Defines the Time in Seconds, when the Admin-Mode will be diabled


/* Static Defines  */
boolean E131Web::firstStart = true;
int E131Web::AdminTimeOutCounter = 0;
boolean E131Web::AdminEnabled = false;
ESP8266WebServer E131Web::server = ESP8266WebServer(E131_WEB_PORT);
config_t E131Web::config;

E131Web::E131Web() {
//    firstStart = true;          // On firststart = true, NTP will try to get a valid time
//    AdminTimeOutCounter = 0;    // Counter for Disabling the AdminMode
//    AdminEnabled = false;       // Enable Admin Mode for a given Time
}

void E131Web::begin() {
    AdminEnabled = false;       // Enable Admin Mode for a given Time
//    EEPROM.begin(512);
//    Serial.begin(115200);
//    delay(500);
//    Serial.println("Starting WebConfig");
//  if (!ReadConfig())
//  {
        // DEFAULT CONFIG
        config.ssid = "huzzah";
        config.password = "omgthisismywirelesskeyhaha";
        config.dhcp = true;
        config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
        config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
        config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
        config.ntpServerName = "0.de.pool.ntp.org";
        config.Update_Time_Via_NTP_Every =  0;
        config.timezone = -10;
        config.daylight = true;
        config.DeviceName = "Not Named";
        config.AutoTurnOff = false;
        config.AutoTurnOn = false;
        config.TurnOffHour = 0;
        config.TurnOffMinute = 0;
        config.TurnOnHour = 0;
        config.TurnOnMinute = 0;
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

//  server.on ( "/", processExample  );
    /* JavaScript and Stylesheets */
    server.on ("/style.css", []() { Serial.println("style.css"); server.send (200, "text/plain", PAGE_STYLE_CSS); });
    server.on ("/microajax.js", []() { Serial.println("microajax.js"); server.send (200, "text/plain", PAGE_MICROAJAX_JS); });

    /* HTML Pages */
    server.on("/", []() { server.send(200, "text/html", PAGE_ROOT); });
    server.on("/config/net.html", send_config_net_html);
    server.on("/config/dmx.html", send_config_dmx_html);
    server.on("/status/net.html", []()  { server.send(200, "text/html", PAGE_STATUS_NET); });
    server.on("/status/dmx.html", []()  { server.send(200, "text/html", PAGE_STATUS_DMX); });

    /* AJAX Handlers */
    server.on ("/config/netvals", send_config_net_vals_html);
    server.on ("/config/dmxvals", send_config_dmx_vals_html);
    server.on ("/config/connectionstate", send_connection_state_vals_html);
    server.on ("/status/netvals", send_status_net_vals_html);
    server.on ("/status/dmxvals", send_status_dmx_vals_html);
   
    

//    server.on ( "/admin/infovalues", send_information_values_html );
//    server.on ( "/admin/devicename",     send_devicename_value_html);

    server.onNotFound([]() { Serial.println("Page Not Found"); server.send ( 404, "text/html", "Page not Found" ); });
    server.begin();

    if (Serial) {
        Serial.print(F("- Web Server started on port "));
        Serial.println(E131_WEB_PORT);
    }
}

 
void E131Web::check() {
    /*
    if (AdminEnabled)
    {
        if (AdminTimeOutCounter > AdminTimeOut)
        {
             AdminEnabled = false;
             Serial.println("Admin Mode disabled!");
             WiFi.mode(WIFI_STA);
        }
    }
    */

    server.handleClient();


    /*
    *    Your Code here
    */
}


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

boolean E131Web::readConfig()
{
    Serial.println("Reading Configuration");
    if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
    {
        Serial.println("Configurarion Found!");
        config.dhcp =   EEPROM.read(16);

        config.daylight = EEPROM.read(17);

        config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

        config.timezone = EEPROMReadlong(22); // 4 Byte

        config.LED_R = EEPROM.read(26);
        config.LED_G = EEPROM.read(27);
        config.LED_B = EEPROM.read(28);

        config.IP[0] = EEPROM.read(32);
        config.IP[1] = EEPROM.read(33);
        config.IP[2] = EEPROM.read(34);
        config.IP[3] = EEPROM.read(35);
        config.Netmask[0] = EEPROM.read(36);
        config.Netmask[1] = EEPROM.read(37);
        config.Netmask[2] = EEPROM.read(38);
        config.Netmask[3] = EEPROM.read(39);
        config.Gateway[0] = EEPROM.read(40);
        config.Gateway[1] = EEPROM.read(41);
        config.Gateway[2] = EEPROM.read(42);
        config.Gateway[3] = EEPROM.read(43);
        config.ssid = ReadStringFromEEPROM(64);
        config.password = ReadStringFromEEPROM(96);
        config.ntpServerName = ReadStringFromEEPROM(128);
        
        
        config.AutoTurnOn = EEPROM.read(300);
        config.AutoTurnOff = EEPROM.read(301);
        config.TurnOnHour = EEPROM.read(302);
        config.TurnOnMinute = EEPROM.read(303);
        config.TurnOffHour = EEPROM.read(304);
        config.TurnOffMinute = EEPROM.read(305);
        config.DeviceName= ReadStringFromEEPROM(306);
        return true;
        
    }
    else
    {
        Serial.println("Configurarion NOT FOUND!!!!");
        return false;
    }
}

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void E131Web::send_config_net_html()
{
    
    if (server.args() > 0 )  // Save Settings
    {
        String temp = "";
        config.dhcp = false;
        for ( uint8_t i = 0; i < server.args(); i++ ) {
            if (server.argName(i) == "ssid") config.ssid =  server.arg(i);
            if (server.argName(i) == "password") config.password =  server.arg(i); 
            if (server.argName(i) == "ip_0") if (checkRange(server.arg(i)))     config.IP[0] =  server.arg(i).toInt();
            if (server.argName(i) == "ip_1") if (checkRange(server.arg(i)))     config.IP[1] =  server.arg(i).toInt();
            if (server.argName(i) == "ip_2") if (checkRange(server.arg(i)))     config.IP[2] =  server.arg(i).toInt();
            if (server.argName(i) == "ip_3") if (checkRange(server.arg(i)))     config.IP[3] =  server.arg(i).toInt();
            if (server.argName(i) == "nm_0") if (checkRange(server.arg(i)))     config.Netmask[0] =  server.arg(i).toInt();
            if (server.argName(i) == "nm_1") if (checkRange(server.arg(i)))     config.Netmask[1] =  server.arg(i).toInt();
            if (server.argName(i) == "nm_2") if (checkRange(server.arg(i)))     config.Netmask[2] =  server.arg(i).toInt();
            if (server.argName(i) == "nm_3") if (checkRange(server.arg(i)))     config.Netmask[3] =  server.arg(i).toInt();
            if (server.argName(i) == "gw_0") if (checkRange(server.arg(i)))     config.Gateway[0] =  server.arg(i).toInt();
            if (server.argName(i) == "gw_1") if (checkRange(server.arg(i)))     config.Gateway[1] =  server.arg(i).toInt();
            if (server.argName(i) == "gw_2") if (checkRange(server.arg(i)))     config.Gateway[2] =  server.arg(i).toInt();
            if (server.argName(i) == "gw_3") if (checkRange(server.arg(i)))     config.Gateway[3] =  server.arg(i).toInt();
            if (server.argName(i) == "dhcp") config.dhcp = true;
        }
        server.send ( 200, "text/html", PAGE_WaitAndReload ); 
        writeConfig();
        configureWiFi();
        AdminTimeOutCounter=0;
    }
    else
    {
        server.send ( 200, "text/html", PAGE_NetworkConfiguration ); 
    }
    Serial.println(__FUNCTION__); 
}

//
//   FILL THE PAGE WITH VALUES
//

void E131Web::send_config_net_vals_html()
{

    String values ="";

    values += "ssid|" + (String) config.ssid + "|input\n";
    values += "password|" +  (String) config.password + "|input\n";
    values += "ip_0|" +  (String) config.IP[0] + "|input\n";
    values += "ip_1|" +  (String) config.IP[1] + "|input\n";
    values += "ip_2|" +  (String) config.IP[2] + "|input\n";
    values += "ip_3|" +  (String) config.IP[3] + "|input\n";
    values += "nm_0|" +  (String) config.Netmask[0] + "|input\n";
    values += "nm_1|" +  (String) config.Netmask[1] + "|input\n";
    values += "nm_2|" +  (String) config.Netmask[2] + "|input\n";
    values += "nm_3|" +  (String) config.Netmask[3] + "|input\n";
    values += "gw_0|" +  (String) config.Gateway[0] + "|input\n";
    values += "gw_1|" +  (String) config.Gateway[1] + "|input\n";
    values += "gw_2|" +  (String) config.Gateway[2] + "|input\n";
    values += "gw_3|" +  (String) config.Gateway[3] + "|input\n";
    values += "dhcp|" +  (String) (config.dhcp ? "checked" : "") + "|chk\n";
    server.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 
    
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
   
    String values ="";
    values += "connectionstate|" +  state + "|div\n";
    values += "networks|" +  Networks + "|div\n";
    server.send ( 200, "text/plain", values);
//    Serial.println(__FUNCTION__); 
}

// Functions for this Page
/*
void send_devicename_value_html()
{
        
    String values ="";
    values += "devicename|" + (String) config.DeviceName + "|div\n";
    server.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 
    
}
*/

void E131Web::send_config_dmx_html()
{
    
    if (server.args() > 0 )  // Save Settings
    {
        config.AutoTurnOn = false;
        config.AutoTurnOff = false;
        String temp = "";
        for ( uint8_t i = 0; i < server.args(); i++ ) {
            if (server.argName(i) == "devicename") config.DeviceName =  server.arg(i); 
            if (server.argName(i) == "tonenabled") config.AutoTurnOn = true; 
            if (server.argName(i) == "toffenabled") config.AutoTurnOff = true; 
            if (server.argName(i) == "tonhour") config.TurnOnHour =  server.arg(i).toInt(); 
            if (server.argName(i) == "tonminute") config.TurnOnMinute =  server.arg(i).toInt(); 
            if (server.argName(i) == "toffhour") config.TurnOffHour =  server.arg(i).toInt(); 
            if (server.argName(i) == "toffminute") config.TurnOffMinute =  server.arg(i).toInt(); 
        }
//      WriteConfig();
        firstStart = true;
    }
    server.send ( 200, "text/html", PAGE_CONFIG_DMX ); 
    Serial.println(__FUNCTION__); 
    
    
}

void E131Web::send_config_dmx_vals_html()
{
    String values ="";
    values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
    values += "tonhour|" +  (String)  config.TurnOnHour +  "|input\n";
    values += "tonminute|" +   (String) config.TurnOnMinute +  "|input\n";
    values += "toffhour|" +  (String)  config.TurnOffHour +  "|input\n";
    values += "toffminute|" +   (String)  config.TurnOffMinute +  "|input\n";
    values += "toffenabled|" +  (String) (config.AutoTurnOff ? "checked" : "") + "|chk\n";
    values += "tonenabled|" +  (String) (config.AutoTurnOn ? "checked" : "") + "|chk\n";
    server.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 
}

//
// FILL WITH INFOMATION
// 

void E131Web::send_status_net_vals_html ()
{

    String values ="";

    values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
    values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
    values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
    values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
    values += "x_mac|" + GetMacAddress() +  "|div\n";
    server.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 

}

//
// FILL WITH INFOMATION
// 

void E131Web::send_status_dmx_vals_html ()
{

    String values ="";

    values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
    values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
    values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
    values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
    values += "x_mac|" + GetMacAddress() +  "|div\n";
    server.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 
}