#ifndef __CONNECTION_CONTROLLER_H__
#define __CONNECTION_CONTROLLER_H__

#include "mqttMessageReceiver.h"
#include "XBee.h"
#include "xbeetcpbridge.h"
#include "PubSubClient.h"

#define LED_BLINK_INTERVAL_SHORT 500UL  // 
#define LED_BLINK_INTERVAL_LONG 2000UL  // approx 1/2 sec 
#define RECONNECT_INTERVAL 10000UL // approx 5 sec

class ConnectionController : public MQTTMessageReceiver {

    private:
        enum Status  {
            DISCONNECTED       = 0,
            CONNECTING         = 1,
            SUBSCRIBING        = 2,
            ONLINE             = 3
            };

        Status status;

        XBee xbee;
        XBeeAddress64 proxyAddress;
   
        XBeeTCPBridge bridge;
        PubSubClient mqtt;

        uint8_t subscriptionStep;


        bool ledIsOn ;
        uint8_t blinksDone ;

        unsigned long lastBlinkTime;
        unsigned long lastDisconnectTime;

        void doDisconnectedStuff();
        void doConnectingStuff();
        void doConnectedStuff();
        void doSubscribingStuff();
        void doOnlineStuff();

        void blinkLed();

    public:
        ConnectionController();
        void messageReceived(char* topic, uint8_t* payload, unsigned int length);

        bool publish(char* topic, char* msg);
        void loop();
        void begin();




};

extern ConnectionController CONNECTION_CONTROLLER;
#endif
