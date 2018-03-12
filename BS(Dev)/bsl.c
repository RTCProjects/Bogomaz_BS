#include "bsl.h"
#include "can.h"
#include "main.h"

/*
0x00019C - вектор для подпрограммы прошивки данных

	019C = FA05
	019E = 0000 - или другой адрес, по которому мы загрузим в ОЗУ подпрограмму загрузчика
*/

void	BSL_Init()
{
		//настройка MO CAN
	/*
	CAN_IC15 = 0x0068;
	
	CAN_Message_16[14].MOCTRH = 0x0E08;
	CAN_Message_16[14].MOCTRL = 0x0000;
	CAN_Message_16[14].MOFCRH = 0x0800;
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();

	
	CAN_Message_16[15].MOCTRH = 0x0080;
	CAN_Message_16[15].MOCTRL = 0x0000;
	CAN_Message_16[15].MOIPRL = 0x000F;
	CAN_Message_16[15].MOFCRH = 0x0801;
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	
	
	CAN_Message_16[14].MOARH = 0x9558;	 // идентификатор 556
  CAN_Message_16[14].MOAMRH = 0x3FFF; //	маска идентификатора
	
	CAN_Message_16[15].MOARH = 0x9554;	 // идентификатор 555
  CAN_Message_16[15].MOAMRH = 0x3FFF; //	маска идентификатора
	
	

	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();	
	
	PANCTR_H = 0x010E;				 // message object 14 -> List 1
  PANCTR = 0x0002;	
	 _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
 
	PANCTR_H = 0x010F;				 // message object 14 -> List 1
  PANCTR = 0x0002;	
	 _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
 

	CAN_Message_16[14].MOCTRH = 0x0020; 
	CAN_Message_16[14].MOCTRL = 0x0000;

	CAN_Message_16[15].MOCTRH = 0x0020; 
	CAN_Message_16[15].MOCTRL = 0x0000;
	
	__asm
	{
		CALLS 04H 0E000H
	}
	*/
}


void BSL_Start()
{
	/*
		1. Очищаем PSRAM
		2. Загружаем в PSRAM загрузчик
		3. Прыгаем в PSRAM
	*/
	uint8	msgData[8] = {0x09,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
  CAN_SendMessage(0x556,msgData,8);
		
}
/*
void BSL_Receive(void) interrupt 0x67
{

	CAN_Message_16[14].MOCTRH = 0x0100;
}*/
