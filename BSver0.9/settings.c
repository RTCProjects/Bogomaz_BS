#include "settings.h"
#include "flash.h"

tSettings	MainSettings;

void	SETTINGS_Init()
{

	if(!SETTINGS_Load())
	{
		SETTINGS_Default();	
	}
}

void	SETTINGS_Default()
{
	int i = 0;
	
		MainSettings.shieldingFactor = 4.0f;//коэф. экранирования
	
		MainSettings.limitDetect[0] = 4.0f;	//SKO
		//МД мГр/ч
		MainSettings.limitDetect[1] = 0.01f;
		MainSettings.limitDetect[2] = 1.0f;
		MainSettings.limitDetect[3] = 100.0f;
		MainSettings.limitDetect[4] = 1000.0f;
		MainSettings.limitDetect[5] = 5000.0f;
		//Д мГр/ч
		MainSettings.limitDetect[6] = 10.0f;
		MainSettings.limitDetect[7] = 100.0f;
		MainSettings.limitDetect[8] = 250.0f;
		MainSettings.limitDetect[9] = 1000.0f;
	
	MainSettings.accumulationTime = 30;
	
	SETTINGS_Save();
	
}

void 	SETTINGS_Save()
{
	int i = 0,addr_counter = 0;
	
	 uint16  *pSettings = (tSettings*)&MainSettings;
	 uint16 sizeSettings = sizeof(MainSettings);
	
	 uint16	chkSum = 0;
	
	 for(i = 0;i<sizeSettings >> 1;i++)
			chkSum += *(pSettings + i);
	
	FLASH_EraseSector(0x22000);
	FLASH_FSRControl();
	
	FLASH_WriteWord(0x22000,chkSum);
	FLASH_FSRControl();

	
	for(i = 0;i<sizeSettings;i+=2)
	{
		FLASH_WriteWord(0x22002 + i,*(pSettings + addr_counter));
		FLASH_FSRControl();
		

		addr_counter++;
	}
}
uint8 SETTINGS_Load()
{
	int i = 0;
	unsigned int far *addressChkSum = (unsigned int far *) 0x022000;
	unsigned int far *addressData = (unsigned int far *) 0x022002;
	
	uint16	*pSettings = (tSettings*)&MainSettings;
	uint16	dataSize = sizeof(MainSettings);
	uint16  chkSum = 0;
	
	for(i = 0;i<dataSize >> 1;i++){
		chkSum += *(addressData + i);

	}

	if(chkSum != *addressChkSum)
		return 0;
	
	
	for(i = 0;i<dataSize >> 1;i++){
		*(pSettings + i) = *(addressData + i);
		
	}
	return 1;
}
