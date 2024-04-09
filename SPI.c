#include "SPI.h"

void SPI_Configuration(void)
{
	GPIO_InitTypeDef gpioInit;
	SPI_InitTypeDef  spiInit;
	
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	gpioInit.GPIO_Mode=GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed=GPIO_Speed_50MHz;
	gpioInit.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpioInit);
	
	spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiInit.SPI_Mode = SPI_Mode_Master;
	spiInit.SPI_DataSize = SPI_DataSize_8b;
	spiInit.SPI_CPOL = SPI_CPOL_Low;
	spiInit.SPI_CPHA = SPI_CPHA_1Edge;
	spiInit.SPI_NSS = SPI_NSS_Soft;
	spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
	spiInit.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &spiInit);
	
	SPI_Cmd(SPI1, ENABLE);
	
}
uint8_t SPI_Transfer_Data(uint8_t Data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1, Data);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
}
