#ifndef PAGE_CONFIG_DMX_H
#define PAGE_CONFIG_DMX_H
//
//  HTML PAGE
//

const char PAGE_CONFIG_DMX[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<link rel="stylesheet" href="/style.css" type="text/css" />
<script src="/microajax.js"></script>
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Name of Device</td>
	<td><input type="text" id="devicename" name="devicename" value=""></td>
</tr>

<tr>
	<td align="left" colspan="2"><hr></td>
</tr>

<tr>
	<td align="left" colspan="2">Turn on at</td>
</tr>
<tr>
	<td align="right"> Enabled:</td>
	<td><input type="checkbox" id="tonenabled" name="tonenabled"></td>
</tr>

<tr>
	<td align="right"> Time:</td>
	<td><input type="text" id="tonhour" name="tonhour" size="2" value="00">:<input type="text" id="tonminute" name="tonminute" size="2" value="00"></td>
</tr>

<tr>
	<td align="left" colspan="2">Turn off at:</td>
<tr>
	<td align="right"> Enabled:</td>
	<td><input type="checkbox" id="toffenabled" name="toffenabled"></td>
</tr>
<tr>
	<td align="right"> Time:</td>
	<td><input type="text" id="toffhour" name="toffhour" size="2" value="00">:<input type="text" id="toffminute" name="toffminute" size="2" value="00"></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>
	setValues("/admin/generalvalues");
</script>
)=====";
#endif