#include "SPISlave.h"
#define BREAK 100

// PUBLIC

SPISlave::SPISlave(uint8_t miso, uint8_t mosi, uint8_t sclk, uint8_t ss)
{
    // Configure SPI bus
    memset(&this->bus_cfg, 0, sizeof(spi_bus_config_t));
    this->bus_cfg.mosi_io_num = mosi;
    this->bus_cfg.miso_io_num = miso;
    this->bus_cfg.sclk_io_num = sclk;
    this->bus_cfg.quadwp_io_num = -1;
    this->bus_cfg.quadhd_io_num = -1;
    this->bus_cfg.flags = SPICOMMON_BUSFLAG_NATIVE_PINS;

    // Configure SPI device
    memset(&this->slave_cfg, 0, sizeof(spi_slave_interface_config_t));
    this->slave_cfg.mode = SPI_MODE0;
    this->slave_cfg.spics_io_num = ss;
    this->slave_cfg.queue_size = 1;                  
    spi_slave_initialize(HSPI_HOST, &this->bus_cfg, &this->slave_cfg, SPI_DMA_CH_AUTO);

    // Configure SPI transaction
    memset(&this->trans, 0, sizeof(spi_slave_transaction_t));
    //this->trans.flags = ;
    this->trans.length = DATA_LEN * 8;
    this->trans.tx_buffer = this->tx_buffer;
    this->trans.rx_buffer = this->rx_buffer;

    Serial.println("SPI Slave init finished.");
}


void SPISlave::addListener(SPISlaveListener* listener)
{
    this->listeners.push_back(listener);
    Serial.println("SPI Slave listener added.");
}


void SPISlave::startListening()
{
    std::tuple<SPISlave*> params = std::make_tuple(this);
    uint32_t stackDepth = 10 * configMINIMAL_STACK_SIZE;
    uint32_t priority = configMAX_PRIORITIES - 3;

    xTaskCreate(SPISlave::sendResponseTask, "sendResponseTask", stackDepth, &params, priority, &this->handle);
}


void SPISlave::stopListening()
{
    vTaskDelete(this->handle);
    spi_bus_free(HSPI_HOST);
}


// PRIVATE

void SPISlave::printBuffer()
{
    Serial.print("TX (Slave): ");

    for (uint8_t i = 1; i < DATA_LEN; i++)
    {
        Serial.print(this->tx_buffer[i], HEX);
        Serial.print(" ");
    }

    Serial.print("\tRX (Master): ");

    for (uint8_t i = 1; i < DATA_LEN; i++)
    {
        Serial.print(this->rx_buffer[i], HEX);
        Serial.print(" ");
    }

    Serial.println();
}


bool SPISlave::checkSum()
{
    uint8_t sum = 0;

    for (uint8_t i = 1; i < DATA_LEN; i++)
        sum += this->rx_buffer[i];

    return sum == 0x00;
}


// PRIVATE STATIC

void SPISlave::sendResponseTask(void* params)
{
    auto lambda = static_cast<std::tuple<SPISlave*>*>(params);
    SPISlave* slave = std::get<0>(*lambda);

    // Infinite loop mandatory for FreeRTOS thread
    for (;;)
    {
        spi_slave_transaction_t* t = &slave->trans;
        spi_slave_queue_trans(HSPI_HOST, t, portTICK_RATE_MS * 100);
        slave->printBuffer();
        
        if (slave->checkSum())
        {
            RequestCode rx_code = (RequestCode) slave->rx_buffer[1];
            uint8_t rx_data = slave->rx_buffer[2];
            uint8_t tx_data = 0;

            for (auto listener : slave->listeners)
            {
                tx_data = listener->onRequestReceive(rx_code, rx_data);

                // Return the first matched response
                if (tx_data != NO_RESPONSE)
                    break;
            }

            slave->tx_buffer[1] = (uint8_t) rx_code;
            slave->tx_buffer[2] = tx_data;
            uint8_t sum = (uint8_t) rx_code + tx_data;
            slave->tx_buffer[3] = - sum;
        }
        
        spi_slave_get_trans_result(HSPI_HOST, &t, portTICK_RATE_MS * 100);
        vTaskDelay(BREAK / portTICK_PERIOD_MS);
    }
}