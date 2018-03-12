#include "system.h"

sbit P2_8 = P2^8;

uint8	System_RAMTest(void)
{
		__IO uint32	cnt = 0;
		__IO uint32	temp = 0;
		uint32 	index = 0;
		uint16		*ramStart = 0x600000;

		uint16	ramData = 0;
		uint16	loopData = 0;
	
		xmemset(ramStart,0,0x80000);
	
		for(index = 0;index<0x40000;index++)
		{
			(*(ramStart+index)) = (uint16)index;
		}
	
		for(index = 0;index<0x40000;index++)
		{
			ramData = (*((uint16*)ramStart+index));
		
			
			if(ramData != (uint16)index)
			{
				return 0;
			}	
		}
		xmemset(ramStart,0,0x80000);
		
		return 1;
}

void	System_Delay(unsigned long z){
  __IO unsigned long x;
  for (x = 0; x < z; x++){
		_nop_();
		_nop_();
		
	}
}
void	System_Reset()
{
		__asm { SRST }
}

void	System_LEDPulse()
{

}

void	System_LEDState(eLEDState	State)
{
	switch(State)
	{
		case LED_ON:
		{
			if(T3R)
				T3R = 0;
		   	CC8 = 0xC500;
		}break;
		
		case LED_OFF:
		{
			if(T3R)
				T3R = 0;
			CC8 = 0xC001;
		}break;
		
		case LED_BLINKY_FAST:
		{
			T3CON = 0x0003;
			T3R = 1;
			
		}break;
		
		case LED_BLINKY_LOW:
		{
			T3CON = 0x0005;
			T3R = 1;
		}break;
	}
}

void _timer1(void) interrupt T1IC_VEC
{
	
	T1 = 0xC000;

	
}
void _timer3(void) interrupt T3IC_VEC
{
CC8 = 0xC500;	
	/*
	if(CC8 == 0xC500){
		CC8 = 0xC001;
		return;}
	if(CC8 == 0xC001){
		
		return;}*/
}
