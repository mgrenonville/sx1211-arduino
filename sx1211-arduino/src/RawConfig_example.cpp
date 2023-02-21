#include "SX1211_driver.h"
#include <SPI.h>

SX1211_Driver sx1211;

SPIClass spi;

void showConfig()
{
    byte *configWritten = sx1211.readConfig();
    Serial.write("Config: ");
    for (byte address = 0; address < 32; address++)
    {
        Serial.printf("%02X", configWritten[address]);
    }
    Serial.write("\n");
}

void setup()
{
    Serial.begin(115200);
    delay(6000);
    boolean success = spi.pins(D5, D6, D7, 0);

    pinMode(D6, INPUT);
    pinMode(D7, OUTPUT);
    pinMode(D5, OUTPUT);
    spi.begin();
    sx1211.begin(spi, D1, D0, D3, D2);
    delay(1000);
    Serial.write("GO ! \n");
    Serial.printf("success: %2d\n", success);

    showConfig();

    Serial.write("Writing my config\n");
    sx1211.writeRawConfig(0x00 / 2, 0x30);
    sx1211.writeRawConfig(0x02 / 2, 0x8C);
    sx1211.writeRawConfig(0x04 / 2, 0x07);
    sx1211.writeRawConfig(0x06 / 2, 0x07);
    sx1211.writeRawConfig(0x08 / 2, 0x0C);
    sx1211.writeRawConfig(0x0A / 2, 0xCF);
    sx1211.writeRawConfig(0x0C / 2, 0x7F);
    sx1211.writeRawConfig(0x0E / 2, 0x65);
    sx1211.writeRawConfig(0x10 / 2, 0x4A);
    sx1211.writeRawConfig(0x12 / 2, 0x77);
    sx1211.writeRawConfig(0x14 / 2, 0x5F);
    sx1211.writeRawConfig(0x16 / 2, 0x19);
    sx1211.writeRawConfig(0x18 / 2, 0x38);
    sx1211.writeRawConfig(0x1A / 2, 0x88);
    sx1211.writeRawConfig(0x1C / 2, 0x19);
    sx1211.writeRawConfig(0x1E / 2, 0x82);
    sx1211.writeRawConfig(0x20 / 2, 0xE3);
    sx1211.writeRawConfig(0x22 / 2, 0x38);
    sx1211.writeRawConfig(0x24 / 2, 0x38);
    sx1211.writeRawConfig(0x26 / 2, 0x07);
    sx1211.writeRawConfig(0x2A / 2, 0x00);
    sx1211.writeRawConfig(0x2C / 2, 0x12);
    sx1211.writeRawConfig(0x2E / 2, 0x34);
    sx1211.writeRawConfig(0x30 / 2, 0x56);
    sx1211.writeRawConfig(0x32 / 2, 0x78);
    sx1211.writeRawConfig(0x34 / 2, 0x50);
    sx1211.writeRawConfig(0x36 / 2, 0x3C);
    sx1211.writeRawConfig(0x38 / 2, 0x40);
    sx1211.writeRawConfig(0x3A / 2, 0xFF);
    sx1211.writeRawConfig(0x3C / 2, 0xEC);
    sx1211.writeRawConfig(0x3E / 2, 0x00);
    sx1211.writeRawConfig(0x00 / 2, 0x10);

    sx1211.writeRawConfig(0x2C / 2, 0x05); // Satellite value.
    sx1211.writeRawConfig(0x2E / 2, 0xdb);
    sx1211.writeRawConfig(0x30 / 2, 0xe7);
    sx1211.writeRawConfig(0x32 / 2, 0x6b);
    sx1211.writeRawConfig(0x3A / 2, 0x01); // Address 0x01
    sx1211.writeRawConfig(0x00 / 2, 0x30);

    // disable address filtering.
    sx1211.writeRawConfig(30, 0xE8);

    // crc off
    // sx1211.writeRawConfig(30, 0xE0);

    sx1211.sendConfig();
    sx1211.setMode(SX1211_MODE_SLEEP);

    delay(1000);

    showConfig();
    delay(1000);

    showConfig();
}

void loop()
{

    sx1211.setMode(SX1211_MODE_FS);
    delay(1);

    sx1211.setMode(SX1211_MODE_RX);

    while (!sx1211.hasAvailableData())
    {
        delay(1000);
    }
    sx1211.receive();
    sx1211.setMode(SX1211_MODE_SLEEP);

    delay(100);
}
