#ifndef _BDSP_H
#define _BDSP_H

#include "main.h"
#include "protocol.h"

#define	CHANNELS	4 
#define NUCLIDES		5
/*
Нулевой нуклид - это всё окно от 0 до 128 канала
*/

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

#define QUERY_SIZE 8

#define BACKGROUND_TIME 30	//фон 30 сек
#define BACKGROUND_KOEF 1.0f / BACKGROUND_TIME

#define AMP_FACTOR	1.0f	//коэф. усиления - заглушка
//Данные спектрометрического канала
typedef struct
{
	uint8		ActiveBuffer;	//номер активного буфера
	uint8		ChangeBufferEvent;	//флаг смены буфера
	uint8		Ready;	//флаг стабилизации 0 - не готов 1 - ожидает стабилизации 2 - застабилизирован
	uint8		Detected;	//флаг обнаружения канала спектрометра после сброса
	uint8		Serial;	
	uint16	Background;	//накопление фона - 1 накоплен 0 - не накоплен
	uint16	AccumSpectr;	//накопление спектра - 1 накоплен 0 - не накоплен
	uint16	Data[QUERY_SIZE][SPECTR_SIZE];
	uint16	DebugData[SPECTR_SIZE];
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

	float		BackgroundAverageDose;
	float		DoseRate;
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




//режим идентификации
typedef enum
{
	BACK_MODE = 0x00,
	FULL_MODE = 0x01
}eIdentificationMode;

//статусы проверки инициализации канадов спектрометра
typedef enum
{
	CHK_OK = 0x01,							//каналы проинициализированы корректно
	CHK_INVALID_CHN_CNH = 0x02,	//не от всех каналов получен ответ перезапуска
	CHK_INVALID_CHN_WAIT = 0x03	//кол-во каналов в режиме ожидания стабилизации не соответствует кол-ву детектированных каналов
}eChanChkStatus;

typedef struct
{
	float	fDeadTime[CHANNELS];
	float	fSKO;
	float	fAligmentFactors[CHANNELS];
	float fCorrectionFactor;
}tBDSPParametrs;



void	BDSP_StartCalbiration(uint8 Channel);
void	BDSP_ParametrRequests(void);
void	BDSP_ParametrRequest(uint8 Channel,uint8 paramIndex);
void	BDSP_StartWritingParametrs(void);
void	BDSP_WriteParametr(uint8	Channel,uint8	paramIndex,uint8	*pParam,int dataSize);
void 	BDSP_Reset(void);
void 	BDSP_Start(void);
void 	BDSP_Stop(void);
void 	BDSP_StopChannel(uint8	Channel);

void 	BDPS_ClearQuery(void);
void	BDSP_ClearData(void);
void	BDSP_ClearWorkData(void);

void	BDSP_InsertCmd(uint8	Channel,uint8	*pData);
void	BDSP_InsertData(uint8	Channel,uint8	*pData);

void 	BDSP_InsertInQuery(uint16	 *pData, uint8	Channel);
void 	BDSP_StartSpectrAccumulation(eSpecAccumMode Mode);
void 	BDSP_BackgroundAccumulation(uint8	Channel,uint16	*pData);
void 	BDSP_ResetBackgroundData(void);
void	BDSP_SpectrAccumulation(uint8	Channel,uint16	*pData);
void	BDSP_SetMeasureNumber(uint8	MeasureNum);

uint8	BDSP_GetAccumQueryState(void);
uint8	BDSP_Identification(uint16	*ePhotoPeak, float	*iSpdCounter);
uint8		BDSP_GetBackgroundReady(void);	//запрос, накоплен ли фон у всех каналов
uint8		*BDSP_GetCurrentSpectr(void);
uint8		*BDSP_GetChannelSpectr(uint8	channel);
uint8		*BDSP_GetAccumulationSpectr(uint8	Channel);
void		BDSP_CalculateAngularDiagram(eSpecMode Mode);
float		*BDSP_GetAngularDiagram(void);
float		BDSP_GetDose(void);
float		BDSP_GetDoseRate(void);
float		BDSP_GetBackgroundQueryIntegral(void);
uint16	BDSP_GetMaximumAngle(void);


uint16	BDSP_MinCounter(uint8	*minChannelIndex);
uint16	BDSP_MaxCounter(uint8	*maxChannelIndex);


void 	BDSP_SetSpectrChannelSize(eSpecMode eMode,uint8	Channel);
void	BDSP_Process(void);

eChanChkStatus		BDSP_ChannelInitCheck(void);
sSpecModeData			BDSP_GetMeasureInPointData(void);

uint8	*BDSP_GetParametrs(void);

uint8	*BDSP_DEBUG_GetQuerySumSpectr(uint8	channel);
#endif
