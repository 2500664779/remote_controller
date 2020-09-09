/********************************
 *      传感器接口
 *     提供getJson方式
 * 
 * 
 * ******************************/



#ifndef __ISRNSOR_H__
#define __ISRNSOR_H__

#include <string>
namespace Sensor{
    
class ISensor
{
    public:
        virtual ~ISensor(){};
        virtual char* getInfo() = 0;
};

}; // namespace Sensor

#endif