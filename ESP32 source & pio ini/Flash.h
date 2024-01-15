#ifndef FLASH_H
#define FLASH_H

#include <Arduino.h>
#include "SPISlave.h"

class Flash : SPISlaveListener
{
    private:
        uint8_t pin;
    
    public:
        Flash(uint8_t pin) : pin(pin)
        {
            pinMode(this->pin, OUTPUT);
        }

        uint8_t onRequestReceive(RequestCode code, uint8_t data) override 
        {
            if (code == TURN_FLASH)
                analogWrite(this->pin, data);

            return NO_RESPONSE;
        }

};

#endif // FLASH_H