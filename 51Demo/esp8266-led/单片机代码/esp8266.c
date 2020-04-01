
#include <reg51.h>
#include "esp8266.h"
#include "delay.h"

uchar commandData[11] = {0}; // 数据命令返回结果为 +IPD,<client号>,<收到的字符长度>:收到的字符
uchar reviceDataCnt = 0;
uchar reciveData[1] = 0;

// 设置模块的工作的模式 1 station 2 ap
uchar code CWMODE1[] = "AT+CWMODE=1\r\n";
// 设置连接的路由器
uchar code CWJAP[] = "AT+CWJAP=\"L\",\"lww418777\"\r\n";
// 重启
uchar code RST[] = "AT+RST\r\n";
// 启动多连接
uchar code CIPMUX[] = "AT+CIPMUX=1\r\n";
// 开启服务器模式，端口号10087
uchar code CIPSERVER[] = "AT+CIPSERVER=1,10087\r\n";
// 链接局域网内的服务器
uchar code CIPSTART[] = "AT+CIPSTART=\"TCP\",\"192.168.9.106\",10086\r\n";
// 断开链接
uchar code CIPCLOSE[] = "AT+CIPCLOSE\r\n";

void uart_init()
{
    SCON = 0x50; //设置串口工作方式1
    TMOD = 0x20; //设置计数器工作方式2
    PCON = 0x80; //即SMOD=1，波特率不加倍
    TH1 = 0xF3;  //计数器初值，波特率是4800,晶振为12MHz
    TL1 = 0xF3;
    ES = 1;  //打开接收中断
    EA = 1;  //打开总中断
    TR1 = 1; //打开计数器
}

void ESP8266_init()
{
    delay_ms(10000);         // 延时
    uart_tx_string(CWMODE1); // 设置ESP8266工作在Station模式下
    delay_ms(10000);
    uart_tx_string(CWJAP); // 链接ap(路由器)
    delay_ms(30000);
    uart_tx_string(CIPMUX); // 允许多连接
    delay_ms(10000);
    uart_tx_string(CIPSERVER); // 建立服务器
    delay_ms(10000);
}

/*************************************************
函数：uart_tx_byte
功能：串口发送一个字节
出口：void
入口：一个字节
*************************************************/
void uart_tx_byte(uchar str)
{
    TI = 0;
    SBUF = str;
    while (!TI)
        ;
    TI = 0;
}
/*************************************************
函数：uart_tx_string
功能：串口发送一个字符串
出口：void
入口：字符串数组
*************************************************/
void uart_tx_string(uchar *str)
{
    while (*str != '\0')
    {
        uart_tx_byte(*str++);
    }
}

void tempContactCommand(uchar *a, uchar c, uchar *b, uchar *result)
{
    int i, j;
    for (i = 0; a[i] != '\0'; i++)
    {
        result[i] = a[i];
    }
    result[i++] = c;

    for (j = 0; b[j] != '\0'; j++)
    {
        result[i + j] = b[j];
    }
    result[i + j] = '\0';
}
void uartInterrupt(void) interrupt 4
{
    uchar result[50];
    if (RI)
    {
        uchar tempCharData = SBUF;
        RI = 0; //清除串口接收标志位
        commandData[reviceDataCnt] = tempCharData;
        if (commandData[0] == '+')
        {
            reviceDataCnt++;
        }
        if (reviceDataCnt >= 4)
        {
            if (commandData[1] != 'I' || commandData[2] != 'P' || commandData[3] != 'D')
            {
                reviceDataCnt = 0;
            }
        }
        if (reviceDataCnt >= 10)
        {
            if (commandData[7] == '1' && commandData[8] == ':')
            {
                reciveData[0] = commandData[9];
            }
        }
        if (reviceDataCnt >= 10)
        {
            reviceDataCnt = 0;
            // 收到命令后发送相应 OK
            tempContactCommand("AT+CIPSEND=", commandData[5], ",2\r\n", result);
            uart_tx_string(result);
            delay_ms(100);
            uart_tx_string("OK");
        }
    }
}