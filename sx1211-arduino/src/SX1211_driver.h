
#ifndef __SX1211_DRIVER_H__
#define __SX1211_DRIVER_H__

#include <Arduino.h>
#include <SPI.h>

#define SX1211_MODE_SLEEP 0
#define SX1211_MODE_STBY 1
#define SX1211_MODE_FS 2
#define SX1211_MODE_RX 3
#define SX1211_MODE_TX 4

/*******************************************************************
** SX1211 definitions                                             **
*******************************************************************/

/*******************************************************************
** SX1211 Operating modes definition -- reg0 bits 7-5             **
*******************************************************************/
#define REG0_RF_SLEEP 0x00
#define REG0_RF_STANDBY 0x20
#define REG0_RF_SYNTHESIZER 0x40
#define REG0_RF_RECEIVER 0x60
#define REG0_RF_TRANSMITTER 0x80

/*******************************************************************
** SX1211 registers Address                              **
*******************************************************************/
#define SX1211_REG_MCPARAM1 0
#define SX1211_REG_MCPARAM2 1
#define SX1211_REG_FDEV 2
#define SX1211_REG_BITRATE 3
#define SX1211_REG_OOKFLOORTHRESH 4
#define SX1211_REG_MCPARAM6 5
#define SX1211_REG_R1 6
#define SX1211_REG_P1 7
#define SX1211_REG_S1 8
#define SX1211_REG_R2 9
#define SX1211_REG_P2 10
#define SX1211_REG_S2 11
#define SX1211_REG_PARAMP 12

#define SX1211_REG_IRQPARAM1 13
#define SX1211_REG_IRQPARAM2 14
#define SX1211_REG_RSSIIRQTHRESH 15

#define SX1211_REG_RXPARAM1 16
#define SX1211_REG_RXPARAM2 17
#define SX1211_REG_RXPARAM3 18
#define SX1211_REG_RES19 19
#define SX1211_REG_RSSIVALUE 20
#define SX1211_REG_RXPARAM6 21

#define SX1211_REG_SYNCBYTE1 22
#define SX1211_REG_SYNCBYTE2 23
#define SX1211_REG_SYNCBYTE3 24
#define SX1211_REG_SYNCBYTE4 25

#define SX1211_REG_TXPARAM 26

#define SX1211_REG_OSCPARAM 27

#define SX1211_REG_PKTPARAM1 28
#define SX1211_REG_NODEADRS 29
#define SX1211_REG_PKTPARAM3 30
#define SX1211_REG_PKTPARAM4 31

/*******************************************************************
** SX1211 initialisation register values definition               **
*******************************************************************/
#define DEF_MCPARAM1 0x00
#define DEF_MCPARAM2 0x00
#define DEF_FDEV 0x00
#define DEF_BITRATE 0x00
#define DEF_OOKFLOORTHRESH 0x00
#define DEF_MCPARAM6 0x00
#define DEF_R1 0x00
#define DEF_P1 0x00
#define DEF_S1 0x00
#define DEF_R2 0x00
#define DEF_P2 0x00
#define DEF_S2 0x00
#define DEF_PARAMP 0x20

#define DEF_IRQPARAM1 0x00
#define DEF_IRQPARAM2 0x08
#define DEF_RSSIIRQTHRESH 0x00

#define DEF_RXPARAM1 0x00
#define DEF_RXPARAM2 0x08
#define DEF_RXPARAM3 0x20
#define DEF_RES19 0x07
#define DEF_RSSIVALUE 0x00
#define DEF_RXPARAM6 0x00

#define DEF_SYNCBYTE1 0x00
#define DEF_SYNCBYTE2 0x00
#define DEF_SYNCBYTE3 0x00
#define DEF_SYNCBYTE4 0x00

#define DEF_TXPARAM 0x00

#define DEF_OSCPARAM 0x00

#define DEF_PKTPARAM1 0x00
#define DEF_NODEADRS 0x00
#define DEF_PKTPARAM3 0x00
#define DEF_PKTPARAM4 0x00

/*******************************************************************
** SX1211 bit control definition                                  **
*******************************************************************/
// MC Param 1 (reg0)
// Chip operating mode
#define SX_1211_MC1_SLEEP 0x00
#define SX_1211_MC1_STANDBY 0x20
#define SX_1211_MC1_SYNTHESIZER 0x40
#define SX_1211_MC1_RECEIVER 0x60
#define SX_1211_MC1_TRANSMITTER 0x80

// Frequency band (reg0)
#define SX_1211_MC1_BAND_915L 0x00
#define SX_1211_MC1_BAND_915H 0x08
#define SX_1211_MC1_BAND_868 0x10
#define SX_1211_MC1_BAND_950 0x10

// VCO trimming (reg0)
#define SX_1211_MC1_VCO_TRIM_00 0x00
#define SX_1211_MC1_VCO_TRIM_01 0x02
#define SX_1211_MC1_VCO_TRIM_10 0x04
#define SX_1211_MC1_VCO_TRIM_11 0x06

// RF frequency selection (reg0)
#define SX_1211_MC1_RPS_SELECT_1 0x00
#define SX_1211_MC1_RPS_SELECT_2 0x01

// MC Param 2
// Modulation scheme selection (reg1)
#define SX_1211_MC2_MODULATION_OOK 0x40
#define SX_1211_MC2_MODULATION_FSK 0x80

// Data operation mode (reg1)
#define SX_1211_MC2_DATA_MODE_CONTINUOUS 0x00
#define SX_1211_MC2_DATA_MODE_BUFFERED 0x20
#define SX_1211_MC2_DATA_MODE_PACKET 0x04

// Rx OOK threshold mode selection (reg1)
#define SX_1211_MC2_OOK_THRESH_TYPE_FIXED 0x00
#define SX_1211_MC2_OOK_THRESH_TYPE_PEAK 0x08
#define SX_1211_MC2_OOK_THRESH_TYPE_AVERAGE 0x10

// Gain on IF chain (reg1)
#define SX_1211_MC2_GAIN_IF_00 0x00
#define SX_1211_MC2_GAIN_IF_01 0x01
#define SX_1211_MC2_GAIN_IF_10 0x02
#define SX_1211_MC2_GAIN_IF_11 0x03

// Frequency deviation (kHz) (reg2)
#define SX_1211_FDEV_33 0x0B
#define SX_1211_FDEV_40 0x09
#define SX_1211_FDEV_50 0x07
#define SX_1211_FDEV_80 0x04  /* +/-92.16KHz with 14.7456MHz crystal  */
#define SX_1211_FDEV_100 0x03 /* +/-115.2KHz with 14.7456MHz crystal  */
#define SX_1211_FDEV_133 0x02
#define SX_1211_FDEV_200 0x01

// Bitrate (bit/sec)   (reg3)
#define SX_1211_BITRATE_1600 0x7C
#define SX_1211_BITRATE_2000 0x63
#define SX_1211_BITRATE_2500 0x4F
#define SX_1211_BITRATE_5000 0x27
#define SX_1211_BITRATE_8000 0x18
#define SX_1211_BITRATE_10000 0x13
#define SX_1211_BITRATE_20000 0x09
#define SX_1211_BITRATE_25000 0x07
#define SX_1211_BITRATE_40000 0x04
#define SX_1211_BITRATE_50000 0x03
#define SX_1211_BITRATE_100000 0x01

// OOK threshold (reg4)
#define SX_1211_OOKFLOORTHRESH_VALUE 0x0C

// MC Param 6
// FIFO size (reg5)
#define SX_1211_MC6_FIFO_SIZE_16 0x00
#define SX_1211_MC6_FIFO_SIZE_32 0x40
#define SX_1211_MC6_FIFO_SIZE_48 0x80
#define SX_1211_MC6_FIFO_SIZE_64 0xC0

// FIFO threshold (reg5)
#define SX_1211_MC6_FIFO_THRESH_VALUE 0x0F

// PA ramp times in OOK
#define SX_1211_PARAMP_00 0x00
#define SX_1211_PARAMP_01 0x08
#define SX_1211_PARAMP_10 0x10
#define SX_1211_PARAMP_11 0x18

// IRQ Param 1 (reg13)
// Select RX&STDBY IRQ_0 sources (Packet mode) (reg13)
#define SX_1211_IRQ0_RX_STDBY_PAYLOADREADY 0x00
#define SX_1211_IRQ0_RX_STDBY_WRITEBYTE 0x40
#define SX_1211_IRQ0_RX_STDBY_FIFOEMPTY 0x80
#define SX_1211_IRQ0_RX_STDBY_SYNCADRS 0xC0

// Select RX&STDBY IRQ_1 sources (Packet mode) (reg13)
#define SX_1211_IRQ1_RX_STDBY_CRCOK 0x00
#define SX_1211_IRQ1_RX_STDBY_FIFOFULL 0x10
#define SX_1211_IRQ1_RX_STDBY_RSSI 0x20
#define SX_1211_IRQ1_RX_STDBY_FIFOTHRESH 0x30

// Select TX IRQ_1 sources (Packet mode) (reg13)
#define SX_1211_IRQ1_TX_FIFOFULL 0x00
#define SX_1211_IRQ1_TX_TXDONE 0x08

// FIFO overrun/clear  (reg13)
#define SX_1211_IRQ1_FIFO_OVERRUN_CLEAR 0x01

// IRQ Param 2
// Select TX start condition and IRQ_0 source (Packet mode)
#define SX_1211_IRQ0_TX_FIFOTHRESH_START_FIFOTHRESH 0x00
#define SX_1211_IRQ0_TX_FIFOEMPTY_START_FIFONOTEMPTY 0x10

// RSSI IRQ flag
#define SX_1211_IRQ2_RSSI_IRQ_CLEAR 0x04

// PLL_Locked flag
#define SX_1211_IRQ2_PLL_LOCK_CLEAR 0x02

// PLL_Locked pin
#define SX_1211_IRQ2_PLL_LOCK_PIN_OFF 0x00
#define SX_1211_IRQ2_PLL_LOCK_PIN_ON 0x01

// RSSI threshold for interrupt
#define SX_1211_RSSIIRQTHRESH_VALUE 0x00

// RX Param 1
// Passive filter (kHz)
#define SX_1211_RX1_PASSIVEFILT_65 0x00
#define SX_1211_RX1_PASSIVEFILT_82 0x10
#define SX_1211_RX1_PASSIVEFILT_109 0x20
#define SX_1211_RX1_PASSIVEFILT_137 0x30
#define SX_1211_RX1_PASSIVEFILT_157 0x40
#define SX_1211_RX1_PASSIVEFILT_184 0x50
#define SX_1211_RX1_PASSIVEFILT_211 0x60
#define SX_1211_RX1_PASSIVEFILT_234 0x70
#define SX_1211_RX1_PASSIVEFILT_262 0x80
#define SX_1211_RX1_PASSIVEFILT_321 0x90
#define SX_1211_RX1_PASSIVEFILT_378 0xA0
#define SX_1211_RX1_PASSIVEFILT_414 0xB0
#define SX_1211_RX1_PASSIVEFILT_458 0xC0
#define SX_1211_RX1_PASSIVEFILT_514 0xD0
#define SX_1211_RX1_PASSIVEFILT_676 0xE0
#define SX_1211_RX1_PASSIVEFILT_987 0xF0

// Butterworth filter (kHz)
#define SX_1211_RX1_FC_VALUE 0x03
// !!! Values defined below only apply if RFCLKREF = DEFAULT VALUE = 0x07 !!!
#define SX_1211_RX1_FC_FOPLUS25 0x00
#define SX_1211_RX1_FC_FOPLUS50 0x01
#define SX_1211_RX1_FC_FOPLUS75 0x02
#define SX_1211_RX1_FC_FOPLUS100 0x03
#define SX_1211_RX1_FC_FOPLUS125 0x04
#define SX_1211_RX1_FC_FOPLUS150 0x05
#define SX_1211_RX1_FC_FOPLUS175 0x06
#define SX_1211_RX1_FC_FOPLUS200 0x07
#define SX_1211_RX1_FC_FOPLUS225 0x08
#define SX_1211_RX1_FC_FOPLUS250 0x09

// RX Param 2
// Polyphase filter center value (kHz)
#define SX_1211_RX2_FO_VALUE 0x03
// !!! Values defined below only apply if RFCLKREF = DEFAULT VALUE = 0x07 !!!
#define SX_1211_RX2_FO_50 0x10
#define SX_1211_RX2_FO_75 0x20
#define SX_1211_RX2_FO_100 0x30
#define SX_1211_RX2_FO_125 0x40
#define SX_1211_RX2_FO_150 0x50
#define SX_1211_RX2_FO_175 0x60
#define SX_1211_RX2_FO_200 0x70
#define SX_1211_RX2_FO_225 0x80
#define SX_1211_RX2_FO_250 0x90
#define SX_1211_RX2_FO_275 0xA0
#define SX_1211_RX2_FO_300 0xB0
#define SX_1211_RX2_FO_325 0xC0
#define SX_1211_RX2_FO_350 0xD0
#define SX_1211_RX2_FO_375 0xE0
#define SX_1211_RX2_FO_400 0xF0

// Rx Param 3
// Polyphase filter enable
#define SX_1211_RX3_POLYPFILT_ON 0x80
#define SX_1211_RX3_POLYPFILT_OFF 0x00

// Bit synchronizer
// Automatically activated in Packet mode

// Sync word recognition
// Automatically activated in Packet mode

// Size of the reference sync word
#define SX_1211_RX3_SYNC_SIZE_8 0x00
#define SX_1211_RX3_SYNC_SIZE_16 0x08
#define SX_1211_RX3_SYNC_SIZE_24 0x10
#define SX_1211_RX3_SYNC_SIZE_32 0x18

// Number of tolerated errors for the sync word recognition
#define SX_1211_RX3_SYNC_TOL_0 0x00
#define SX_1211_RX3_SYNC_TOL_1 0x02
#define SX_1211_RX3_SYNC_TOL_2 0x04
#define SX_1211_RX3_SYNC_TOL_3 0x06

// RSSI Value (Read only)

// Rx Param 6
// Decrement step of RSSI threshold in OOK demodulator (peak mode)
#define SX_1211_RX6_OOK_THRESH_DECSTEP_000 0x00
#define SX_1211_RX6_OOK_THRESH_DECSTEP_001 0x20
#define SX_1211_RX6_OOK_THRESH_DECSTEP_010 0x40
#define SX_1211_RX6_OOK_THRESH_DECSTEP_011 0x60
#define SX_1211_RX6_OOK_THRESH_DECSTEP_100 0x80
#define SX_1211_RX6_OOK_THRESH_DECSTEP_101 0xA0
#define SX_1211_RX6_OOK_THRESH_DECSTEP_110 0xC0
#define SX_1211_RX6_OOK_THRESH_DECSTEP_111 0xE0

// Decrement period of RSSI threshold in OOK demodulator (peak mode)
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_000 0x00
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_001 0x04
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_010 0x08
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_011 0x0C
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_100 0x10
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_101 0x14
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_110 0x18
#define SX_1211_RX6_OOK_THRESH_DECPERIOD_111 0x1C

// Cutoff freq of average function of RSSI threshold in OOK demodulator (average mode)
#define SX_1211_RX6_OOK_THRESH_AVERAGING_00 0x00
#define SX_1211_RX6_OOK_THRESH_AVERAGING_11 0x03

// TX Param
// Interpolator filter Tx (kHz)
#define SX_1211_TX_FC_VALUE 0x70
// !!! Values defined below only apply if RFCLKREF = DEFAULT VALUE = 0x07 !!!
#define SX_1211_TX_FC_25 0x00
#define SX_1211_TX_FC_50 0x10
#define SX_1211_TX_FC_75 0x20
#define SX_1211_TX_FC_100 0x30
#define SX_1211_TX_FC_125 0x40
#define SX_1211_TX_FC_150 0x50
#define SX_1211_TX_FC_175 0x60
#define SX_1211_TX_FC_200 0x70
#define SX_1211_TX_FC_225 0x80
#define SX_1211_TX_FC_250 0x90
#define SX_1211_TX_FC_275 0xA0
#define SX_1211_TX_FC_300 0xB0
#define SX_1211_TX_FC_325 0xC0
#define SX_1211_TX_FC_350 0xD0
#define SX_1211_TX_FC_375 0xE0
#define SX_1211_TX_FC_400 0xF0

// Tx Output power (dBm)
#define SX_1211_TX_POWER_MAX 0x00
#define SX_1211_TX_POWER_PLUS10 0x02
#define SX_1211_TX_POWER_PLUS7 0x04
#define SX_1211_TX_POWER_PLUS4 0x06
#define SX_1211_TX_POWER_PLUS1 0x08
#define SX_1211_TX_POWER_MINUS2 0x0a
#define SX_1211_TX_POWER_MINIMUM 0x0e

// OSC Param
// CLKOUT enable
#define SX_1211_OSC_CLKOUT_ON 0x80
#define SX_1211_OSC_CLKOUT_OFF 0x00

// CLKOUT frequency (kHz)
#define CLKOUT_DIVBY_1 (0 << 2)   // ie: 12.8MHz -> 12.8MHz
#define CLKOUT_DIVBY_2 (1 << 2)   // ie: 12.8MHz -> 6.4MHz
#define CLKOUT_DIVBY_4 (2 << 2)   // ie: 12.8MHz -> 3.2MHz
#define CLKOUT_DIVBY_6 (3 << 2)   // ie: 12.8MHz -> 2.13333MHz
#define CLKOUT_DIVBY_8 (4 << 2)   //
#define CLKOUT_DIVBY_10 (5 << 2)  //
#define CLKOUT_DIVBY_12 (6 << 2)  //
#define CLKOUT_DIVBY_14 (7 << 2)  //
#define CLKOUT_DIVBY_16 (8 << 2)  //
#define CLKOUT_DIVBY_18 (9 << 2)  //
#define CLKOUT_DIVBY_20 (10 << 2) //
#define CLKOUT_DIVBY_22 (11 << 2) //
#define CLKOUT_DIVBY_24 (12 << 2) //
#define CLKOUT_DIVBY_26 (13 << 2) //
#define CLKOUT_DIVBY_28 (14 << 2) //
#define CLKOUT_DIVBY_30 (15 << 2) //
#define CLKOUT_DIVBY_32 (16 << 2) //
#define CLKOUT_DIVBY_34 (17 << 2) //
#define CLKOUT_DIVBY_36 (18 << 2) //
#define CLKOUT_DIVBY_38 (19 << 2) //

#define CLKOUT_DISABLE 0xff // impossible value to signify no clkout desired
#if RF_CLKOUT_DIV == CLKOUT_DISABLE
#define SX_1211_OSC_CLKOUT_ENABLE RF_OSC_CLKOUT_OFF
#else
#define SX_1211_OSC_CLKOUT_ENABLE RF_OSC_CLKOUT_ON
#endif

// PKT Param 1
// Manchester enable
#define SX_1211_PKT1_MANCHESTER_ON 0x80
#define SX_1211_PKT1_MANCHESTER_OFF 0x00

// Payload length
#define SX_1211_PKT1_LENGTH_VALUE 0x00

// Node Address
#define SX_1211_NODEADRS_VALUE 0x00

// PKT Param 3
// Packet format
#define SX_1211_PKT3_FORMAT_FIXED 0x00
#define SX_1211_PKT3_FORMAT_VARIABLE 0x80

// Preamble size
#define SX_1211_PKT3_PREAMBLE_SIZE_8 0x00
#define SX_1211_PKT3_PREAMBLE_SIZE_16 0x20
#define SX_1211_PKT3_PREAMBLE_SIZE_24 0x40
#define SX_1211_PKT3_PREAMBLE_SIZE_32 0x60

// Whitening enable
#define SX_1211_PKT3_WHITENING_ON 0x10
#define SX_1211_PKT3_WHITENING_OFF 0x00

// CRC enable
#define SX_1211_PKT3_CRC_ON 0x08
#define SX_1211_PKT3_CRC_OFF 0x00

// Address filtering
#define SX_1211_PKT3_ADRSFILT_NONE 0x00             /* no filtering */
#define SX_1211_PKT3_ADRSFILT_ME_ONLY 0x02          /* only Node_adrs accepted (register 29) */
#define SX_1211_PKT3_ADRSFILT_ME_AND_00 0x04        /* Node_adrs and 0x00 accepted */
#define SX_1211_PKT3_ADRSFILT_ME_AND_00_AND_FF 0x06 /* Node_adrs and 0x00 and 0xff accepted */

// CRC status (Read only)
#define SX_1211_PKT3_CRC_STATUS 0x01

// PKT Param 4
// FIFO autoclear if CRC failed for current packet
#define SX_1211_PKT4_AUTOCLEAR_ON 0x00
#define SX_1211_PKT4_AUTOCLEAR_OFF 0x80

// Select FIFO access in standby mode (read or write)
#define SX_1211_PKT4_FIFO_STANDBY_WRITE 0x00
#define SX_1211_PKT4_FIFO_STANDBY_READ 0x40

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
    byte readConfigByte(byte address);

    byte readWriteData(byte value);

    void set_fifo_stby_access(bool value);
};

#endif