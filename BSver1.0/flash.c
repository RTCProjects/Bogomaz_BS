#include "flash.h"

void FLASH_EraseAll()
{
		unsigned int far *Command_1 = (unsigned int far *) 0x080554;
		unsigned int far *Command_2 = (unsigned int far *) 0x080AAA;
	//step1
		*Command_1 = 0xAA00;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 	
	//step2
		*Command_2 = 0x0055;
	_nop_ (); _nop_ (); _nop_ (); _nop_ ();
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		//step3
		*Command_1 = 0x8000;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		//step4
		*Command_1 = 0xAA00;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		//step5
		*Command_2 = 0x0055;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		//step6
		*Command_1 = 0x1000;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 		
}

void FLASH_EraseSector(unsigned long sector)
{
		unsigned int far *Command_1 = (unsigned int far *) 0x080554;
		unsigned int far *Command_2 = (unsigned int far *) 0x080AAA;
	
		unsigned int far *Command_sector = (unsigned int far *)sector;
	
		*Command_1 = 0xAA00;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_2 = 0x0055;
	_nop_ (); _nop_ (); _nop_ (); _nop_ ();
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		
		*Command_1 = 0x8000;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_1 = 0xAA00;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_2 = 0x0055;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_sector = 0x0030;
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	_nop_ (); _nop_ (); _nop_ (); _nop_ ();
	_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
}

void FLASH_WriteWord(unsigned long address,unsigned int word)
{
		unsigned int far *Command_1 = (unsigned int far *) 0x080554;
		unsigned int far *Command_2 = (unsigned int far *) 0x080AAA;
	
		unsigned int far *Command_sector = (unsigned int far *)address;
	
		*Command_1 = 0xAA00;
		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_2 = 0x0055;
		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_1 = 0xA000;
		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
	
		*Command_sector = word;

		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		_nop_ (); _nop_ (); _nop_ (); _nop_ (); 
		_nop_ (); _nop_ (); _nop_ (); _nop_ ();
	
}
void FLASH_FSRControl() 
{
	volatile unsigned int far *FSR = (unsigned int far *) 0x08F000;
	volatile unsigned int Register = *FSR;
	
	while(Register & 0x0001){
		Register = *FSR;
	}
}

