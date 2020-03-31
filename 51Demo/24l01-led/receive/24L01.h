#ifndef _24L01_DEF_
#define _24L01_DEF_
#include <reg51.h>

#ifndef uchar
#define uchar unsigned char
#endif

#define TX_ADR_WIDTH 5   // 5字节宽度的发送/接收地址
#define TX_PLOAD_WIDTH 1 // 数据通道有效数据宽度

sbit NOT_FIND = P0 ^ 0;

// Define interface to nRF24L01

// Define SPI pins
sbit CE = P1 ^ 2;   // Chip Enable pin signal (output)芯片使能引脚信号
sbit CSN = P1 ^ 3;  // Slave Select pin, (output to CSN, nRF24L01)从端选择引脚
sbit IRQ = P1 ^ 7;  // Interrupt signal, from nRF24L01 (input)中断信号
sbit MISO = P1 ^ 6; // Master In, Slave Out pin (input)主入从出
sbit MOSI = P1 ^ 5; // Serial Clock pin, (output)主出从入
sbit SCK = P1 ^ 4;  // Master Out, Slave In pin (output)时钟信号

// SPI(nRF24L01) commands
#define READ_REG 0x00    // Define read command to register
#define WRITE_REG 0x20   // Define write command to register
#define RD_RX_PLOAD 0x61 // Define RX payload register address
#define WR_TX_PLOAD 0xA0 // Define TX payload register address
#define FLUSH_TX 0xE1    // Define flush TX register command
#define FLUSH_RX 0xE2    // Define flush RX register command
#define REUSE_TX_PL 0xE3 // Define reuse TX payload register command
#define NOP 0xFF         // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses)
#define CONFIG 0x00      // 'Config' register address
#define EN_AA 0x01       // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR 0x02   // 'Enabled RX addresses' register address
#define SETUP_AW 0x03    // 'Setup address width' register address
#define SETUP_RETR 0x04  // 'Setup Auto. Retrans' register address
#define RF_CH 0x05       // 'RF channel' register address
#define RF_SETUP 0x06    // 'RF setup' register address
#define STATUS 0x07      // 'Status' register address
#define OBSERVE_TX 0x08  // 'Observe TX' register address
#define CD 0x09          // 'Carrier Detect' register address
#define RX_ADDR_P0 0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1 0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2 0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3 0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4 0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5 0x0F  // 'RX address pipe5' register address
#define TX_ADDR 0x10     // 'TX address' register address
#define RX_PW_P0 0x11    // 'RX payload width, pipe0' register address
#define RX_PW_P1 0x12    // 'RX payload width, pipe1' register address
#define RX_PW_P2 0x13    // 'RX payload width, pipe2' register address
#define RX_PW_P3 0x14    // 'RX payload width, pipe3' register address
#define RX_PW_P4 0x15    // 'RX payload width, pipe4' register address
#define RX_PW_P5 0x16    // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17 // 'FIFO Status Register' register address

void init_io();
void delay_ms(unsigned int y);
uchar SPI_RW(uchar byte);
uchar SPI_RW_Reg(uchar reg, uchar value);
uchar SPI_Read(uchar reg);
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar bytes);
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar bytes);
void RX_Mode(void);
void TX_Mode();
uchar Check_ACK(bit clear);
uchar NRF_Check(void);

#endif /* _API_DEF_ */