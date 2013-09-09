#ifndef __PUMP_CONTROLLER_H__
#define __PUMP_CONTROLLER_H__

#include "stdint.h"

struct PumpControllerConfig {
    bool autoStart;
    bool autoStop;
};


class PumpController {

    private:
        struct PumpControllerConfig config;

    public:
        PumpController();

        void saveConfig();
        void loadConfig();
        
        void setAutoStop(bool value);
        bool getAutoStop();
        void setAutoStart(bool value);
        bool getAutoStart();

        void startMainPump();
        void stopMainPump();
        bool getMainPumpState();

        void publishMainPumpState();
        void publishWaterLevel();
        void publishAutoStopValue();
        void publishAutoStartValue();


        uint8_t getWaterLevel();

        void loop();

};


extern PumpController PUMP_CONTROLLER;

#endif
