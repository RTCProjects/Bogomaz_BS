#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "main.h"

#define LIMIT_COUNTS	10


typedef struct
{
	float 	shieldingFactor;		//коэф. экранирования
	//пороги обнаружения
	/*
		1. Порог обнаружения по превышению фона	он же СКО
		2. первый порог обнаружения превышения МД
		3. второй порог обнаружения превышения МД
		4. третий порог обнаружения превышения МД
		5. четвертый порог обнаружения превышения МД
		6. пятый порог обнаружения превышения МД
		7. первый порог обнаружения превышения Д
		8. второй порог обнаружения превышения Д
		9. третий порог обнаружения превышения Д
		10. чевертый порог обнаружения превышения Д
	
	*/
	float		limitDetect[LIMIT_COUNTS];				
	uint16	accumulationTime;		//время накопления спектра	 
}tSettings;

extern tSettings	MainSettings;

void		SETTINGS_Init(void);
void		SETTINGS_Default(void);
void 		SETTINGS_Save(void);
uint8  	SETTINGS_Load(void);

#endif
