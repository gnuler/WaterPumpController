

avrdude -p atmega328 -c buspirate -B 32 -U eeprom:r:-:i -P /dev/ttyUSB0
#avrdude -p atmega328 -c buspirate -B 32 -U eeprom:r:mydata.eep:r -P /dev/ttyUSB0
