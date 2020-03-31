#include <reg51.h>
#include "24L01.h"

sbit LED = P0 ^ 1; // led灯

uchar RX_BUF[TX_PLOAD_WIDTH];
uchar commandData[64] = {0};
uchar reviceDataCnt = 0;
uchar CWMODE1[] = "AT+CWMODE=1\r\n";

uchar bdata sta;
sbit RX_TD = sta ^ 6;

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

/**************************************************
函数：main()
描述：
    主函数
/**************************************************/
void main(void)
{

    uart_init();
    init_io();        // 初始化IO
    if (!NRF_Check()) // 如果检查不存在24l01芯片 led灯亮
    {
        NOT_FIND = 1;
    }
    RX_Mode(); // 设置为发送模式
    while (1)
    {
        sta = SPI_Read(STATUS); // 读状态寄存器
        if (RX_TD)              // 判断是否接受到数据 | if(sta^6)
        {
            SPI_Read_Buf(RD_RX_PLOAD, RX_BUF, TX_PLOAD_WIDTH); // 从RX FIFO读出数据
            LED = RX_BUF[0];
        }
        SPI_RW_Reg(FLUSH_RX, 0xff);          // 清楚接受缓冲器
        SPI_RW_Reg(WRITE_REG + STATUS, sta); // 清除RX_DS中断标志
        delay_ms(100);
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