#ifndef HELPERS_H
#define HELPERS_H

//
// Check the Values is between 0-255
//
boolean checkRange(String Value)
{
	 if (Value.toInt() < 0 || Value.toInt() > 255)
	 {
		 return false;
	 }
	 else
	 {
		 return true;
	 }
}


void WriteStringToEEPROM(int beginaddress, String string)
{
	char  charBuf[string.length()+1];
	string.toCharArray(charBuf, string.length()+1);
	for (int t=  0; t<sizeof(charBuf);t++)
	{
			EEPROM.write(beginaddress + t,charBuf[t]);
	}
}
String  ReadStringFromEEPROM(int beginaddress)
{
	byte counter=0;
	char rChar;
	String retString = "";
	while (1)
	{
		rChar = EEPROM.read(beginaddress + counter);
		if (rChar == 0) break;
		if (counter > 31) break;
		counter++;
		retString.concat(rChar);

	}
	return retString;
}
void EEPROMWritelong(int address, long value)
      {
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

String GetMacAddress()
{
	uint8_t mac[6];
    char macStr[18] = {0};
	WiFi.macAddress(mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return  String(macStr);
}

#endif