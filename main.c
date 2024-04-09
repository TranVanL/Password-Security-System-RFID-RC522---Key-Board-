#include "stm32f10x.h"                
#include "I2C_LCD.h"
#include "key.h"
#include "SPI.h"
#include "RC522.h"

#include <stdio.h>
#include <stdint.h>


void Lock_Disable(uint8_t Time_Lock);
void CorrectPassWord(void);
void InCorrectPassWord(void);
void EnterPassWord(void);
void I2C_LCD_Configuration(void);
void PWM_Configuration(void);
void OpenDoor(void);
void CloseDoor(void);
void Notify_Horn(uint8_t u8Notify_Time);
uint8_t Check_SDCard(uint8_t *String);
void Correct_Card(void);
void Incorrect_Card(void);


static uint8_t CardID[16];
static uint8_t PassWord[];
static uint8_t DisplayKey[18];
static uint8_t u8Key;
static uint8_t u8PositionPass = 0;
static uint8_t u8Status;
static uint8_t Key_Card[5] = {0xc4, 0x24, 0x99, 0x22, 0x5b};
	

typedef struct
{
   uint8_t u8CheckPass;
	 uint8_t u8CheckLength;
   uint8_t u8Incorrect;
	 uint8_t u8NumberOfIncorrect;
} CheckType;
static CheckType SetupCheck;


void Lock_Disable(uint8_t Time_Lock)
{
	for ( uint8_t i = Time_Lock ; i > 0 ; i--){
    HD44780_SetCursor(0,0);
	  HD44780_PrintStr("Disable the Lock ");
	  HD44780_SetCursor(0,1);
		sprintf(DisplayKey, "Wait %d second...", i);
	  HD44780_PrintStr(DisplayKey);
	  Delay_Ms(980);
	  HD44780_Clear();
	}

}
void CorrectPassWord(void)
{
	HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("PassWord correct");
	HD44780_SetCursor(0,1);
	HD44780_PrintStr("Congratulation");
	HD44780_PrintSpecialChar(3);
	HD44780_PrintSpecialChar(3);
	SetupCheck.u8Incorrect++;
	/* Motor rotates 90 degrees */
  OpenDoor();
	Notify_Horn(200);
	Delay_Ms(100);
	Notify_Horn(200);
	Delay_Ms(5000);
	CloseDoor();
	HD44780_Clear();
}

void InCorrectPassWord(void)
{
	HD44780_Clear();
  HD44780_SetCursor(0,0);
	HD44780_PrintStr("PassWord Incorrect");
	HD44780_SetCursor(0,1);
	HD44780_PrintStr("Enter Password ");
	Notify_Horn(4000);
	Delay_Ms(2000);
	HD44780_Clear();
	SetupCheck.u8CheckPass = 0;
	SetupCheck.u8CheckLength = 1;
	u8PositionPass = 0;
 
}
void EnterPassWord(void)
{
	HD44780_SetCursor(0,0);
  HD44780_PrintStr("Enter password");
	HD44780_SetCursor(u8PositionPass,1);
	HD44780_PrintSpecialChar(2);
	u8PositionPass++;
	if (u8Key != SetupCheck.u8CheckLength) SetupCheck.u8CheckPass++;
		 SetupCheck.u8CheckLength++;
  
}

void I2C_LCD_Configuration(void)
{
	  GPIO_InitTypeDef  gpioInit;
    I2C_InitTypeDef  i2cInit;
		
	  /* PortB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	  /* I2C1 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    GPIO_StructInit(&gpioInit);
	
    /* I2C1 SDA and SCL configuration */
    gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &gpioInit);

		/* Configure I2Cx */
    I2C_StructInit(&i2cInit);
    i2cInit.I2C_Mode = I2C_Mode_I2C;
    i2cInit.I2C_DutyCycle = I2C_DutyCycle_2;
    i2cInit.I2C_OwnAddress1 = 0x00;
    i2cInit.I2C_Ack = I2C_Ack_Enable;
    i2cInit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2cInit.I2C_ClockSpeed = 100000;

    I2C_Init(I2C_Chanel, &i2cInit);
    I2C_Cmd(I2C_Chanel, ENABLE); 
}

void PWM_Configuration(void)
{ 
	/*Configure Timer 2 */
	TIM_TimeBaseInitTypeDef timerInit;
	GPIO_InitTypeDef gpioInit;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Pin = GPIO_Pin_0;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);
	
	timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	timerInit.TIM_Period = 10000 - 1; /* 20ms */
	timerInit.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInit(TIM2, &timerInit);
	
}

void OpenDoor(void){
	
	TIM_OCInitTypeDef pwmInit;
	/* CH1 */
	/* 1.5ms */
	pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;
	pwmInit.TIM_Pulse = 1000;
	pwmInit.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM2, &pwmInit);
	
	TIM_Cmd(TIM2, ENABLE);

}
void CloseDoor(void){
	TIM_OCInitTypeDef pwmInit;
	/* CH1 */
	/* 1.5ms */
	pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;
	pwmInit.TIM_Pulse = 500;
	pwmInit.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM2, &pwmInit);
	
	TIM_Cmd(TIM2, ENABLE);

	
}

void Notify_Horn(uint8_t u8Notify_Time)
{
	GPIO_InitTypeDef  gpioInit;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
  gpioInit.GPIO_Pin = GPIO_Pin_14;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
  gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpioInit);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_14);
	Delay_Ms(u8Notify_Time);
	GPIO_ResetBits(GPIOC, GPIO_Pin_14);
	
}

uint8_t Check_SDCard(uint8_t *Str)
{ 
	uint8_t u8CheckCard = 0;
	for (uint8_t i = 0 ; i < 5 ; i++){
		if (Key_Card[i] != *Str) u8CheckCard++;
		Str++;
	}
	return u8CheckCard;
	
}

void Correct_Card(void)	
{
	HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("SD Card correct");
	HD44780_SetCursor(0,1);
	HD44780_PrintStr("Congratulation");
	HD44780_PrintSpecialChar(3);
	HD44780_PrintSpecialChar(3);
	/* Motor rotates 90 degrees */
  OpenDoor();
	Notify_Horn(200);
	Delay_Ms(100);
	Notify_Horn(200);
	Delay_Ms(5000);
	CloseDoor();
	HD44780_Clear();
	
}

void Incorrect_Card(void)
{
	HD44780_Clear();
  HD44780_SetCursor(0,0);
	HD44780_PrintStr("SDCard Incorrect");
	Notify_Horn(1000);
	Delay_Ms(2000);
	HD44780_Clear();

}
int main(void){
	
  /* Function Init */
	I2C_LCD_Configuration();
	PWM_Configuration();
	Key_Init();
	MFRC522_Init();

	
	/*Hello World */
	HD44780_Init(2);
  HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("HELLO  WORLD");
  HD44780_SetCursor(0,1);
  HD44780_PrintStr("TVL Stm32f103c8");
  Delay_Ms(4500);
	HD44780_Clear();
	
	/*Default Setting */
  SetupCheck.u8CheckPass = 0;
	SetupCheck.u8CheckLength = 1;
	SetupCheck.u8Incorrect = 0;
  SetupCheck.u8NumberOfIncorrect = 0;
	
  while (1) {
		
		
		/* Check SD Card */
		u8Status = MFRC522_Request(PICC_REQIDL, CardID);
		if (u8Status == MI_OK){
			 if(Check_SDCard(CardID)) Incorrect_Card();
			 Correct_Card();
			
		}
		
		/* Check Password */
		u8Key = Key_Scan();
		/* Enter each character in the password */
		if (u8Key != 'a' && u8Key > 0 && SetupCheck.u8Incorrect == 0) {
			EnterPassWord();
		}
		/* Check Password */
	 if(u8Key == 'a') {
	    if (SetupCheck.u8CheckPass == 0 &&  SetupCheck.u8CheckLength == 7 ){
				CorrectPassWord();
			}
			else {
				if(SetupCheck.u8NumberOfIncorrect < 5){
			  InCorrectPassWord();
				SetupCheck.u8NumberOfIncorrect++;
				}
				/* Enter incorrect password 5 times */
				else {
					InCorrectPassWord();
					Lock_Disable(60);
					SetupCheck.u8NumberOfIncorrect = 0;
				}
			}
	 }
}
}
