#!/usr/bin/python

import sys, time
from pyBuspirateLite.BitBang import *


bbio = BBIO("/dev/ttyUSB0")
bbio.BBmode()
bbio.enter_SPI()

bbio.cfg_pins(PinCfg.CS)


bbio.cfg_pins(0x00)
print "1"

time.sleep(10)

bbio.reset()
print "2"


time.sleep(10)


bbio.resetBP()

