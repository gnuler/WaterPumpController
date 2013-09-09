#ifndef __IO_H__
#define __IO_H__

#include "deviceconfig.h"



class InputOutput {

    private:

    public:
        // public methods
        InputOutput();

        void startMainPump();
        void stopMainPump();
        bool getMainPumpState();

        bool getBtnState();

        bool alarmHigh();
        bool alarmLow();

        void setAliveLed(bool state);
        void init();

    private:
        // priv methods


};

extern InputOutput IO;
#endif
