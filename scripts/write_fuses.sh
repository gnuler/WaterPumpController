avrdude -p m328 -c buspirate  -P/dev/ttyUSB0 -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m 
