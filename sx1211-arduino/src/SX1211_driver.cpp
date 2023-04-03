
#ifndef __SX1211_DRIVER_
#define __SX1211_DRIVER_

#include "SX1211_driver.h"

#define READ_WRITE_BYTE 1 << 6
#define LOG_DEBUG 1

void IRAM_ATTR SX1211_Driver::SX_1211_IRQ0(void *p)
{
    SX1211_Driver *ptr = (SX1211_Driver *)p;
    ptr->irq0();
};

void IRAM_ATTR SX1211_Driver::SX_1211_IRQ1(void *p)
{
    SX1211_Driver *ptr = (SX1211_Driver *)p;
    ptr->irq1();
};

ICACHE_RAM_ATTR void SX1211_Driver::irq0()
{
    this->notFifoEmpty = digitalRead(IRQ_0_PIN);
};

ICACHE_RAM_ATTR void SX1211_Driver::irq1()
{
    this->crcOk = digitalRead(IRQ_1_PIN);
};

void SX1211_Driver::begin(SPIClass &spi, uint8_t NSS_CONFIG_PIN, uint8_t NSS_DATA_PIN, uint8_t IRQ_0_PIN, uint8_t IRQ_1_PIN)
{
    spi = spi;
    this->NSS_CONFIG_PIN = NSS_CONFIG_PIN;
    this->NSS_DATA_PIN = NSS_DATA_PIN;
    this->IRQ_0_PIN = IRQ_0_PIN;
    this->IRQ_1_PIN = IRQ_1_PIN;

    pinMode(NSS_CONFIG_PIN, OUTPUT);
    digitalWrite(this->NSS_CONFIG_PIN, 1);

    pinMode(NSS_DATA_PIN, OUTPUT);
    digitalWrite(this->NSS_DATA_PIN, 1);

    // In my config, I have:
    //
    // Rx_stby_irq_0	IRQ_0 source in Rx and Standby modes: /Fifoempty
    // Rx_stby_irq_1	IRQ_1 source in Rx and Standby modes: CRC_ok
    // Tx_irq_1			IRQ_1 source in Tx mode: Tx_done

    pinMode(IRQ_0_PIN, INPUT);
    attachInterruptArg(digitalPinToInterrupt(IRQ_0_PIN), SX_1211_IRQ0, this, CHANGE);
    this->notFifoEmpty = digitalRead(IRQ_0_PIN);

    pinMode(IRQ_1_PIN, INPUT);
    if (LOG_DEBUG)
    {
        Serial.printf("IRQ1 is interruptable ? %d", digitalPinToInterrupt(IRQ_1_PIN) != NOT_AN_INTERRUPT);
    }
    attachInterruptArg(digitalPinToInterrupt(IRQ_1_PIN), SX_1211_IRQ1, this, CHANGE);
    this->crcOk = digitalRead(IRQ_1_PIN);
    // 1052673 / 2 = ~500khz CLK, looks like frisquet
    settings = SPISettings(1052673 / 2, MSBFIRST, SPI_MODE0);
};

void SX1211_Driver::setSyncWord(byte *syncword, byte syncword_size)
{
    for (byte i = 0; i < syncword_size; i++)
    {
        this->writeRawConfig(SX1211_REG_SYNCBYTE1 + i, syncword[i]);
    }
    byte rx3Value = SX_1211_RX3_SYNC_TOL_0 | SX_1211_RX3_POLYPFILT_OFF | SX_1211_RX3_SYNC_WORD_REC;
    switch (syncword_size)
    {
    case 1:
        rx3Value |= SX_1211_RX3_SYNC_SIZE_8;
        break;
    case 2:
        rx3Value |= SX_1211_RX3_SYNC_SIZE_16;
        break;
    case 3:
        rx3Value |= SX_1211_RX3_SYNC_SIZE_24;
        break;
    case 4:
        rx3Value |= SX_1211_RX3_SYNC_SIZE_32;
        break;
    }
    this->writeConfig(SX1211_REG_RXPARAM3, rx3Value);
};

void SX1211_Driver::setAddress(byte addr)
{
    this->writeConfig(SX1211_REG_NODEADRS, addr);
};

void SX1211_Driver::writeRawConfig(byte address, byte value)
{
    config[address] = value;
};
byte *SX1211_Driver::readConfig()
{

    for (byte address = 0; address < 32; address++)
    {
        config[address] = readConfigByte(address);
    }
    return this->config;
};

byte SX1211_Driver::readConfigByte(byte address)
{
    byte formatted = address << 1 ^ READ_WRITE_BYTE;

    spi->beginTransaction(this->settings);
    digitalWrite(this->NSS_CONFIG_PIN, 0);

    spi->transfer(formatted);        // Enable READ mode at address
    byte read = spi->transfer(0x00); // Write nothing, but read
    if (LOG_DEBUG)
    {
        Serial.printf("Read: %02X (%02X) = %02X \n", address, formatted, read);
    }
    digitalWrite(this->NSS_CONFIG_PIN, 1);

    spi->endTransaction();
    return read;
};
void SX1211_Driver::sendConfig()
{
    spi->beginTransaction(this->settings);
    for (byte address = 0; address < 32; address++)
    {
        writeConfig(address, config[address]);
    }
    spi->endTransaction();
};

void SX1211_Driver::writeConfig(byte address, byte value)
{
    byte formatted = address << 1;
    digitalWrite(this->NSS_CONFIG_PIN, 0);
    delayMicroseconds(4);

    spi->transfer(formatted); // Enable WRITE mode at address
    byte previousValue = spi->transfer(value);
    if (LOG_DEBUG)
    {
        Serial.printf("Write: %02X (%02X) = %02X -> %02X \n", address, formatted, previousValue, value);
    }
    digitalWrite(this->NSS_CONFIG_PIN, 1);
    delayMicroseconds(4);
};

void SX1211_Driver::setMode(uint8_t mode)
{
    byte current = config[0];
    current &= 0b00011111;
    current += mode << 5;
    if (LOG_DEBUG)
    {
        Serial.printf("Set mode to %02X", mode);
    }

    spi->beginTransaction(this->settings);
    writeConfig(0, current);
    spi->endTransaction();
};

bool SX1211_Driver::hasAvailableData()
{
    // if(LOG_DEBUG)
    {
        Serial.printf("IRQ status: 0: %d, 1: %d\n", notFifoEmpty, crcOk);
    }
    return crcOk == 1;
};

byte SX1211_Driver::readWriteData(byte value)
{
    digitalWrite(this->NSS_CONFIG_PIN, 1);
    digitalWrite(this->NSS_DATA_PIN, 0);
    byte received = spi->transfer(value);
    digitalWrite(this->NSS_DATA_PIN, 1);
    digitalWrite(this->NSS_CONFIG_PIN, 1);

    return received;
};

void SX1211_Driver::set_fifo_stby_access(bool value)
{
    spi->beginTransaction(this->settings);
    if (value)
    {
        config[SX1211_REG_PKTPARAM4] |= value << 6;
    }
    else
    {
        config[SX1211_REG_PKTPARAM4] &= 0b10111111;
    }

    writeConfig(SX1211_REG_PKTPARAM4, config[31]);
    spi->endTransaction();
};

byte SX1211_Driver::receive(byte *received)
{

    setMode(SX1211_MODE_STBY);

    byte rssi = readConfigByte(SX1211_REG_RSSIVALUE);
    byte crc_status = readConfigByte(SX1211_REG_PKTPARAM3);

    delay(1);
    set_fifo_stby_access(true);
    delay(1);
    if (LOG_DEBUG)
    {
        Serial.printf("Received at (crc_ok: %d,crc_on: %d) %d dB: ", (crc_status & SX_1211_PKT3_CRC_STATUS) == SX_1211_PKT3_CRC_STATUS, (crc_status & SX_1211_PKT3_CRC_ON) == SX_1211_PKT3_CRC_ON, rssi);
    }
    byte i = 0;
    while (notFifoEmpty)
    {

        spi->beginTransaction(settings);
        int data = readWriteData(0x00);

        if (LOG_DEBUG)
        {
            Serial.printf("%02X ", data);
            received[i] = data;
        }
        spi->endTransaction();
        i++;
    }
    if (LOG_DEBUG)
    {
        Serial.printf("\n");
    }
    return i - 1;
};


void SX1211_Driver::transmit(byte size, byte *payload)
{
    // prepare transmit
    setMode(SX1211_MODE_STBY);
    set_fifo_stby_access(false);

    spi->beginTransaction(settings);
    // if (LOG_DEBUG)
    {
        Serial.printf("Prepare transmit %d bytes to %02X\n", size, payload[0]);
    }
    readWriteData(size);
    for (int i = 0; i < size; i++)
    {
        readWriteData(payload[i]);
        // if (LOG_DEBUG)
        {
            Serial.printf("%02X ", payload[i]);
        }
    }

    spi->endTransaction();
    delay(40);
    setMode(SX1211_MODE_FS);
    delay(10);
    setMode(SX1211_MODE_TX);
    // if (LOG_DEBUG)
    {
        Serial.printf("transmitting");
    }
    while (!crcOk)
    {
        delay(10);
    }
    // if (LOG_DEBUG)
    {
        Serial.printf("transmittion done \n");
    }
    setMode(SX1211_MODE_STBY);
};

#endif