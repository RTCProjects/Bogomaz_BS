#include "main.h"
#include "can.h"
#include "flash.h"
#include "settings.h"
#include "protocol.h"
#include "process.h"
#include "asc0.h"
#include "system.h"
#include "devices.h"
#include "math.h"

#include "bdsp.h"
#include "bdgp.h"

uint8 	malloc_mempool[0x1000];	//32Б динамической памяти 
uint8		ramTestOK = 1;

 const uint32	SerialNumberBD = 1301004;


void main(void)
{
	
	
	unsigned int far *pReprogWord = (unsigned int far *) 0x50000;

	//while(1);

	WDTIN = 1;
	WDTPRE = 1;
	
	
	CAN_Init();

	
	
	
	if(SWR == 1)
	{
		if(*pReprogWord == 0x1234)
		{
			*pReprogWord = 0;
			
			__asm {CALLS 04H 0E000H}
		}
		
	}
	init_asc0(20000000, 9600);
	
	ALTSEL0P2 = 0x0100;
	DP2 = 0x0100;

	

	T01CON = 0x0106;
	T0REL = 0x6769;		
//	T1REL = 0x1000;
	T1 = 0xC000;
	
	T0IC = 0x006A;
	T1IC = 0x0065;
	
	T3CON = 0x0005;
	T3IC = 0x006E;
	
	CCM2 = 0x000F;
	CC8 = 0xC500;

	
	
	IEN = 1;
	
	T1R = 1;
	
	//System_LEDState(LED_BLINKY_FAST);

	//ramTestOK = System_RAMTest();	//запускаем тест памяти
	
	//if(ramTestOK)
	{
		
		//инициализируем системы
		init_mempool(&malloc_mempool, sizeof(malloc_mempool));
		
		SETTINGS_Init();		
		IMDBProtocol_Init();

		
		Process_Initializetion();
		Process_SetStatus(IMDB_INITIALIZATION);	
//		IMDB_SendState();
		//запускам запрос на поиск блоков
		Devices_SendStateRequest();
		//запускаем главный таймер
		T0 = 0xFFFF;
		T0R = 1;
		
		//
		//проверяем статус сторожевого таймера
			if(WDTR == 1)
				IMDB_SendPacket(IMDB_HARDWAREFAIL,0,0);

		
	}
/*	else
		System_LEDState(LED_BLINKY_LOW);
*/
		while(1)
		{		
			_srvwdt_();
			
			BDSP_Process();
			BDGP_Process();
			IMDB_Process();
			
			CAN_Process();
		}
}



