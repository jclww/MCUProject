#include<reg51.h>
#include"esp8266.h"
#include "delay.h"

sbit led = P0^1;

void main()
{
    uchar i;
    uart_init();
    ESP8266_init();
    while (1)
    {
		led = reciveData[0];
        delay_ms(2000);
    }
}