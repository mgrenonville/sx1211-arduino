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
    sx1211.setSyncWord({0x05, 0xda, 0x2e, 0xe2}, 4);

    sx1211.writeRawConfig(SX1211_REG_RES19, 0x07);
    sx1211.writeRawConfig(SX1211_REG_RXPARAM6, 0x00);

    sx1211.setAddress(0x3A / 2, 0x08); // Address 0x80, chaudiere

    // disable address filtering.
    sx1211.writeRawConfig(SX1211_REG_PKTPARAM3, 0xE8);

    sx1211.sendConfig();
    sx1211.setMode(SX1211_MODE_SLEEP);

    delay(1000);

    showConfig();
    delay(1000);

    showConfig();
}

void parse(byte *data)
{
    // SIZE:11 TO:80 FROM:08 19 TRAME?:00 01 17 a0 29 00 15 a0 VAR:2f 00 01 02 00 192

    byte size = data[0];
    byte to = data[1];
    byte from = data[2];

    if (from == 0x09) // Z2
    {
        byte buffer[] = {128, data[3], 0, 0x81, data[6], 42, 5, 10, 0, 0, 35, 2, 35, 33, 65, 19, 8, 4, 0, 192, 0, 110, 0, 37, 1, 59, 1, 59, 4, 246, 0, 0, 0, 0, 0, 0, 0, 0, 4, 246, 0, 0, 0, 0, 0, 0, 0, 0};
        sx1211.transmit(49 - 1, 9, buffer);
    }
}

void transmit(byte *buf)
{
    sx1211.transmit(buf[0] - 1, buf[1], buf + 2);
}

void loop()
{

    // first.
    byte buf[] = {0x3f, 0x80, 0x08, 0xbe, 0xe4, 0x7e, 0x17, 0xa1, 0x54, 0x00, 0x15, 0xa1, 0x54, 0x00, 0x18, 0x30, 0x96, 0x82, 0x00, 0x08, 0x10, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc0, 0x33, 0x66, 0x38, 0x30};

    transmit(buf);
    delay(100);
    byte reponse[] = {0X3f, 0X7e, 0X80, 0X6e, 0Xe4, 0X08, 0X17, 0Xa1, 0X54, 0X00, 0X15, 0Xa1, 0X54, 0X00, 0X18, 0X30, 0X96, 0X82, 0X00, 0X08, 0X10, 0X00, 0X00, 0Xc0, 0Xff, 0Xff, 0Xff, 0X0f, 0X00, 0Xc0, 0Xff, 0Xff, 0Xff, 0X0f, 0X00, 0Xc0, 0Xff, 0Xff, 0Xff, 0X0f, 0X00, 0Xc0, 0Xff, 0Xff, 0Xff, 0X0f, 0X00, 0Xc0, 0Xff, 0Xff, 0Xff, 0X0f, 0X00, 0Xc0, 0Xff, 0Xff, 0Xff, 0X0f, 0X00, 0Xc0, 0X33, 0X66, 0X37, 0X65};

    transmit(reponse);
    delay(100);

    byte trois[] = {0X31, 0X80, 0X7e, 0X6e, 0Xe4, 0X88, 0X17, 0X2a, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

    transmit(trois);
    delay(100);
    byte quatre[] = {0X31, 0X08, 0X80, 0Xbe, 0Xe4, 0Xfe, 0X17, 0X2a, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

    transmit(quatre);
    delay(10000);
    // sx1211.setMode(SX1211_MODE_FS);
    // delay(1);

    // sx1211.setMode(SX1211_MODE_RX);

    // while (!sx1211.hasAvailableData())
    // {
    // delay(100000);
    // }
    // byte data[64];
    // byte read = sx1211.receive(data);
    // for (int i = 0; i <= read; i++)
    // {
    //     Serial.printf("%02X ", data[i]);
    // }
    // parse(data);
    sx1211.setMode(SX1211_MODE_SLEEP);
}
