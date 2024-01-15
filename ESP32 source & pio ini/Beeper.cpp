#include "Beeper.h"
#include <chrono>
#include <thread>

#define DEBUG_2   // To prevent error due to GPIO 16 during debug mode

// PUBLIC

/**
 * default ctor
*/
Beeper::Beeper(uint8_t pin)
{
    //this->on = new Characteristic::On();
    this->pin = pin;

    #ifndef DEBUG_2
    pinMode(pin, OUTPUT);
    analogWrite(this->pin, 0);
    #endif
}

/**
 * copy ctor
*/
//Beeper::Beeper(const Beeper& other) : pin(other.pin) { }

/**
 * move ctor
*/
//Beeper::Beeper(Beeper&& other) : pin(other.pin) { }

/*
bool Beeper::update()
{
    if (!this->on->getNewVal())
        return false;

    this->beep(150, 1, 100, false);
    this->beep(100, 1, 100, false);
    this->beep(50, 1, 100, true);
    return true;
}
*/


/**
 * FreeRTOS thread safety
*/
void Beeper::beep(uint8_t power, uint8_t repet, uint16_t demiPeriodMs, bool turnOff)
{
    std::tuple<Beeper*, uint8_t, uint8_t, uint16_t, bool> params = std::make_tuple(this, power, repet, demiPeriodMs, turnOff);
    uint32_t stackDepth = 10 * configMINIMAL_STACK_SIZE;
    uint32_t priority = configMAX_PRIORITIES - 3;

    xTaskCreate(Beeper::beepTask, "beepTask", stackDepth, &params, priority, NULL);
}


uint8_t Beeper::onRequestReceive(RequestCode code, uint8_t data)
{
    if (code == BEEP)
        this->beep(100, 3, 300, true);
    
    return NO_RESPONSE;
}



// PRIVATE

void Beeper::beepTask(void* params)
{
    auto lambda = static_cast<std::tuple<Beeper*, uint8_t, uint8_t, uint16_t, bool>*>(params);

    Beeper* beeper = std::get<0>(*lambda);
    uint8_t power = std::get<1>(*lambda);
    uint8_t repet = std::get<2>(*lambda);
    uint16_t demiPeriodMs = std::get<3>(*lambda);
    bool turnOff = std::get<4>(*lambda);

    if (xSemaphoreTake(beeper->mutex, portMAX_DELAY) == pdTRUE)
    {
        for (int i = 0; i < repet; i++)
        {
            #ifndef DEBUG_2
            analogWrite(beeper->pin, power);
            #endif
            vTaskDelay(demiPeriodMs / portTICK_PERIOD_MS);
            #ifndef DEBUG_2
            analogWrite(beeper->pin, 0);
            #endif
            vTaskDelay(demiPeriodMs / portTICK_PERIOD_MS);
        }
        // if (turnOff)
        //     this->on->setVal(false);

        xSemaphoreGive(beeper->mutex);
    }
    
    vTaskDelete(NULL);

    for(;;) 
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}

