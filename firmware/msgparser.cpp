
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h> 

#include "PumpController.h"
#include "msgparser.h"
#include "io.h"

MsgParser::MsgParser(){
    this->publisher = NULL;
}

bool MsgParser::publish(char* topic, char* msg){
    if (this->publisher)
        return publisher->publish(topic, msg);
    return false;
}


void MsgParser::setMessagePublisher(MQTTMessagePublisher* publisher){
    this->publisher = publisher;
}

void MsgParser::handleMsg(char* topic, char* payload){

    char *token;

    token  = (char*) strtok(topic, "/");

    if (!token)
        return;

    if (strncmp (token, STR_WATER_TANK, STR_WATER_TANK_LEN) != 0)
        return;


    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    if (strncmp (token, STR_PUMP, STR_PUMP_LEN) == 0){
        this->handlePumpMsg(payload);
    }
    else if (strncmp (token, STR_SENSOR, STR_SENSOR_LEN) == 0){
        this->handleSensorMsg(payload);
    }
    else if (strncmp (token, STR_CONFIG, STR_CONFIG_LEN) == 0){
        this->handleConfigMsg(payload);
    }



}



/*****************************************************************************/
/*                                 PUMP                                      */
/*****************************************************************************/

void MsgParser::handlePumpMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    if (strncmp (token, STR_MAIN, STR_MAIN_LEN) == 0){
        this->handlePumpMainMsg(payload);
    }
 
}


void MsgParser::handlePumpMainMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    // Handle Set
    if (strncmp (token, STR_SET, STR_SET_LEN) == 0){

        if (strncmp (payload, STR_ON, STR_ON_LEN) == 0){
            PUMP_CONTROLLER.startMainPump();
        }
        else if (strncmp (payload, STR_OFF, STR_OFF_LEN) == 0){
            PUMP_CONTROLLER.stopMainPump();
        }

    }
    // Handler Get
    else if (strncmp (token, STR_GET, STR_GET_LEN) == 0){

        char topic[255];

        snprintf(topic, 255, "%s/%s/%s/%s",
            STR_WATER_TANK,
            STR_PUMP,
            STR_MAIN,
            STR_STATUS);

        if (PUMP_CONTROLLER.getMainPumpState()) {
            this->publish(topic, STR_ON);
        }
        else {
            this->publish(topic, STR_OFF);
        }

    }
 
}


/*****************************************************************************/
/*                                SENSOR                                     */
/*****************************************************************************/

void MsgParser::handleSensorMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    if (strncmp (token, STR_ALARM_HIGH, STR_ALARM_HIGH_LEN) == 0){
        this->handleSensorAlarmHighMsg(payload);
    }
    else if (strncmp (token, STR_ALARM_LOW, STR_ALARM_LOW_LEN) == 0){
        this->handleSensorAlarmLowMsg(payload);
    }

}

void MsgParser::handleSensorAlarmLowMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    // Handler Get
    if (strncmp (token, STR_GET, STR_GET_LEN) == 0){

        char topic[255];
        char value[5];

        snprintf(topic, 255, "%s/%s/%s/%s",
            STR_WATER_TANK,
            STR_SENSOR,
            STR_ALARM_LOW,
            STR_STATUS);

        if (IO.alarmLow()) {
            this->publish(topic, STR_ON);
        }
        else {
            this->publish(topic, STR_OFF);
        }
    }
 
}



void MsgParser::handleSensorAlarmHighMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    // Handler Get
    if (strncmp (token, STR_GET, STR_GET_LEN) == 0){

        char topic[255];
        char value[5];

        snprintf(topic, 255, "%s/%s/%s/%s",
            STR_WATER_TANK,
            STR_SENSOR,
            STR_ALARM_HIGH,
            STR_STATUS);

        if (IO.alarmHigh()) {
            this->publish(topic, STR_ON);
        }
        else {
            this->publish(topic, STR_OFF);
        }
    }
 
}




/*****************************************************************************/
/*                                CONFIG                                     */
/*****************************************************************************/

void MsgParser::handleConfigMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    if (strncmp (token, STR_AUTO_STOP, STR_AUTO_STOP_LEN) == 0){
        this->handleConfigAutoStopMsg(payload);
    }

    if (strncmp (token, STR_AUTO_START, STR_AUTO_START_LEN) == 0){
        this->handleConfigAutoStartMsg(payload);
    }

}

void MsgParser::handleConfigAutoStartMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    // Handle Set
    if (strncmp (token, STR_SET, STR_SET_LEN) == 0){

        if (strncmp (payload, STR_ON, STR_ON_LEN) == 0){
            PUMP_CONTROLLER.setAutoStart(true);
        }
        else if (strncmp (payload, STR_OFF, STR_OFF_LEN) == 0){
            PUMP_CONTROLLER.setAutoStart(false);
        }

    }
    // Handler Get
    else if (strncmp (token, STR_GET, STR_GET_LEN) == 0){

        char topic[255];

        snprintf(topic, 255, "%s/%s/%s/%s",
            STR_WATER_TANK,
            STR_CONFIG,
            STR_AUTO_START,
            STR_STATUS);

        if (PUMP_CONTROLLER.getAutoStart()) {
            this->publish(topic, STR_ON);
        }
        else {
            this->publish(topic, STR_OFF);
        }

    }
 

}

void MsgParser::handleConfigAutoStopMsg(char* payload){
    char *token;

    // Token is the zone name
    token = (char*) strtok(NULL, "/");

    if (!token)
        return;

    // Handle Set
    if (strncmp (token, STR_SET, STR_SET_LEN) == 0){

        if (strncmp (payload, STR_ON, STR_ON_LEN) == 0){
            PUMP_CONTROLLER.setAutoStop(true);
        }
        else if (strncmp (payload, STR_OFF, STR_OFF_LEN) == 0){
            PUMP_CONTROLLER.setAutoStop(false);
        }

    }
    // Handler Get
    else if (strncmp (token, STR_GET, STR_GET_LEN) == 0){

        char topic[255];

        snprintf(topic, 255, "%s/%s/%s/%s",
            STR_WATER_TANK,
            STR_CONFIG,
            STR_AUTO_STOP,
            STR_STATUS);

        if (PUMP_CONTROLLER.getAutoStop()) {
            this->publish(topic, STR_ON);
        }
        else {
            this->publish(topic, STR_OFF);
        }
    }
}


// Preinstantiate object
MsgParser MSGPARSER;
