#include <reg51.h>
#include "24L01.h"

sbit k1 = P1 ^ 0;  // 按键控制
sbit LED = P0 ^ 1; // led灯

uchar tx_buffer[TX_PLOAD_WIDTH] = {0};

uchar commandData[64] = {0};
uchar reviceDataCnt = 0;

void uart_init()
{
    SCON = 0x50; //设置串口工作方式1
    TMOD = 0x20; //设置计数器工作方式2
    PCON = 0x80; //即SMOD=1，波特率不加倍
    TH1 = 0xF3;  //计数器初值，波特率是9600,晶振为12MHz
    TL1 = 0xF3;
    ES = 1;  //打开接收中断
    EA = 1;  //打开总中断
    TR1 = 1; //打开计数器
}

void uart_tx_byte(uchar str)
{
    TI = 0;
    SBUF = str;
    while (!TI)
        ;
    TI = 0;
}
void uart_tx_string(uchar *str)
{
    while (*str != '\0')
    {
        uart_tx_byte(*str++);
    }
}

uchar keypros()
{
    if (k1 == 0)
    {
        delay_ms(10);
        if (k1 == 0)
        {
            LED = ~LED;
            return 1;
        }
    }
    return 0;
}

/**************************************************
函数：main()
描述：
    主函数
/**************************************************/
void main(void)
{

    uchar bdata sta;
    init_io();
    if (!NRF_Check())
    {
        NOT_FIND = 1;
    }
    TX_Mode();

    uart_init();
    while (1)
    {
        if (keypros())
        {
            tx_buffer[0] = LED;
            CE = 0;
            SPI_Write_Buf(WR_TX_PLOAD, tx_buffer, TX_PLOAD_WIDTH);
            CE = 1;
            while (IRQ)
                ;
            sta = SPI_Read(STATUS);
            SPI_RW_Reg(FLUSH_TX, 0xff);
            SPI_RW_Reg(WRITE_REG + STATUS, sta);
            delay_ms(1000);
        }
    }
}

void uartInterrupt(void) interrupt 4
{
    if (RI)
    {
        uchar tempCharData = SBUF;
        uart_tx_byte(tempCharData);
        RI = 0;
    }
}