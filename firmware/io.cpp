#include "io.h"

#include <avr/eeprom.h>

InputOutput::InputOutput(){
}

void InputOutput::init(){
        // configure LED GPIO as output
        LED_ALIVE_DIR  |=  LED_ALIVE_BIT;

        // Set actuators as GPIO outputs
        PUMP_DIR |= PUMP_BIT;

        // Set inputs
        BTN_1_DIR         &= ~BTN_1_BIT;
        ALARM_HIGH_DIR    &= ~ALARM_HIGH_BIT;
        ALARM_LOW_DIR     &= ~ALARM_LOW_BIT;

        // Turn off immediately
        PUMP_PORT &= ~PUMP_BIT;

}


void InputOutput::startMainPump(){
    PUMP_PORT |= PUMP_BIT;
}

void InputOutput::stopMainPump(){
    PUMP_PORT &= ~PUMP_BIT;
}

bool InputOutput::getMainPumpState(){
    return (PUMP_PORT & PUMP_BIT);
}

bool InputOutput::getBtnState(){
    return (BTN_1_PORT & BTN_1_BIT);
}


bool InputOutput::alarmHigh(){
    return (ALARM_HIGH_PORT & ALARM_HIGH_BIT);
}


bool InputOutput::alarmLow(){
    return (ALARM_LOW_PORT & ALARM_LOW_BIT);
}


void InputOutput::setAliveLed(bool state){
    if (state)
        LED_ALIVE_PORT |=  LED_ALIVE_BIT;
    else
        LED_ALIVE_PORT &= ~LED_ALIVE_BIT;
}

// Preinstantiate object
InputOutput IO;
