#include "relay.h"

Relay::Relay(int pin) : m_pin(pin)
{
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
};

void Relay::on()
{
    digitalWrite(m_pin, HIGH);
}

void Relay::off()
{
    digitalWrite(m_pin, LOW);
}