#ifndef __ESP8266_H_
#define __ESP8266_H_

#include <reg51.h>

#ifndef uchar
#define uchar unsigned char
#endif


extern uchar commandData[11];
extern uchar reviceDataCnt;
extern uchar reciveData[1];


void uart_init();
void ESP8266_init();
void uart_tx_byte(uchar str);
void uart_tx_string(uchar *str);

#endif