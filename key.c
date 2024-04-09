#include "I2C_LCD.h"
#include "key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
	gpioInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);
	
	gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
	gpioInit.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);
}

uint32_t Key_Scan(void){
	
	uint32_t u32Ret = 0;
	
	/* Row 1 */
  GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  Delay_Ms(1);
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) {
		u32Ret = 'a';
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) {
		u32Ret = 3;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
		u32Ret = 2;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) {
		u32Ret = 1;
		Delay_Ms(500);
	}
	
	/* Row 2 */
  GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
  Delay_Ms(1);
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) {
		u32Ret = 'b';
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) {
		u32Ret = 6;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
		u32Ret = 5;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) {
		u32Ret = 4;
		Delay_Ms(500);
	}
	
	/* Row 3 */
  GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_15);
    Delay_Ms(1);
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) {
		u32Ret = 'c';
	  Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) {
		u32Ret = 9;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
		u32Ret = 8;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) {
		u32Ret = 7;
		Delay_Ms(500);
	}
	
	/* Row 4 */
  GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_12);
  Delay_Ms(1);
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) {
		u32Ret = 'A';
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) {
		u32Ret = 12;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
		u32Ret = 0;
		Delay_Ms(500);
	}
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) {
		u32Ret = 'a';
		Delay_Ms(500);
	}
	
	return u32Ret;
}