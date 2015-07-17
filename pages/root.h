#ifndef PAGE_ROOT_H
#define PAGE_ROOT_H
//
//  HTML PAGE
//

const char PAGE_ROOT[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<link rel="stylesheet" href="style.css" type="text/css" />
<strong>ESP8266 E1.31 Admin</strong>
<hr>
<a href="config/net.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="config/dmx.html" style="width:250px" class="btn btn--m btn--blue" >DMX Configuration</a><br>
<a href="status/net.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="status/dmx.html"   style="width:250px"  class="btn btn--m btn--blue" >E1.31 Status</a><br>
)=====";
#endif