#include "SX1211_driver.h"
#include <SPI.h>

SX1211_Driver sx1211;

SPIClass spi;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    boolean success = spi.pins(D5, D6, D7, 0);
    spi.begin();
    sx1211.begin(spi, D1, D0, D3, D2);
    delay(10000);
    Serial.write("GO ! \n");
    Serial.write(success);

    byte *config = sx1211.readConfig();
    Serial.write("Config: ");
    for (byte address = 0; address < 32; address++)
    {
        Serial.printf("%02X", config[address]);
    }
    Serial.write("\n");

    sx1211.writeRawConfig(0x00 / 2, 0x30);
    sx1211.writeRawConfig(0x02 / 2, 0x8c);
    sx1211.writeRawConfig(0x04 / 2, 0x07);
    sx1211.writeRawConfig(0x06 / 2, 0x07);
    sx1211.writeRawConfig(0x08 / 2, 0x0c);
    sx1211.writeRawConfig(0x0a / 2, 0xcf);
    sx1211.writeRawConfig(0x0c / 2, 0x7f);
    sx1211.writeRawConfig(0x0e / 2, 0x65);
    sx1211.writeRawConfig(0x10 / 2, 0x4a);
    sx1211.writeRawConfig(0x12 / 2, 0x77);
    sx1211.writeRawConfig(0x14 / 2, 0x5f);
    sx1211.writeRawConfig(0x16 / 2, 0x19);
    sx1211.writeRawConfig(0x18 / 2, 0x38);
    sx1211.writeRawConfig(0x1a / 2, 0x88);
    sx1211.writeRawConfig(0x1c / 2, 0x19);
    sx1211.writeRawConfig(0x1e / 2, 0x82);
    sx1211.writeRawConfig(0x20 / 2, 0xe3);
    sx1211.writeRawConfig(0x22 / 2, 0x38);
    sx1211.writeRawConfig(0x24 / 2, 0x38);
    sx1211.writeRawConfig(0x26 / 2, 0x07);
    sx1211.writeRawConfig(0x2a / 2, 0x00);
    sx1211.writeRawConfig(0x2c / 2, 0x12);
    sx1211.writeRawConfig(0x2e / 2, 0x34);
    sx1211.writeRawConfig(0x30 / 2, 0x56);
    sx1211.writeRawConfig(0x32 / 2, 0x78);
    sx1211.writeRawConfig(0x34 / 2, 0x50);
    sx1211.writeRawConfig(0x36 / 2, 0x3c);
    sx1211.writeRawConfig(0x38 / 2, 0x40);
    sx1211.writeRawConfig(0x3a / 2, 0xff);
    sx1211.writeRawConfig(0x3c / 2, 0xec);
    sx1211.writeRawConfig(0x3e / 2, 0x00);
    // disable address filtering.
    // sx1211.writeRawConfig(30, 0xE8);

    // crc off
    sx1211.writeRawConfig(30,  0xE0);

    sx1211.sendConfig();

    delay(10000);
}

void loop()
{

    sx1211.setMode(SX1211_MODE_RX);

    while (!sx1211.hasAvailableData())
    {
        delay(1000);
    }
    sx1211.receive();
}
