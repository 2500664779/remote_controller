#ifndef __DHT11_H__
#define __DHT11_H__

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#include "sensor.h"

#include <string>


namespace Sensor{


//温湿度传感器类
//getJson方式返回Json格式字符串
class Dht11 : public ISensor
{
    public:
        Dht11(int pin);
        ~Dht11();
        char* getInfo();

    private:
        enum Limit
        {
            MAX_TIME = 85,
        };
        int read(int *mois, int *temp);

        //存储温度和湿度的变量
        int m_temp;
        int m_mois;

        //定义了引脚
        int m_pin;

        //定义了传感器的数据信息;
        char m_info[1024];
};

}; // namespace Sensor

#endif