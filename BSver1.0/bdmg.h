#ifndef _BDMG_H
#define _BDMG_H

#include "main.h"

#define	CAN_BDMG_ALL_TX		0x320
#define CAN_BDMG_CMD_TX		0x321
#define CAN_BDMG_CMD_RX		0x521
#define CAN_BDMG_DATA_RX	0x721

typedef struct
{
	float 	currentDose;		//текущая полученная МД
	float 	averageDose;		//средняя МД
	float		oldCurrentDose; //текущая МД на прошлом шаге
	float		Dose;						//Доза
	uint8		rangeNumber;
	uint16	counterRate;
}tBDMGData;

typedef struct
{
	float		correctionFactors[8];
	float		mDeadTime[3];
	float		fSensCounters[3];
	uint16	range1to2Value;
	uint16	range2to3Value;
	uint16	range3maxValue;
	uint16	range3to2Value;
	uint16	range2to1Value;
	
}tBDMGParametrs;

void 	BDMG_Reset(void);
void 	BDMG_Start(void);
void	BDMG_ForcedStart(uint8	rangeIndex);
void 	BDMG_Stop(void);
void	BDMG_InsertData(uint8	*pData);
void 	BDMG_InsertCmd(uint8	*pData);

float BDMG_GetDose(void);
float	BDMG_GetCurrentDose(void);
uint8	BDMG_GetInsideFeature(void);

void	BDMG_ParametrRequest(uint8	paramIndex,uint16	paramSubIndex);
void 	BDMG_WriteParametr(uint8	paramIndex,uint16 paramSubIndex, uint8	*pParam,int dataSize);
void	BDMG_StartWritingParametrs(void);
uint8	*BDMG_GetParametrs(void);
#endif
