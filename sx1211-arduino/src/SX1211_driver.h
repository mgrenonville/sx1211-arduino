
#ifndef __SX1211_DRIVER_H__
#define __SX1211_DRIVER_H__

#include <Arduino.h>
#include <SPI.h>

#define SX1211_MODE_SLEEP 0
#define SX1211_MODE_STBY 1
#define SX1211_MODE_FS 2
#define SX1211_MODE_RX 3
#define SX1211_MODE_TX 4

class SX1211_Driver
{

public:
    void begin(SPIClass &spi, uint8_t NSS_CONFIG_PIN, uint8_t NSS_DATA_PIN, uint8_t IRQ_0_PIN, uint8_t IRQ_1_PIN);
    void writeRawConfig(byte address, byte value);

    byte *readConfig();
    void sendConfig();

    void setMode(uint8_t mode);

    void receivedData();

    bool hasAvailableData();

    void receive();
    void transmit();


private:
    static void SX_1211_IRQ0(void *p);
    static void SX_1211_IRQ1(void *p);
    volatile int crcOk;
    volatile int notFifoEmpty;

    void irq0();
    void irq1();
    SPIClass *spi;
    byte config[32];
    uint8_t NSS_CONFIG_PIN, NSS_DATA_PIN, IRQ_0_PIN, IRQ_1_PIN;
    SPISettings settings;
    void writeConfig(byte address, byte value);
    byte readWriteData(byte value);
    
};

#endif