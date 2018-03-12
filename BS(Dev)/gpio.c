#include "gpio.h"

void GPIO_SetPin(uint16 pin,uint8	_bit)
{
		uint16 mask = 1;
					 mask = mask << _bit;
	
		pin |= mask;
}
