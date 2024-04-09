
#include "I2C_LCD.h"

extern I2C_TypeDef hi2c1;

static uint8_t dpFunction;
static uint8_t dpControl;
static uint8_t dpMode;
static uint8_t dpRows;
static uint8_t dpBacklight;

static void SendCommand(uint8_t);
static void SendChar(uint8_t);
static void Send(uint8_t, uint8_t);
static void Write4Bits(uint8_t);
static void ExpanderWrite(uint8_t);
static void PulseEnable(uint8_t);

static uint8_t special1[8] = {
        0U,
        25U,
        27U,
        6U,
        12U,
        27U,
        19U,
        0U
};

static uint8_t special2[8] = {
        24U,
        24U,
        6U,
        9U,
        8U,
        9U,
        6U,
        0U
};
static uint8_t special3[8] = {
       0x04,
       0x0E,
       0x1F,
       0x0E,
       0x04,
       0x00,
       0x00,
       0x00
};

static uint8_t special4[8] = {
       0x04,
       0x04,
       0x04,
       0x04,
       0x04,
       0x04,
       0x00,
       0x04
};


void HD44780_Init(uint8_t rows)
{
  dpRows = rows;

  dpBacklight = LCD_BACKLIGHT;

  dpFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  if (dpRows > 1)
  {
    dpFunction |= LCD_2LINE;
  }
  else
  {
    dpFunction |= LCD_5x10DOTS;
  }
  ExpanderWrite(dpBacklight);
  Delay_Ms(1000);

  /* 4bit Mode */
  Write4Bits(0x03 << 4);
  Delay_Us(4500);

  Write4Bits(0x03 << 4);
  Delay_Us(4500);

  Write4Bits(0x03 << 4);
  Delay_Us(4500);

  Write4Bits(0x02 << 4);
  Delay_Us(100);

  /* Display Control */
  SendCommand(LCD_FUNCTIONSET | dpFunction);

  dpControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  HD44780_Display();
  HD44780_Clear();

  /* Display Mode */
  dpMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  SendCommand(LCD_ENTRYMODESET | dpMode);
  Delay_Us(4500);

  HD44780_CreateSpecialChar(0, special1);
  HD44780_CreateSpecialChar(1, special2);
	HD44780_CreateSpecialChar(2, special3);
  HD44780_CreateSpecialChar(3, special4);

  HD44780_Home();
}

void HD44780_Clear(void)
{
  SendCommand(LCD_CLEARDISPLAY);
  Delay_Us(2000);
}

void HD44780_Home(void)
{
  SendCommand(LCD_RETURNHOME);
  Delay_Us(2000);
}

void HD44780_SetCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (row >= dpRows)
  {
    row = dpRows-1;
  }
  SendCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void HD44780_NoDisplay(void)
{
  dpControl &= ~LCD_DISPLAYON;
  SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_Display(void)
{
  dpControl |= LCD_DISPLAYON;
  SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_NoCursor(void)
{
  dpControl &= ~LCD_CURSORON;
  SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_Cursor(void)
{
  dpControl |= LCD_CURSORON;
  SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_NoBlink(void)
{
  dpControl &= ~LCD_BLINKON;
  SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_Blink(void)
{
  dpControl |= LCD_BLINKON;
  SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

void HD44780_ScrollDisplayLeft(void)
{
  SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void HD44780_ScrollDisplayRight(void)
{
  SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void HD44780_LeftToRight(void)
{
  dpMode |= LCD_ENTRYLEFT;
  SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_RightToLeft(void)
{
  dpMode &= ~LCD_ENTRYLEFT;
  SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_AutoScroll(void)
{
  dpMode |= LCD_ENTRYSHIFTINCREMENT;
  SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_NoAutoScroll(void)
{
  dpMode &= ~LCD_ENTRYSHIFTINCREMENT;
  SendCommand(LCD_ENTRYMODESET | dpMode);
}

void HD44780_CreateSpecialChar(uint8_t location, uint8_t charmap[])
{
	int i;
	
  location &= 0x7;
  SendCommand(LCD_SETCGRAMADDR | (location << 3));
  for (i=0; i<8; i++)
  {
    SendChar(charmap[i]);
  }
}

void HD44780_PrintSpecialChar(uint8_t index)
{
  SendChar(index);
}

void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t *rows)
{
  HD44780_CreateSpecialChar(char_num, rows);
}

void HD44780_PrintStr(const char c[])
{
  while(*c) SendChar(*c++);
}

void HD44780_SetBacklight(uint8_t new_val)
{
  if(new_val) HD44780_Backlight();
  else HD44780_NoBacklight();
}

void HD44780_NoBacklight(void)
{
  dpBacklight=LCD_NOBACKLIGHT;
  ExpanderWrite(0);
}

void HD44780_Backlight(void)
{
  dpBacklight=LCD_BACKLIGHT;
  ExpanderWrite(0);
}

static void SendCommand(uint8_t cmd)
{
  Send(cmd, 0);
}

static void SendChar(uint8_t ch)
{
  Send(ch, RS);
}

static void Send(uint8_t value, uint8_t mode)
{
  uint8_t highnib = value & 0xF0;
  uint8_t lownib = (value<<4) & 0xF0;
  Write4Bits((highnib)|mode);
  Write4Bits((lownib)|mode);
}

static void Write4Bits(uint8_t value)
{
  ExpanderWrite(value);
  PulseEnable(value);
}

static void ExpanderWrite(uint8_t _data)
{
  uint8_t data = _data | dpBacklight;

	/* Send START condition */
  I2C_GenerateSTART(I2C_Chanel, ENABLE);
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_MODE_SELECT));
  /* Send PCF8574A address for write */
  I2C_Send7bitAddress(I2C_Chanel, DEVICE_ADDR, I2C_Direction_Transmitter);
	/* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  /* Send the byte to be written */
  I2C_SendData(I2C_Chanel, data);
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C_Chanel, ENABLE);
}

static void PulseEnable(uint8_t _data)
{
  ExpanderWrite(_data | LCD_ENABLE);
  Delay_Us(20);

  ExpanderWrite(_data & ~LCD_ENABLE);
  Delay_Us(20);
}

void Delay_Ms(uint32_t u32Delay)
{
	while(u32Delay) 
	{
		/* Dealy 1ms*/
		SysTick->LOAD = 72*1000 - 1;
		SysTick->VAL = 0;
		SysTick->CTRL = 5;
		
		while (!(SysTick->CTRL & (1 << 16)))
		{
			/* Wating for Returns 1 if timer counted to 0 since last time this was read*/
		}
		--u32Delay;
	}
}
void Delay_Us(uint32_t u32Delay)
{
	while(u32Delay) 
	{
		/* Dealy 1ms*/
		SysTick->LOAD = 72 - 1;
		SysTick->VAL = 0;
		SysTick->CTRL = 5;
		
		while (!(SysTick->CTRL & (1 << 16)))
		{
			/* Wating for Returns 1 if timer counted to 0 since last time this was read*/
		}
		--u32Delay;
	}
}

