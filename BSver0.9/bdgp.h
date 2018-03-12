#ifndef _BDGP_H
#define _BDGP_H

#include "main.h"

#include "bdsp.h"

#define CAN_BDGP_CMD_TX	0x180
#define CAN_BDGP_CMD_RX	0x280

#define CAN_BDGP_DATA_II_RX		0x723
#define CAN_BDGP_DATA_III_RX	0x724

#define SENSORS		 	12
#define SLIDER_SIZE 30



typedef struct
{
	float 	currentDose;
	float 	averageDose;
	float		Dose;
	float		BackgroundAverageDose;	//фоновая МД
	float		BackgroundAverageSpeed;	//средняя фоновая скорост ьсчёта
	float		fQuantile;	//квантиль фоновой идентификации
	
	uint8		rangeNumber;
	uint8		Background;	// флаг накопления фона
	uint8		QuerySuccess;	//флаг накопления движка МД
	uint8		ActiveBuffer;	//номер активного буфера
	uint8		ChangeBufferEvent;	//флаг смены буфера
	uint8		NuclideDetection;	//флаг идентификации нуклида
	
	uint16	counterRate;
	float		sensorsCntQuery[SLIDER_SIZE][SENSORS];	//очередь счетов БД с поправкой на поправочные к-ты
	float		fCorrectionSumSensData[SENSORS];		//массив счетов для передачи в ф-ию вычисления значения угловой диаграммы
	uint16	InputData[2][SENSORS];			//массив счетов с БД
	uint16	WorkData[SENSORS];	//копия массива счетов
	
	uint16	BackgroundIndex;	//счётчик накопления фона
	uint16	radiationMaximum;
	
	float		nLikelihoods[48];
}tBDGPData;

//параметры ля одного диапазона БДГП
typedef struct
{
	float		fCorrectionFactors[12];
	float		fDeadTime;
	float 	fEffectCounters;
	float		fSensCounters;
	uint16	rangeMin;
	uint16	rangeMax;
}tBDGPParametrs;

void 	BDGP_Reset(void);
void 	BDGP_Start(void);
void	BDGP_ForcedStart(uint8	rangeNumber);
void 	BDGP_Stop(void);
void	BDGP_InsertParametrs(uint16	regID,uint8	*pData);
void	BDGP_InsertDataNew(uint16 regID,uint8	*pData);
void 	BDGP_InsertCmd(uint8	*pData);

void	BDGP_Process(void);
void	BDGP_BackgroundAccumulation(uint16	*pData);
void	BDGP_ResetBackgroundData(void);
void	BDGP_StartWritingParametrs(uint8	rangeIndex);
void	BDGP_ParametrRequest(uint8	range,uint8	paramIndex,uint8	paramSubIndex);
void 	BDGP_WriteParametr(uint8	range,uint8	paramIndex,uint8 paramSubIndex, uint8	*pParam,int dataSize);
float	*BDGP_DirectionDiagramm(float	*pData, uint8	SensCount, float cntMin,float cntMax, uint16	*pAngleMax, eAngDiagrammMode Mode);

float 	BDGP_GetAverageDoseRate(void);
float		BDGP_GetCurrentDoseRate(void);
uint8		BDGP_GetOutsideFeature(void);
float		*BDGP_GetAngularDiagram(void);
float		BDGP_GetMaximumAngle(void);
float 	BDGP_GetDose(void);

uint8		*BDGP_GetParametrs(void);
uint8		BDGP_GetBackgroundState(void);
void		BDGP_BackgroundIdentification(void);

#endif
