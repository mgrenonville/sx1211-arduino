#include "SX1211_driver.h"
#include <SPI.h>

SX1211_Driver sx1211;
#define SCK D5
#define MISO D6
#define MOSI D7

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
    boolean success = spi.pins(SCK, MISO, MOSI, 0);

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
    // sx1211.writeRawConfig(SX1211_REG_MCPARAM1, 0x30);
    sx1211.writeRawConfig(SX1211_REG_MCPARAM1, SX_1211_MC1_BAND_868 | SX_1211_MC1_RPS_SELECT_1 | SX_1211_MC1_VCO_TRIM_00 | SX_1211_MC1_SLEEP);
    // sx1211.writeRawConfig(SX1211_REG_MCPARAM2, 0x8C);
    sx1211.writeRawConfig(SX1211_REG_MCPARAM2, SX_1211_MC2_MODULATION_FSK | SX_1211_MC2_DATA_MODE_PACKET | SX_1211_MC2_OOK_THRESH_TYPE_PEAK | SX_1211_MC2_GAIN_IF_00); // Mode packet

    // sx1211.writeRawConfig(SX1211_REG_FDEV, 0x07);
    sx1211.writeRawConfig(SX1211_REG_FDEV, SX_1211_FDEV_50);

    // sx1211.writeRawConfig(SX1211_REG_BITRATE, 0x07);
    sx1211.writeRawConfig(SX1211_REG_BITRATE, SX_1211_BITRATE_25000);

    // sx1211.writeRawConfig(SX1211_REG_OOKFLOORTHRESH, 0x0C);
    sx1211.writeRawConfig(SX1211_REG_OOKFLOORTHRESH, SX_1211_OOKFLOORTHRESH_VALUE);

    // sx1211.writeRawConfig(SX1211_REG_MCPARAM6, 0xCF);
    sx1211.writeRawConfig(SX1211_REG_MCPARAM6, SX_1211_MC6_FIFO_SIZE_64 | SX_1211_MC6_FIFO_THRESH_VALUE);

    // Configured values for my usecase
    sx1211.writeRawConfig(SX1211_REG_R1, 0x7F);
    sx1211.writeRawConfig(SX1211_REG_P1, 0x65);
    sx1211.writeRawConfig(SX1211_REG_S1, 0x4A);
    sx1211.writeRawConfig(SX1211_REG_R2, 0x77);
    sx1211.writeRawConfig(SX1211_REG_P2, 0x5F);
    sx1211.writeRawConfig(SX1211_REG_S2, 0x19);

    // For OOK, not usefull
    sx1211.writeRawConfig(SX1211_REG_PARAMP, 0x38);

    // sx1211.writeRawConfig(SX1211_REG_IRQPARAM1, 0x88);
    sx1211.writeRawConfig(SX1211_REG_IRQPARAM1, SX_1211_IRQ0_RX_STDBY_FIFOEMPTY | SX_1211_IRQ1_RX_STDBY_CRCOK | SX_1211_IRQ1_TX_TXDONE);

    sx1211.writeRawConfig(SX1211_REG_IRQPARAM2, 0x19);

    sx1211.writeRawConfig(SX1211_REG_RSSIIRQTHRESH, 0x82);
    sx1211.writeRawConfig(SX1211_REG_RXPARAM1, 0xE3);
    sx1211.writeRawConfig(SX1211_REG_RXPARAM2, 0x38);

    // sx1211.writeRawConfig(SX1211_REG_RXPARAM3, 0x38);
    byte network_id[] = {0x05, 0xda, 0x2e, 0xe2};
    sx1211.setSyncWord(network_id, 4);

    sx1211.writeRawConfig(SX1211_REG_TXPARAM, 0x50);
    sx1211.writeRawConfig(SX1211_REG_PKTPARAM1, 0x40);
    sx1211.writeRawConfig(SX1211_REG_OSCPARAM, 0x3c);


    sx1211.writeRawConfig(SX1211_REG_RES19, 0x07);
    sx1211.writeRawConfig(SX1211_REG_RXPARAM6, 0x00);

    sx1211.setAddress(0x08); // Address 0x80, chaudiere

    // disable address filtering.
    sx1211.writeRawConfig(SX1211_REG_PKTPARAM3, 0xE8);

    sx1211.sendConfig();
    sx1211.setMode(SX1211_MODE_SLEEP);

    delay(1000);

    showConfig();
    delay(1000);

    showConfig();
}


void transmit(byte *buf)
{
    sx1211.transmit(buf[0], buf + 1);
}


void loop()
{

//    // first.
    // byte buf[] = {0x3f, 0x80, 0x08, 0xbe, 0xe4, 0x7e, 0x17, 0xa1, 0x54, 0x00, 0x15, 0xa1, 0x54, 0x00, 0x18, 0x30, 0x96, 0x82, 0x00, 0x08, 0x10, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0x33, 0x66, 0x38, 0x30};
    byte buf[] = {0x17, 0x80, 0x08, 0x19, 0x7c, 0x01, 0x17, 0xa0, 0x29, 0x00, 0x15, 0xa0, 0x2f, 0x00, 0x04, 0x08, 0x00, 0xcc, 0x00, 0xaa, 0x00, 0x24, 0x00, 0xc6};
    // // byte buff[] = {00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E };

    transmit(buf);
    // byte network_id[] = {0xFF, 0xFF, 0xFF, 0xFF};
    // sx1211.setSyncWord(network_id, 4);
    // sx1211.sendConfig();

    // transmit(buf);

    // for (byte size = 63; size > 0; size--)
    // {
    //     // for (byte j = 0x55; j < 0xAA; j++)
    //     {
    //         // for (byte k = 0; k < size; k++)
    //         // {
    //         //     buff[k] = k;
    //         // }
    //         delay(1000);
    //         // sx1211.transmit(size, buff);
    //     }
    // }


    delay(10);

    sx1211.setMode(SX1211_MODE_FS);
    delay(1);

    sx1211.setMode(SX1211_MODE_RX);

    while (!sx1211.hasAvailableData())
    {
    delay(1000);
    }
    byte data[64];
    byte read = sx1211.receive(data);
    for (int i = 0; i <= read; i++)
    {
        Serial.printf("%02X ", data[i]);
    }
    // parse(data);
    sx1211.setMode(SX1211_MODE_SLEEP);
}
