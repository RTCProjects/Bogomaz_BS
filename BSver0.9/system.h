#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "main.h"

typedef enum
{
	LED_ON,
	LED_OFF,
	LED_BLINKY_FAST,
	LED_BLINKY_LOW
}eLEDState;

uint8	System_RAMTest(void);
void	System_Delay(unsigned long z);
void	System_Reset(void);
void		System_LEDState(eLEDState	State);
void	System_LEDPulse(void);
#endif
