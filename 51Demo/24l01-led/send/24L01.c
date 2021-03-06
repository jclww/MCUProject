/**************************************************
主函数
/**************************************************/
#include <reg51.h>
#include "24L01.h"

uchar code TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01}; // 定义一个静态发送地址

/**************************************************/

/**************************************************/

/**************************************************
函数：delay_ms()
描述：
    延迟x毫秒
/**************************************************/
void delay_ms(unsigned int y)
{
	uchar x;
	for (; y > 0; y--)
	{
		for (x = 110; x > 0; x--)
			;
	}
}
/**************************************************/

/**************************************************
函数: init_io()
描述:
    初始化IO
/**************************************************/
void init_io(void)
{
	NOT_FIND = 0;
	delay_ms(200); // 等待200ms
	CE = 0;		   // 待机
	CSN = 1;	   // SPI禁止
	SCK = 0;	   // SPI时钟置低
}

/**************************************************
函数：SPI_RW()
描述：
    根据SPI协议，写一字节数据到nRF24L01，同时从nRF24L01
	读出一字节
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar i;
	for (i = 0; i < 8; i++) // 循环8次
	{
		MOSI = (byte & 0x80); // byte最高位输出到MOSI
		byte <<= 1;			  // 低一位移位到最高位
		SCK = 1;			  // 拉高SCK，nRF24L01从MOSI读入1位数据，同时从MISO输出1位数据
		byte |= MISO;		  // 读MISO到byte最低位
		SCK = 0;			  // SCK置低
	}
	return (byte); // 返回读出的一字节
}
/**************************************************/

/**************************************************
函数：SPI_RW_Reg()
描述：
    写数据value到reg寄存器
/**************************************************/
uchar SPI_RW_Reg(uchar reg, uchar value)
{
	uchar status;
	CSN = 0;			  // CSN置低，开始传输数据
	status = SPI_RW(reg); // 选择寄存器，同时返回状态字
	SPI_RW(value);		  // 然后写数据到该寄存器
	CSN = 1;			  // CSN拉高，结束数据传输
	return (status);	  // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：SPI_Read()
描述：
    从reg寄存器读一字节
/**************************************************/
uchar SPI_Read(uchar reg)
{
	uchar reg_val;
	CSN = 0;			 // CSN置低，开始传输数据
	SPI_RW(reg);		 // 选择寄存器
	reg_val = SPI_RW(0); // 然后从该寄存器读数据
	CSN = 1;			 // CSN拉高，结束数据传输
	return (reg_val);	// 返回寄存器数据
}
/**************************************************/

/**************************************************
函数：SPI_Read_Buf()
描述：
    从reg寄存器读出bytes个字节，通常用来读取接收通道
	数据或接收/发送地址
/**************************************************/
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar bytes)
{
	uchar status, i;
	CSN = 0;			  // CSN置低，开始传输数据
	status = SPI_RW(reg); // 选择寄存器，同时返回状态字
	for (i = 0; i < bytes; i++)
		pBuf[i] = SPI_RW(0); // 逐个字节从nRF24L01读出
	CSN = 1;				 // CSN拉高，结束数据传输
	return (status);		 // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：SPI_Write_Buf()
描述：
    把pBuf缓存中的数据写入到nRF24L01，通常用来写入发
	射通道数据或接收/发送地址
/**************************************************/
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar bytes)
{
	uchar status, i;
	CSN = 0;			  // CSN置低，开始传输数据
	status = SPI_RW(reg); // 选择寄存器，同时返回状态字
	for (i = 0; i < bytes; i++)
		SPI_RW(pBuf[i]); // 逐个字节写入nRF24L01
	CSN = 1;			 // CSN拉高，结束数据传输
	return (status);	 // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：RX_Mode()
描述：
    这个函数设置nRF24L01为接收模式，等待接收发送设备的数据包
/**************************************************/
void RX_Mode(void)
{
	CE = 0;
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 接收设备接收通道0使用和发送设备相同的发送地址
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);							 // 使能接收通道0自动应答
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);						 // 使能接收通道0
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);								 // 选择射频通道0x40
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);				 // 接收通道0选择和发送通道相同有效数据宽度
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);							 // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);							 // CRC使能，16位CRC校验，上电，接收模式
	CE = 1;															 // 拉高CE启动接收设备
}
/**************************************************/

/**************************************************
函数：TX_Mode()
描述：
    这个函数设置nRF24L01为发送模式，（CE=1持续至少10us），
	130us后启动发射，数据发送结束后，发送模块自动转入接收
	模式等待应答信号。
/**************************************************/
void TX_Mode()
{
	CE = 0;
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);	// 写入发送地址
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 为了应答接收设备，接收通道0地址和发送地址相同
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);							 // 使能接收通道0自动应答
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);						 // 使能接收通道0
	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x0a);						 // 自动重发延时等待250us+86us，自动重发10次
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);								 // 选择射频通道0x40
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);				 // 接收通道0选择和发送通道相同有效数据宽度
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);							 // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);							 // CRC使能，16位CRC校验，上电
	CE = 1;
}
/**************************************************/

/**************************************************
函数：Check_ACK()
描述：
    检查接收设备有无接收到数据包，设定没有收到应答信
	号是否重发
/**************************************************/
uchar Check_ACK(bit clear)
{
	uchar bdata sta;
	while (IRQ)
		;
	sta = SPI_RW(NOP); // 返回状态寄存器
	if (sta ^ 4)
		if (clear) // 是否清除TX FIFO，没有清除在复位MAX_RT中断标志后重发
			SPI_RW(FLUSH_TX);
	SPI_RW_Reg(WRITE_REG + STATUS, sta); // 清除TX_DS或MAX_RT中断标志
	IRQ = 1;
	if (sta ^ 5)
		return (0x00);
	else
		return (0xff);
}
/**************************************************/

uchar NRF_Check(void)
{
	uchar buf[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	uchar buf1[5];
	uchar i;

	SPI_Write_Buf(WRITE_REG + TX_ADDR, buf, 5); /*写入5个字节的地址.  */
	SPI_Read_Buf(TX_ADDR, buf1, 5);				/*读出写入的地址 */
	for (i = 0; i < 5; i++)						/*比较*/
	{
		if (buf1[i] != 0xC2)
			break;
	}
	if (i == 5)
		return 1; //MCU与NRF成功连接
	else
		return 0; //MCU与NRF不正常连接
}