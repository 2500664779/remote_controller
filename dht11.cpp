#include "dht11.h"
#include <string>
#include <ctime>
#include <time.h>

Dht11::Dht11(int pin) : m_pin(pin){};
Dht11::~Dht11(){};

//try read 5times
//if failed return ""
char* Dht11::getInfo() {
    int trytimes = 5;
    while (trytimes--) {
        if (read(&m_temp, &m_mois) == 0 && m_temp != 0) break;
        
    }

    if (trytimes != -1) {
        sprintf(m_info, "temperature:%d,moisture:%d\n", m_temp, m_mois);
    }
    return m_info;
}

//dht读取函数
//正确返回0,否则返回-1
int Dht11::read(int *h, int *t) {
    uint8_t lststate=HIGH;
    uint8_t counter=0;
    uint8_t j=0,i;

    int dht11_val[5];
    for (i=0;i<5;i++) {
        dht11_val[i]=0;
    }

    pinMode(m_pin,OUTPUT);
    digitalWrite(m_pin,LOW);
    delay(18);
    digitalWrite(m_pin,HIGH);
    delayMicroseconds(40);
    pinMode(m_pin,INPUT);

    for (i=0;i<MAX_TIME;i++) {
        counter=0;
        while (digitalRead(m_pin)==lststate){
            counter++;
            delayMicroseconds(1);
            if (counter==255)
                break;
        }
        lststate=digitalRead(m_pin);
        if (counter==255)
             break;
        // top 3 transitions are ignored
        if ((i>=4) && (i%2==0)) {
            dht11_val[j/8]<<=1;
            if(counter>16)
                dht11_val[j/8]|=1;
            j++;
        }
    }

    // verify cheksum and print the verified data
    if ((j>=40) && (dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))) {
        // Only return the integer part of humidity and temperature. The sensor
        // is not accurate enough for decimals anyway 
        *h = dht11_val[0];
        *t = dht11_val[2];
        return 0;
    }
    else {
        // invalid data
        return -1;
    }
}

