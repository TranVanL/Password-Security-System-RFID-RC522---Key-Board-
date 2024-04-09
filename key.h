#ifndef KEY_H_
#define KEY_H_

#include "stm32f10x.h"

void Key_Init(void);
uint32_t Key_Scan(void);

#endif