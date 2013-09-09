
#include "ConnectionController.h"
#include "PumpController.h"
#include "io.h"

#include <avr/eeprom.h>

struct PumpControllerConfig EEMEM ee_pumpControllerConfig = {
    false,
    false,
    };


PumpController::PumpController(){
    this->loadConfig();
}


void PumpController::saveConfig(){
    // Update EEPROM
    eeprom_busy_wait();
    eeprom_update_block((void*)&this->config, &ee_pumpControllerConfig,sizeof(struct PumpControllerConfig));

}

void PumpController::loadConfig(){
    // Update EEPROM
    eeprom_busy_wait();
    eeprom_read_block((void*)&this->config, &ee_pumpControllerConfig, sizeof(struct PumpControllerConfig));
}


void PumpController::setAutoStop(bool value){
    this->config.autoStop = value;
    this->saveConfig();
}

bool PumpController::getAutoStop(){
    return this->config.autoStop;
}


void PumpController::setAutoStart(bool value){
    this->config.autoStart = value;
    this->saveConfig();
}

bool PumpController::getAutoStart(){
    return this->config.autoStart;
}


void PumpController::startMainPump(){
    IO.startMainPump();
//    CONNECTION_CONTROLLER.publish("...","");
}

void PumpController::stopMainPump(){
    IO.stopMainPump();
}

bool PumpController::getMainPumpState(){
    return IO.getMainPumpState();
}



void PumpController::publishMainPumpState(){
}

void PumpController::publishWaterLevel(){
}

void PumpController::publishAutoStopValue(){
}

void PumpController::publishAutoStartValue(){
}



void PumpController::loop(){

    return;
    //if [Pump is ON]:
    if (IO.getMainPumpState() == true){
        if (this->getAutoStop()){
            if (IO.alarmHigh()){
                IO.stopMainPump();
            }
        }
        else {
            //   if [START/STOP BTN pressed]
            //      stop_pump
        }
    }

    //[Pump is OFF]:
    else {
        if (this->getAutoStart()){
            if (IO.alarmLow()){
                IO.startMainPump();
            }
        }
        else {
            //  else if [START/STOP BTN pressed]
            //      start_pump
        }
    }



}

PumpController PUMP_CONTROLLER;
