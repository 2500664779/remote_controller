#ifndef __RELAY_H__
#define __RELAY_H__

#include <wiringPi.h>

#include "controller.h"

class Relay: public IController
{
    public:
        Relay(int pin);
        ~Relay(){};
        void on();
        void off();

    private:
        int m_pin;
};


#endif