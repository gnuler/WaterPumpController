/*
 PubSubClient.h - A simple client for MQTT.
  Nicholas O'Leary
  http://knolleary.net
*/

#ifndef PubSubClient_h
#define PubSubClient_h

#include "xbeetcpbridge.h"
#include "common.h"
#include "mqttMessagePublisher.h"
#include "mqttMessageReceiver.h"
#include <inttypes.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define boolean bool

// MQTT_MAX_PACKET_SIZE : Maximum packet size
#define MQTT_MAX_PACKET_SIZE 128

// MQTT_KEEPALIVE : keepAlive interval in Seconds
#define MQTT_KEEPALIVE 15

#define MQTTPROTOCOLVERSION 3
#define MQTTCONNECT     1 << 4  // Client request to connect to Server
#define MQTTCONNACK     2 << 4  // Connect Acknowledgment
#define MQTTPUBLISH     3 << 4  // Publish message
#define MQTTPUBACK      4 << 4  // Publish Acknowledgment
#define MQTTPUBREC      5 << 4  // Publish Received (assured delivery part 1)
#define MQTTPUBREL      6 << 4  // Publish Release (assured delivery part 2)
#define MQTTPUBCOMP     7 << 4  // Publish Complete (assured delivery part 3)
#define MQTTSUBSCRIBE   8 << 4  // Client Subscribe request
#define MQTTSUBACK      9 << 4  // Subscribe Acknowledgment
#define MQTTUNSUBSCRIBE 10 << 4 // Client Unsubscribe request
#define MQTTUNSUBACK    11 << 4 // Unsubscribe Acknowledgment
#define MQTTPINGREQ     12 << 4 // PING Request
#define MQTTPINGRESP    13 << 4 // PING Response
#define MQTTDISCONNECT  14 << 4 // Client is Disconnecting
#define MQTTReserved    15 << 4 // Reserved

#define MQTTQOS0        (0 << 1)
#define MQTTQOS1        (1 << 1)
#define MQTTQOS2        (2 << 1)

class PubSubClient : public MQTTMessagePublisher {
public:
    enum Status  {
        DISCONNECTED = 0,
        CONNECTING_PHASE1   = 1,
        CONNECTING_PHASE2   = 2,
        CONNECTED    = 3
        };

private:

    char* id;
    char* user;
    char* pass;
    char* willTopic;
    uint8_t willQos;
    uint8_t willRetain;
    char*  willMessage;

    MQTTMessageReceiver* mqttReceiver;

    Status status;
    XBeeTCPBridge* _tcpbridge;
    uint8_t buffer[MQTT_MAX_PACKET_SIZE];
    uint16_t nextMsgId;
    unsigned long lastOutActivity;
    unsigned long lastInActivity;
    bool pingOutstanding;
    uint8_t *ip;
    char* domain;
    uint16_t port;
    bool _connected;


    void doConnectPhase1(void);
    void doConnectPhase2(void);
    void doConnectPhase3(void);

    void doConnectedStuff(void);

   //void (*callback)(char*,uint8_t*,unsigned int);
   uint16_t readPacket();
   uint8_t readByte();
   boolean write(uint8_t header, uint8_t* buf, uint16_t length);
   uint16_t writeString(char* string, uint8_t* buf, uint16_t pos);

public:
   PubSubClient(MQTTMessageReceiver* mqttReceiver, XBeeTCPBridge* tcpbridge);
   //PubSubClient(void(*)(char*,uint8_t*,unsigned int),XBeeTCPBridge* tcpbridge);
   //PubSubClient(uint8_t *, uint16_t, void(*)(char*,uint8_t*,unsigned int),XBeeTCPBridge* tcpbridge);
   //PubSubClient(char*, uint16_t, void(*)(char*,uint8_t*,unsigned int),XBeeTCPBridge* tcpbridge);

   boolean connect(char *);
   boolean connect(char *, char *, char *);
   boolean connect(char *, char *, uint8_t, uint8_t, char *);
   boolean connect(char *, char *, char *, char *, uint8_t, uint8_t, char*);

   Status getStatus(void);

   void disconnect();
   boolean publish(char *, char *);
   boolean publish(char *, uint8_t *, unsigned int);
   boolean publish(char *, uint8_t *, unsigned int, boolean);
   boolean publish_P(char *, uint8_t PROGMEM *, unsigned int, boolean);
   boolean subscribe(char *);


   boolean loop();
   boolean connected();
};


#endif
