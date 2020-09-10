#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__


class IController
{
    public:
        virtual ~IController(){};
        virtual void on() = 0;
        virtual void off() = 0;
};



#endif