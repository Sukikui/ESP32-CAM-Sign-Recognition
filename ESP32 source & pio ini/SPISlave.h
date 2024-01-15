#ifndef SPISLAVE_H
#define SPISLAVE_H

#include "driver/spi_slave.h"
#include <Arduino.h>
#include <vector>

#define DATA_LEN 4
#define NO_RESPONSE 0xFF


/**
 * SPI protocol
 * DATA_LEN = 3 + 1 : buffer shift to the right to avoid first null byte problem
 * ________________________________
 * |        |        |            |
 * |  CODE  |  DATA  |  CHECKSUM  |
 * |________|________|____________|
*/
enum RequestCode : uint8_t
{
    GET_LAST_SIGN = 0x01,
    TURN_LED = 0x02,
    TURN_FLASH = 0x03,
    BEEP = 0x04,
};

enum LedColor : uint8_t
{
    RAINBOW = 0x00,
    RED = 0x01,
    BLUE = 0x02,
};


/**
 * SPI Slave Listener
 * ------------------
 * Listen to SPI events
*/
class SPISlaveListener
{
    public:
        /**
         * @return data, return NO_RESPONSE if request is not concerned
        */
        virtual uint8_t onRequestReceive(RequestCode code, uint8_t data) = 0;
};


/**
 * HSPI Slave (ESP32 CAM)
 * ------------------------------
 * Here we have only 1 slave select pin
 * Using interruption mode
*/
class SPISlave
{
    private:
        std::vector<SPISlaveListener*> listeners;
        TaskHandle_t handle;

        spi_bus_config_t bus_cfg;
        spi_slave_interface_config_t slave_cfg;
        spi_slave_transaction_t trans;

        uint8_t rx_buffer[DATA_LEN];
        uint8_t tx_buffer[DATA_LEN];

        bool checkSum();
        static void sendResponseTask(void* params);

    public:
        SPISlave(uint8_t miso, uint8_t mosi, uint8_t sclk, uint8_t ss);
        void addListener(SPISlaveListener* listener);
        void startListening();
        void stopListening();
        void printBuffer();
};

#endif // SPISLAVE_H