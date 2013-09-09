#!/usr/bin/python

import sys
from pyBuspirateLite.BitBang import *


bbio = BBIO("/dev/ttyUSB0")
bbio.BBmode()
bbio.enter_SPI()

bbio.cfg_pins(PinCfg.CS|PinCfg.POWER)

print "Press enter to power-off"
sys.stdin.readline()

bbio.cfg_pins(0x00)
bbio.reset()
bbio.resetBP()

