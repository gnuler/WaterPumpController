
#include <avr/interrupt.h>
#include "common.h"
#include "ConnectionController.h"
#include "PumpController.h"
#include <util/delay.h>
#include "io.h"

#include "HardwareSerial.h"

int main(void) 
{
    initTimer0();
	sei();

    IO.init();

    CONNECTION_CONTROLLER.begin();

    _delay_ms(1000);

    while (1){
        CONNECTION_CONTROLLER.loop();
        PUMP_CONTROLLER.loop();
	}

}


