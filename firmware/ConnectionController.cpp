

#include "ConnectionController.h"
#include "deviceconfig.h"
#include "msgparser.h"
#include "io.h"

ConnectionController::ConnectionController(): 
        proxyAddress(PROXY_ADDRESS_MSB, PROXY_ADDRESS_LSB),
        bridge(&xbee), 
        mqtt((MQTTMessageReceiver*)this, &bridge){


    this->status = ConnectionController::DISCONNECTED;

    this->ledIsOn = false;
    this->blinksDone=0;
    this->lastBlinkTime = millis();
    this->lastDisconnectTime = RECONNECT_INTERVAL;

}


void ConnectionController::begin(){
    xbee.begin(9600);
    this->lastBlinkTime = millis();
    MSGPARSER.setMessagePublisher((MQTTMessagePublisher*)&mqtt);
}


bool ConnectionController::publish(char* topic, char* msg){
    return this->mqtt.publish(topic, msg);
}


void ConnectionController::messageReceived(char* topic, uint8_t* payload, unsigned int length){
   char tmpBuff[255];
    unsigned int i;


 //   length = 4;
    for (i = 0; i<length; i++)
        tmpBuff[i] = payload[i];

    tmpBuff[i] = '\0';

    MSGPARSER.handleMsg(topic, tmpBuff);

}


/*
 * Requests the proxy to connect and moves to the CONNECTING state
 * TODO:
 *  It should keep track of the retries in case of failure and
 *  have some other state like WAITING or whatever.
 */
void ConnectionController::doDisconnectedStuff(){
    // Request connection
    unsigned long now = millis();
    if ((now - lastDisconnectTime) > RECONNECT_INTERVAL){
        mqtt.connect(DEVICE_NAME);
        //mqtt.connect(CONFIG.getDeviceName());
        this->status = ConnectionController::CONNECTING;
    }


}

/*
 * Just waits until either the proxy is connected, or fails.
 * If connected moves to the state SUBSCRIBING
 * If failed, move to the state DISCONNECTED
 */
void ConnectionController::doConnectingStuff(){

    // mqtt should be on one of it connection phases, or connected, but not
    // disconnected. If it is disconencted something went wrong
    
    if (mqtt.getStatus() == PubSubClient::DISCONNECTED){
        this->status = ConnectionController::DISCONNECTED;
        lastDisconnectTime = millis();
    }


    // Good, move to the next phase
    if (mqtt.getStatus() == PubSubClient::CONNECTED){
        this->status = ConnectionController::SUBSCRIBING;
        this->subscriptionStep = 0;
        
    }

}


/*
 * Handles the subscribe procedure, this involves subscribing
 * to the different needed topics, one per loop.
 * Once done moves to the ONLINE state.
 */
void ConnectionController::doSubscribingStuff(){

    char topic[255];


    if (mqtt.getStatus() != PubSubClient::CONNECTED){
        this->status = ConnectionController::DISCONNECTED;
        lastDisconnectTime = millis();
        return;
    }



    snprintf(topic, 255, "%s/#", STR_ZONE_NAME);
    mqtt.subscribe(topic);

    this->status = ConnectionController::ONLINE;

}


void ConnectionController::doOnlineStuff(){
    //TODO check if disconnected
    if (mqtt.getStatus() != PubSubClient::CONNECTED){
        this->status = ConnectionController::DISCONNECTED;
        lastDisconnectTime = millis();
        return;
    }



}


void ConnectionController::blinkLed(){
    // Blink to know we are alive

    uint8_t blinksToDo = 0;
    unsigned long now = millis();

    switch (this->status){
        case ConnectionController::DISCONNECTED:
            blinksToDo = 1;
            break;

        case ConnectionController::CONNECTING:
            blinksToDo = 2;
            break;


        case ConnectionController::SUBSCRIBING:
            blinksToDo = 3;
            break;

        case ConnectionController::ONLINE:
            blinksToDo = 4;
            break;

    }


    if (this->blinksDone > blinksToDo){

        IO.setAliveLed(false);

        if ((now - lastBlinkTime) > LED_BLINK_INTERVAL_LONG){
            lastBlinkTime = now;
            this->blinksDone = 0;
        }
    }
    else {
        if ((now - lastBlinkTime) > LED_BLINK_INTERVAL_SHORT){
            lastBlinkTime = now;
            IO.setAliveLed(ledIsOn);
            if (ledIsOn)
                this->blinksDone += 1;
            ledIsOn = !ledIsOn;
        }

    }






}

void ConnectionController::loop(){
    blinkLed();
    bridge.loop();
    mqtt.loop();


    switch (this->status){
        case ConnectionController::DISCONNECTED:
            doDisconnectedStuff();
            break;

        case ConnectionController::CONNECTING:
            doConnectingStuff();
            break;


        case ConnectionController::SUBSCRIBING:
            doSubscribingStuff();
            break;

        case ConnectionController::ONLINE:
            doOnlineStuff();
            break;

    }



}

ConnectionController CONNECTION_CONTROLLER;
