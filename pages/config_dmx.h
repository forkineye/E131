#ifndef PAGE_CONFIG_DMX_H
#define PAGE_CONFIG_DMX_H

const char PAGE_CONFIG_DMX[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<link rel="stylesheet" href="/style.css" type="text/css"/>
<script src="/microajax.js"></script>
<a href="/" class="btn btn--s"><</a>&nbsp;&nbsp;<strong>DMX Configuration</strong>
<hr>
<form action="" method="get">
<table border="0" cellspacing="0" cellpadding="3">
<tr><td align="right">Universe:</td><td><input type="text" id="universe" name="universe" value=""></td></tr>
<tr><td align="right">Start Channel:</td><td><input type="text" id="channel_start" name="channel_start" value=""></td></tr>
<tr><td align="right">Channel Count:</td><td><input type="text" id="channel_count" name="channel_count" value=""></td></tr>
<tr><td align="right">Enable Multicast:</td><td><input type="checkbox" id="multicast" name="multicast"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>
	setValues("/config/dmxvals");
</script>
)=====";
#endif