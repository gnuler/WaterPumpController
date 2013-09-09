#ifndef MSG_PARSER_H
#define MSG_PARSER_H

#include "mqttMessagePublisher.h"
#include "deviceconfig.h"

// Actions & Values
#define STR_ON "On"
#define STR_ON_LEN strlen(STR_ON)

#define STR_OFF "Off"
#define STR_OFF_LEN strlen(STR_OFF)

#define STR_STATUS "Status"
#define STR_STATUS_LEN strlen(STR_STATUS)

#define STR_GET "Get"
#define STR_GET_LEN strlen(STR_GET)

#define STR_SET "Set"
#define STR_SET_LEN strlen(STR_SET)



class MsgParser {


    private:
        MQTTMessagePublisher* publisher;
        bool publish(char* topic, char* msg);

    public:

        MsgParser();

        void setMessagePublisher(MQTTMessagePublisher* publisher);

        // Parses the message and calls the corresponding 
        // handler
        void handleMsg(char* topic, char* payload);


    private:

 
        void handleZoneMsg(char* payload);

        void handlePumpMsg(char* payload);
        void handlePumpMainMsg(char* payload);
        void handleSensorMsg(char* payload);
        void handleSensorAlarmLowMsg(char* payload);
        void handleSensorAlarmHighMsg(char* payload);

        void handleConfigMsg(char* payload);
        void handleConfigAutoStartMsg(char* payload);
        void handleConfigAutoStopMsg(char* payload);

    
};

extern MsgParser MSGPARSER;
#endif
