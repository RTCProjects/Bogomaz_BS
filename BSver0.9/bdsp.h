#ifndef _BDSP_H
#define _BDSP_H

#include "main.h"

#define	CHANNELS	4 

#define CAN_BDSP_EMER_ALL_RX	0x000
#define CAN_BDSP_EMER_CH1_RX	0x001
#define CAN_BDSP_EMER_CH2_RX	0x002
#define CAN_BDSP_EMER_CH3_RX	0x003
#define CAN_BDSP_EMER_CH4_RX	0x004

#define CAN_BDSP_DATA_TX	0x100

#define CAN_BDSP_DATA_CH1_RX	0x201
#define CAN_BDSP_DATA_CH2_RX	0x202
#define CAN_BDSP_DATA_CH3_RX	0x203
#define CAN_BDSP_DATA_CH4_RX	0x204

#define SPECTR_SIZE 		1024
#define SPECTR_SIZE_128	128

#define QUERY_SIZE	10

#define BACKGROUND_TIME 5	//фон 30 сек
#define BACKGROUND_KOEF 1.0f / BACKGROUND_TIME

//Данные спектрометрического канала
typedef struct
{
	uint8		ActiveBuffer;	//номер активного буфера
	uint8		ChangeBufferEvent;	//флаг смены буфера
	uint8		Ready;	//флаг стабилизации 0 - не готов 1 - ожидает стабилизации 2 - застабилизирован
	uint8		Serial;	
	uint16	Background;	//накопление фона - 1 накоплен 0 - не накоплен
	uint16	AccumSpectr;	//накопление спектра - 1 накоплен 0 - не накоплен
	uint16	Data[QUERY_SIZE][SPECTR_SIZE];
	uint16	InputData[2][SPECTR_SIZE];
	uint16	WorkData[SPECTR_SIZE];
	uint16	DataCounter;	//счётчик входящих посылок
	uint16	QueryIndex;		//счётчик очереди спектров
	uint16	BackgroundIndex;	//счётчик накопления фона
	uint16	AccumSpectrIndex;	//счётчик накопления данных в спектрометрическом режиме
	
	uint32	Integral;	//интеграл по спектру
	uint32	ppMax;	//максимум фотопика
	float		ppBackgroundMax;	//максимум фотопика по фону
	float		BackgroundData[SPECTR_SIZE];			//фон 1024 канала
	float		BackgroundData128[SPECTR_SIZE_128];//фон 128 каналов
	float		fBackgroundQuantile;							//квантиль фона
	float		AccumSpectrData[SPECTR_SIZE];			//данные для накопления в спектрометрическом режиме

	float		Dose;
}tBDSPData;

//режимы приёмопередачи спектра
typedef enum
{
	SIZE_128  = 0x80,
	SIZE_1024 = 0x400
}eSpecMode;

//режимы накопления спектра
typedef enum
{
	ACCUM_ONLY = 0x01,
	ACCUM_AND_MEASURE = 0x02
}eSpecAccumMode;

//режимы накопления спектров в точке
typedef enum
{
	MEASURE_ONE = 0x01,
	MEASURE_TWO = 0x02
}eSpecMeasureNumber;

typedef struct
{
	float	fAligmentFactors[4];
	float	fDeadTime;
	float	fSKO;
	float	fCorrectionFactor;
}tBDSPParametrs;

void	BDSP_SetParametrs(void);

void 	BDSP_Reset(void);
void 	BDSP_Start(void);
void 	BDSP_Stop(void);
void 	BDSP_StopChannel(uint8	Channel);

void 	BDPS_ClearQuery(void);
void	BDSP_ClearData(void);

void	BDSP_InsertCmd(uint8	Channel,uint8	*pData);
void	BDSP_InsertData(uint8	Channel,uint8	*pData);

void 	BDSP_InsertInQuery(uint16	 *pData, uint8	Channel);
void 	BDSP_StartSpectrAccumulation(eSpecAccumMode Mode);
void 	BDSP_BackgroundAccumulation(uint8	Channel,uint16	*pData);
void 	BDSP_ResetBackgroundData(void);
void	BDSP_SpectrAccumulation(uint8	Channel,uint16	*pData);
uint8 BDSP_BackgroundDetection(void);
uint8	BDSP_Identification(uint16	*ePhotoPeak, uint16	*iSpdCounter);
//float	BDSP_BackgroundExcess(void);	//рассчёт квантиля по фону

uint8		BDSP_GetBackgroundReady(void);	//запрос, накоплен ли фон у всех каналов
uint8		*BDSP_GetCurrentSpectr(void);
uint8		*BDSP_GetChannelSpectr(uint8	channel);
uint8		*BDSP_GetAccumulationSpectr(uint8	Channel);
float		*BDSP_GetAngularDiagram(void);
float		BDSP_GetDose(void);
uint16	BDSP_GetMaximumAngle(void);


uint16	BDSP_MinCounter(uint8	*minChannelIndex);
uint16	BDSP_MaxCounter(uint8	*maxChannelIndex);



void	BDSP_Process(void);


uint8	*BDSP_GetParametrs(void);
#endif
