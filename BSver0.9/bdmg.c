#include "bdmg.h"
#include "can.h"
#include "process.h"
#include "protocol.h"


static uint8	msgCounter = 0;
__IO static float	oldCurrentDose = 0;

const uint8		startBDMGMsg[] = {0x40};
const uint8		stopBDMGMsg[] = {0x41};

const uint8		resetBDMGMsg[] = {0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8		autoModeBDMGMsg[] = {0x05,0x90,0x00,0x00,0x00,0x00,0x00,0x00};
//константы запросов параметров
const uint8		getDeadTime1Msg[] = {0x04,0x94,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8		getDeadTime2Msg[] = {0x04,0x95,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8		getDeadTime3Msg[] = {0x04,0x96,0x00,0x00,0x00,0x00,0x00,0x00};


static tBDMGData	BDMGData;
static tBDMGParametrs	BDMGParamets;

void 	BDMG_Reset()
{
	msgCounter = 0;
	
	memset((uint8*)&BDMGData,0,sizeof(tBDMGData));
	memset((uint8*)&BDMGParamets,0,sizeof(tBDMGParametrs));
	
	CAN_SendMessage(CAN_BDMG_ALL_TX,resetBDMGMsg,8);	//команла перезапуска БДМГ-Б

}

void 	BDMG_Start()
{
	CAN_SendMessage(CAN_BDMG_CMD_TX,autoModeBDMGMsg,8);
	CAN_SendMessage(CAN_BDMG_CMD_TX,startBDMGMsg,1);
	
}
void BDMG_ForcedStart(uint8	rangeIndex)
{
	uint8	pData[8];
	
		memcpy(pData,autoModeBDMGMsg,sizeof(uint8) * 8);
	
		pData[4] = rangeIndex;
	
	CAN_SendMessage(CAN_BDMG_CMD_TX,pData,8);
	CAN_SendMessage(CAN_BDMG_CMD_TX,startBDMGMsg,1);
}

void 	BDMG_Stop()
{
	CAN_SendMessage(CAN_BDMG_CMD_TX,stopBDMGMsg,1);
}
//запрос параметра
void	BDMG_ParametrRequest(uint8	paramIndex,uint16	paramSubIndex)
{
	uint8	Data[8];
		
		memset((uint8*)Data,0,sizeof(uint8) * 8);
	
		Data[0] = 0x04;
		Data[1] = paramIndex;
		Data[2] = (uint8)paramSubIndex & 0xF;
		Data[3] = (uint8) (paramSubIndex >> 8);

	CAN_SendMessage(CAN_BDMG_CMD_TX,Data,8);
}
void BDMG_WriteParametr(uint8	paramIndex,uint16 paramSubIndex, uint8	*pParam,int dataSize)
{
	uint8	Data[8];
	
		memset((uint8*)Data,0,sizeof(uint8) * 8);
	
		Data[0] = 0x05;
		Data[1] = paramIndex;
		Data[2] = (uint8)paramSubIndex & 0xF;
		Data[3] = (uint8) (paramSubIndex >> 8);
	
		memcpy(Data + 4,(uint32*)(pParam),dataSize);
	
	CAN_SendMessageIT(CAN_BDMG_CMD_TX,Data,8);
		
}
//запуск записи параметров в БД
void	BDMG_StartWritingParametrs()
{
	BDMG_WriteParametr(0x94,0,(uint8*)&BDMGParamets.mDeadTime[0],sizeof(float));
}
//приём данных
void	BDMG_InsertData(uint8	*pData)
{
	if(pData[4] == 0xFF && pData[5] == 0xFF && pData[6] == 0xFF && pData[7] == 0xFF)//приняли последнее сообщение данных с МД
	{
		BDMGData.oldCurrentDose = BDMGData.currentDose;
		
		memcpy((uint8*)&BDMGData.currentDose,(uint8*)pData,sizeof(float));
		
		BDMGData.Dose += BDMGData.currentDose;
	}

}
//приём команд и параметров
void	BDMG_InsertCmd(uint8	*pData)
{
	//команда перезапуска принята
	if(pData[0] == 0x01 && pData[1] == 0x01)
	{
		Process_BDMGStatus(DEVICE_READY);

		BDMG_ParametrRequest(0x94,0);	//запрос первого параметра
	}
	
	switch(pData[0])
	{
		//выдать параметр
		case 0x04:
		{
			if(pData[2] == 0x94){
				memcpy((uint8*)&BDMGParamets.mDeadTime[0],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0x95,0);
			}
			if(pData[2] == 0x95){
				memcpy((uint8*)&BDMGParamets.mDeadTime[1],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0x96,0);
			}
			if(pData[2] == 0x96){
				memcpy((uint8*)&BDMGParamets.mDeadTime[2],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0x97,0);
			}
			
			if(pData[2] == 0x97){
				memcpy((uint8*)&BDMGParamets.range1to2Value,(pData + 4),sizeof(uint16));
				BDMG_ParametrRequest(0x98,0);
			}
			if(pData[2] == 0x98){
				memcpy((uint8*)&BDMGParamets.range2to3Value,(pData + 4),sizeof(uint16));
				BDMG_ParametrRequest(0x99,0);
			}
			if(pData[2] == 0x99){
				memcpy((uint8*)&BDMGParamets.range3maxValue,(pData + 4),sizeof(uint16));
				BDMG_ParametrRequest(0x9A,0);
			}
			if(pData[2] == 0x9A){
				memcpy((uint8*)&BDMGParamets.range3to2Value,(pData + 4),sizeof(uint16));
				BDMG_ParametrRequest(0x9B,0);
			}
			if(pData[2] == 0x9B){
				memcpy((uint8*)&BDMGParamets.range2to1Value,(pData + 4),sizeof(uint16));
				BDMG_ParametrRequest(0x9C,0);
			}
			if(pData[2] == 0x9C){
				memcpy((uint8*)&BDMGParamets.fSensCounters[0],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0x9D,0);
			}
			if(pData[2] == 0x9D){
				memcpy((uint8*)&BDMGParamets.fSensCounters[1],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0x9E,0);
			}
			if(pData[2] == 0x9E){
				memcpy((uint8*)&BDMGParamets.fSensCounters[2],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xA1,0);
			}
			
			
			if(pData[2] == 0xA1){
				memcpy((uint8*)&BDMGParamets.correctionFactors[0],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xA2,0);
			}
			if(pData[2] == 0xA2){
				memcpy((uint8*)&BDMGParamets.correctionFactors[1],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xA3,0);
			}
			if(pData[2] == 0xA3){
				memcpy((uint8*)&BDMGParamets.correctionFactors[2],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xA4,0);
			}
			if(pData[2] == 0xA4){
				memcpy((uint8*)&BDMGParamets.correctionFactors[3],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xB1,0);
			}
			if(pData[2] == 0xB1){
				memcpy((uint8*)&BDMGParamets.correctionFactors[4],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xB2,0);
			}
			if(pData[2] == 0xB2){
				memcpy((uint8*)&BDMGParamets.correctionFactors[5],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xC1,0);
			}
			if(pData[2] == 0xC1){
				memcpy((uint8*)&BDMGParamets.correctionFactors[6],(pData + 4),sizeof(float));
				BDMG_ParametrRequest(0xC2,0);
			}
			//последний параметр
			if(pData[2] == 0xC2){
				memcpy((uint8*)&BDMGParamets.correctionFactors[7],(pData + 4),sizeof(float));
			}
				 
		}break;
		//задать параметрf
		case 0x05:
		{
			switch(pData[2])
			{
				case 0x94: BDMG_WriteParametr(0x95,0,(uint8*)&BDMGParamets.mDeadTime[1],sizeof(float)); break;
				case 0x95: BDMG_WriteParametr(0x96,0,(uint8*)&BDMGParamets.mDeadTime[2],sizeof(float)); break;
				case 0x96: BDMG_WriteParametr(0x97,0,(uint8*)&BDMGParamets.range1to2Value,sizeof(uint16)); break;
				case 0x97: BDMG_WriteParametr(0x98,0,(uint8*)&BDMGParamets.range2to3Value,sizeof(uint16)); break;
				case 0x98: BDMG_WriteParametr(0x99,0,(uint8*)&BDMGParamets.range3maxValue,sizeof(uint16)); break;
				case 0x99: BDMG_WriteParametr(0x9A,0,(uint8*)&BDMGParamets.range3to2Value,sizeof(uint16)); break;
				case 0x9A: BDMG_WriteParametr(0x9B,0,(uint8*)&BDMGParamets.range2to1Value,sizeof(uint16)); break;
				case 0x9B: BDMG_WriteParametr(0x9C,0,(uint8*)&BDMGParamets.fSensCounters[0],sizeof(float)); break;
				case 0x9C: BDMG_WriteParametr(0x9D,0,(uint8*)&BDMGParamets.fSensCounters[1],sizeof(float)); break;
				case 0x9D: BDMG_WriteParametr(0x9E,0,(uint8*)&BDMGParamets.fSensCounters[2],sizeof(float)); break;
				case 0x9E: BDMG_WriteParametr(0xA1,0,(uint8*)&BDMGParamets.correctionFactors[0],sizeof(float)); break;
				
				case 0xA1: BDMG_WriteParametr(0xA2,0,(uint8*)&BDMGParamets.correctionFactors[1],sizeof(float)); break;
				case 0xA2: BDMG_WriteParametr(0xA3,0,(uint8*)&BDMGParamets.correctionFactors[2],sizeof(float)); break;
				case 0xA3: BDMG_WriteParametr(0xA4,0,(uint8*)&BDMGParamets.correctionFactors[3],sizeof(float)); break;
				case 0xA4: BDMG_WriteParametr(0xB1,0,(uint8*)&BDMGParamets.correctionFactors[4],sizeof(float)); break;
				
				case 0xB1: BDMG_WriteParametr(0xB2,0,(uint8*)&BDMGParamets.correctionFactors[5],sizeof(float)); break;
				case 0xB2: BDMG_WriteParametr(0xC1,0,(uint8*)&BDMGParamets.correctionFactors[6],sizeof(float)); break;
					
				case 0xC1: BDMG_WriteParametr(0xC2,0,(uint8*)&BDMGParamets.correctionFactors[7],sizeof(float)); break;
				case 0xC2: {}break;
			}
		}break;
	}
}
float	BDMG_GetCurrentDose()
{
	return BDMGData.currentDose;
}
float BDMG_GetDose()
{
	return BDMGData.Dose;
}
uint8	BDMG_GetInsideFeature()
{

		if(BDMGData.currentDose >= 1.3f * BDMGData.oldCurrentDose)
			return 1;	//МД увеличилась
		else if(BDMGData.currentDose <= 0.7f * BDMGData.oldCurrentDose)
			return 2;	//МД уменьшилась
		else
			return 0;	//МД неизменилась
}
uint8	*BDMG_GetParametrs()
{
	return (uint8*)&BDMGParamets;
}
