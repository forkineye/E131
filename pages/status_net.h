#ifndef PAGE_STATUS_NET_H
#define PAGE_STATUS_NET_H

//
//   The HTML PAGE
//
const char PAGE_STATUS_NET[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<link rel="stylesheet" href="/style.css" type="text/css" />
<script src="/microajax.js"></script> 
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Network Information</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask :</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway :</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>
	setTimeout(GetState,1000);
	function GetState()
	{
		setValues("/status/netvals");
	}
</script>
)=====" ;
#endif
