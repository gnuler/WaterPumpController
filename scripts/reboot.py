#!/usr/bin/python

import sys, time
from pyBuspirateLite.BitBang import *
from pyBuspirateLite.SPI import *


bbio = BBIO("/dev/ttyUSB0")
bbio.BBmode()
#bbio.enter_SPI()

bbio.cfg_pins(0x00)

time.sleep(1)
bbio.cfg_pins(PinCfg.CS)



#time.sleep(1)

#bbio.resetBP()

