avrdude -p m328 -c buspirate  -P/dev/ttyUSB0 -U hfuse:r:high.txt:h -U lfuse:r:low.txt:h 

LOW=`cat low.txt`
HIGH=`cat high.txt`
echo "Low: $LOW"
echo "High: $HIGH"
rm high.txt
rm low.txt
