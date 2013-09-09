

avrdude -p atmega328 -c buspirate -B 32 -U eeprom:w:myproject.ee.hex:i -P /dev/ttyUSB0
