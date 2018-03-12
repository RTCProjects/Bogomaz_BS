#ifndef _DEVICES_H
#define _DEVICES_H

#include "main.h"

//БДГП-С
/*
#define SPECTR_SIZE 			1024	//длина спектра
#define CHANNELS					4			//кол-во каналов
#define MAX_TIME					60		//максимальное время движка
#define BACKGROUND_TIME		30	//время накопления фона
 
 /*
#define CAN_BDGPS_EMERGENCY_MSG			0x000	//аварийное сообщение всех БДГП-С
#define CAN_BDGPS_CHAN_I_STAB				0x001
#define CAN_BDGPS_CHAN_II_STAB			0x002
#define CAN_BDGPS_CHAN_III_STAB			0x003
#define CAN_BDGPS_CHAN_IV_STAB			0x004

#define CAN_BDGPS_CHAN_I_RESPONSE				0x201	//сообщение ответа БДГП-С
#define CAN_BDGPS_CHAN_II_RESPONSE			0x202	//сообщение ответа БДГП-С
#define CAN_BDGPS_CHAN_III_RESPONSE			0x203	//сообщение ответа БДГП-С
#define CAN_BDGPS_CHAN_IV_RESPONSE			0x204	//сообщение ответа БДГП-С


*/

/*
typedef  struct
{
	uint16	Data[MAX_TIME][SPECTR_SIZE];
	uint8		Index;
	uint8		stabFlag;
	uint8		Ready;
}tSpectrChannel;

typedef struct
{
	float	Data[SPECTR_SIZE];
	uint8	Index;	//счетчик накопления данных
	uint8	Ready;	//флаг завершения накопления данных
}tBackgroundSpectr;
//БДГП-Б

//БДМГ-Б

*/

typedef struct
{
	int a;
}
tBDGPS_Device;

typedef struct
{
	int a;
}
tBDGPB_Device;

typedef struct
{
	int a;
}
tBDMGB_Device;

void	Devices_SendStateRequest(void);	//выполняем запрос к БД комплекса
void	Devices_ReceiveMsgCallback(uint16 id,uint8 xhuge	*msg);
void	Devices_BackgroundAccumulation(uint16 xhuge	*specArr,uint8	channel);
void 	Devices_SpectrAccumulation(uint16	*specArr,uint8	channel);
void	Devices_ClearBackgroundData(void);
uint16	*Devices_GetCurrentSpectr(uint8 channel);

uint16 Devices_Probe(void);
#endif
