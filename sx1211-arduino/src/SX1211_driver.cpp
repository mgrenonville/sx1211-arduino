
#ifndef __SX1211_DRIVER_
#define __SX1211_DRIVER_

#include "SX1211_driver.h"

// #include "esp32-hal-log.h"
#define ClkRegToFreq(reg) (apb_freq / (((reg)->clkdiv_pre + 1) * ((reg)->clkcnt_n + 1)))
typedef union {
    uint32_t value;
    struct {
            uint32_t clkcnt_l:       6;                     /*it must be equal to spi_clkcnt_N.*/
            uint32_t clkcnt_h:       6;                     /*it must be floor((spi_clkcnt_N+1)/2-1).*/
            uint32_t clkcnt_n:       6;                     /*it is the divider of spi_clk. So spi_clk frequency is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1)*/
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
            uint32_t clkdiv_pre:     4;                     /*it is pre-divider of spi_clk.*/
            uint32_t reserved:       9;  					/*reserved*/
#else
            uint32_t clkdiv_pre:    13;                     /*it is pre-divider of spi_clk.*/
#endif
            uint32_t clk_equ_sysclk: 1;                     /*1: spi_clk is eqaul to system 0: spi_clk is divided from system clock.*/
    };
} spiClk_t;




#define READ_WRITE_BYTE 1 << 6

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

void SX1211_Driver::begin(SPIClass *_spi, uint8_t NSS_CONFIG_PIN, uint8_t NSS_DATA_PIN, uint8_t IRQ_0_PIN, uint8_t IRQ_1_PIN)
{
    spi = _spi;

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

    attachInterruptArg(digitalPinToInterrupt(IRQ_1_PIN), SX_1211_IRQ1, this, CHANGE);
    this->crcOk = digitalRead(IRQ_1_PIN);

    // 1052673 / 2 = ~500khz CLK, looks like frisquet
    settings = SPISettings(1052673 / 2 / 2 / 2, MSBFIRST, SPI_MODE0);


#if defined(DEBUG_SX_1211)
    // const spiClk_t minFreqReg = {0x7FFFF000};

    // uint32_t apb_freq = getApbFrequency();
    // uint32_t minFreq = ClkRegToFreq((spiClk_t *)&minFreqReg);
    // Serial.printf("freq to div: %08x , minFreq:%08x\n", spiFrequencyToClockDiv(1 / 2), minFreqReg);

    Serial.printf("IRQ1 is interruptable ? %d", digitalPinToInterrupt(IRQ_1_PIN) != NOT_AN_INTERRUPT);
#endif
    readConfig();
};

void SX1211_Driver::setSyncWord(byte *syncword, byte syncword_size)
{
    for (byte i = 0; i < syncword_size; i++)
    {
        this->writeRawConfig(SX1211_REG_SYNCBYTE1 + i, syncword[i]);
    }
    byte rx3Value = SX_1211_RX3_SYNC_TOL_0 | SX_1211_RX3_POLYPFILT_OFF | SX_1211_RX3_SYNC_WORD_REC;
    Serial.printf("Updating syncword: size: %d", syncword_size);
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
    this->writeRawConfig(SX1211_REG_RXPARAM3, rx3Value);
};

void SX1211_Driver::setAddress(byte addr)
{
    this->writeRawConfig(SX1211_REG_NODEADRS, addr);
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

    this->spi->beginTransaction(settings);
    // Serial.printf("freq to div: %d - div: %d - freq: %d\n", spi->getClockDivider(), spi->_div, spi->_freq);
    digitalWrite(this->NSS_CONFIG_PIN, 0);
    int time = ESP.getCycleCount();
    spi->transfer(formatted);        // Enable READ mode at address
    byte read = spi->transfer(0x00); // Write nothing, but read
    int stop = ESP.getCycleCount();

#if defined(DEBUG_SX_1211)
    Serial.printf("Read: %02X (%02X) = %02X in %d cycle @ %d \n", address, formatted, read, (stop - time), ESP.getCpuFreqMHz());
#endif

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

#if defined(DEBUG_SX_1211)
    Serial.printf("Write Config: %02X (%02X) = %02X -> %02X \n", address, formatted, previousValue, value);
#else

    delay(5);
#endif
    digitalWrite(this->NSS_CONFIG_PIN, 1);

    delayMicroseconds(4);
};

void SX1211_Driver::setMode(uint8_t mode)
{
    byte current = config[0];
    current &= 0b00011111;
    current += mode << 5;
#if defined(DEBUG_SX_1211)
    Serial.printf("Set mode to %02X\n", mode);
#endif

    spi->beginTransaction(this->settings);
    writeConfig(0, current);
    spi->endTransaction();
};

bool SX1211_Driver::hasAvailableData()
{
#if defined(DEBUG_SX_1211)
    Serial.printf("IRQ status: 0: %d, 1: %d\n", notFifoEmpty, crcOk);
    Serial.printf("PIN status: 0: %d, 1: %d\n", digitalRead(IRQ_0_PIN), digitalRead(IRQ_1_PIN));
#endif

    return crcOk == 1;
};

byte SX1211_Driver::readWriteData(byte value)
{
    digitalWrite(this->NSS_CONFIG_PIN, 1);
    digitalWrite(this->NSS_DATA_PIN, 0);

    // delayMicroseconds(4000);
    byte received = spi->transfer(value);
    digitalWrite(this->NSS_DATA_PIN, 1);
    digitalWrite(this->NSS_CONFIG_PIN, 1);

    // delayMicroseconds(4000);
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

    setMode(SX1211_MODE_RX);

    byte rssi = readConfigByte(SX1211_REG_RSSIVALUE);
    byte crc_status = readConfigByte(SX1211_REG_PKTPARAM3);

    delay(1);
    set_fifo_stby_access(true);
    delay(1);
#if defined(DEBUG_SX_1211)
    Serial.printf("Received at (crc_ok: %d,crc_on: %d) %d dB: ", (crc_status & SX_1211_PKT3_CRC_STATUS) == SX_1211_PKT3_CRC_STATUS, (crc_status & SX_1211_PKT3_CRC_ON) == SX_1211_PKT3_CRC_ON, rssi);
#endif

    byte i = 0;
    while (notFifoEmpty)
    {

        spi->beginTransaction(settings);
        int data = readWriteData(0x00);

#if defined(DEBUG_SX_1211)
        Serial.printf("%02X ", data);
#endif
        received[i] = data;
        spi->endTransaction();
        i++;
    }
#if defined(DEBUG_SX_1211)
    Serial.printf("\n");
#endif

    return i - 1;
};

void SX1211_Driver::transmit(byte size, byte *payload)
{

    // prepare transmit
    setMode(SX1211_MODE_STBY);
    set_fifo_stby_access(false);
    spi->beginTransaction(settings);

#if defined(DEBUG_SX_1211)
    Serial.printf("Prepare transmit %d bytes to %02X\n", size, payload[0]);
#endif

    readWriteData(size);
    for (int i = 0; i < size; i++)
    {
        readWriteData(payload[i]);
#if defined(DEBUG_SX_1211)
        Serial.printf("%02X ", payload[i]);
#endif
    }

    spi->endTransaction();
    Serial.print("\n");
    setMode(SX1211_MODE_FS);
    setMode(SX1211_MODE_TX);
#if defined(DEBUG_SX_1211)
    Serial.printf("transmitting...  ");
#endif

    while (!crcOk)
    {
        yield(); // This will prevent wdt reset
    }
#if defined(DEBUG_SX_1211)

    Serial.printf("done \n");

#endif

    setMode(SX1211_MODE_STBY);
};

#endif