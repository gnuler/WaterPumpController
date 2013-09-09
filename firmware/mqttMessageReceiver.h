#ifndef __MQTT_MESSAGE_RECEIVER_H__
#define __MQTT_MESSAGE_RECEIVER_H__

#include "common.h"

class MQTTMessageReceiver{


    public:
        virtual void messageReceived(char* topic, uint8_t* payload, unsigned int length);

};

#endif
