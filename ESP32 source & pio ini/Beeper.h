#ifndef BEEPER_H
#define BEEPER_H

#include <stdlib.h>
#include <Arduino.h>
#include "SPISlave.h"

class Beeper : public SPISlaveListener
{
    private:
        uint8_t pin;
        SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
        //SpanCharacteristic* on;   // HomeSpan

        static void beepTask(void* params);

    public:
        Beeper(uint8_t pin);                // default ctor
        //Beeper(const Beeper& other);      // copy ctor
        //Beeper(Beeper&& other);           // move ctor
        //bool update() override;           // HomeSpan

        void beep(uint8_t power, uint8_t repet, uint16_t demiPeriodMs, bool turnOff);
        uint8_t onRequestReceive(RequestCode code, uint8_t data) override;
};

#endif // BEEPER_H