#ifndef __MQTT_MESSAGE_PUBLISHER_H__
#define __MQTT_MESSAGE_PUBLISHER_H__

#include "common.h"

class MQTTMessagePublisher {


    public:
        virtual bool publish(char* topic, char* payload);

};

#endif
