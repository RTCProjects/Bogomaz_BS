#include "devices.h"
#include "can.h"
#include "protocol.h"
#include "process.h"
#include "protocol.h"
#include "system.h"
//устройства
#include "bdmg.h"	//БДМГ-Б
#include "bdgp.h"	//БДГП-Б
#include "bdsp.h"	//БДГП-С
/*
Реализация работы с блоками детектирования комплекса ИМД-Б
*/
/*
uint16	specChannelsBuf[CHANNELS][SPECTR_SIZE];	//8КБ
uint16	specChannelIndex[CHANNELS];				//индекс кол-ва принятых пакетов спектра по CAN

tSpectrChannel			SpectrChannel[CHANNELS];						//каналы спектрометра	
tBackgroundSpectr		BackgroundSpectr[CHANNELS];					//Фоновый спектр
*/
void	Devices_SendStateRequest()
{
	//команда сброса для все БД
	BDSP_Reset();
	BDGP_Reset();
	BDMG_Reset();	
}
/*
Фия получения CAN сообщений от всех каналов БДГП-С
*/
void 	Devices_ReceiveMsgCallback(uint16 id,uint8 xhuge *msg)
{
	switch(id)
	{
		case CAN_BDMG_CMD_RX:			BDMG_InsertCmd(msg);	break;
		case CAN_BDMG_DATA_RX:		BDMG_InsertData(msg);	break;
		case CAN_BDGP_CMD_RX:			BDGP_InsertCmd(msg);	break;
//		case CAN_BDGP_DATA_II_RX:	BDGP_InsertData(2,msg);	break;	//приём данных с 1 диапазона, TODO сделать приём I и II диапазонов
//		case CAN_BDGP_DATA_III_RX:	BDGP_InsertData(3,msg);	break;	//приём данных с 1 диапазона, TODO сделать приём I и II диапазонов

		
		//спектрометр данные
		case CAN_BDSP_DATA_CH1_RX:	BDSP_InsertData(0,msg);	break;
		case CAN_BDSP_DATA_CH2_RX:	BDSP_InsertData(1,msg);	break;
		case CAN_BDSP_DATA_CH3_RX:	BDSP_InsertData(2,msg);	break;
		case CAN_BDSP_DATA_CH4_RX:	BDSP_InsertData(3,msg);	break;
		
	//	case CAN_BDSP_EMER_ALL_RX:	BDSP_InsertEmer(msg);		break;
		//спектрометр стабилизация
		case CAN_BDSP_EMER_CH1_RX:	BDSP_InsertCmd(0,msg);	break;
		case CAN_BDSP_EMER_CH2_RX:	BDSP_InsertCmd(1,msg);	break;
		case CAN_BDSP_EMER_CH3_RX:	BDSP_InsertCmd(2,msg);	break;
		case CAN_BDSP_EMER_CH4_RX:	BDSP_InsertCmd(3,msg);	break;
		
	}
}

/*
void	Devices_ReceiveMsgCallback(uint16 id,uint8 xhuge *msg)
{	
	//временные переменные для БДГП-С
 	__IO	uint8		command = 0;
	__IO	uint8		channel = 0;
	__IO	uint16	length = 0;
	__IO	uint16	time = 0;
	
	uint16	*pSpecCopy = 0;
	
	//Обарботка сообщений стабилизации
	if(	id == CAN_BDGPS_CHAN_I_STAB || 
			id == CAN_BDGPS_CHAN_II_STAB || 
			id == CAN_BDGPS_CHAN_III_STAB || 
			id == CAN_BDGPS_CHAN_IV_STAB)
	{
		channel = (uint8)(id & 0x0007) - 1;	//номер обрабатываемого канала
			
		SpectrChannel[channel].Ready = 1;
			
	}
	
	//Обработка получения ответа от БДГП-С
	if( id == CAN_BDGPS_CHAN_I_RESPONSE || id == CAN_BDGPS_CHAN_II_RESPONSE || id == CAN_BDGPS_CHAN_III_RESPONSE || id == CAN_BDGPS_CHAN_IV_RESPONSE )
	{
		
				channel = (uint8)(id & 0x0007) - 1;	//номер обрабатываемого канала
				command = msg[0];										//код команды
				
					
					if(command == 0x07 && msg[2] == 0 && msg[7] == 0)
					{
						time = (msg[4]<<8)|msg[3];			
						length  = (msg[6]<<8)|msg[5]; 

						if(length == 0x400 && time == 0x01)
						{
						
							specChannelIndex[channel] = 0;
							
						}
					}

					else
					{
						specChannelsBuf[channel][specChannelIndex[channel]] 	= (uint16)((msg[1] << 8) | msg[0]);
						specChannelsBuf[channel][specChannelIndex[channel]+1] =  (uint16)((msg[3] << 8) | msg[2]);
						specChannelsBuf[channel][specChannelIndex[channel]+2] =  (uint16)((msg[5] << 8) | msg[4]);
						specChannelsBuf[channel][specChannelIndex[channel]+3] =  (uint16)((msg[7] << 8) | msg[6]);
						

						specChannelIndex[channel]+=4;
					}

					if(specChannelIndex[channel] == 1024)
					{
						
						//Devices_BackgroundAccumulation(specChannelsBuf[channel],channel);
						Devices_SpectrAccumulation(specChannelsBuf[channel],channel);
						
						//IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)((uint8*)(&channel)),1);
						specChannelIndex[channel] = 0;
					}

			
			
	}
}

void 	Devices_SpectrAccumulation(uint16	*specArr,uint8	channel)
{
	uint8	sBufSize = MAX_TIME - 1;
	
	
	
	if(SpectrChannel[channel].Index > sBufSize)
	{
		memcpy(&SpectrChannel[channel].Data[0],&SpectrChannel[channel].Data[1],sizeof(uint16) * SPECTR_SIZE * (sBufSize - 1));
		SpectrChannel[channel].Index-=1;
		
		if(SpectrChannel[channel].Ready == 0)
		{
			SpectrChannel[channel].Ready = 1;
			IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)((uint8*)(&channel)),1);
		}
		
		//IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)((uint8*)(&channel)),1);
	}
	
	memcpy(SpectrChannel[channel].Data[SpectrChannel[channel].Index],specArr,sizeof(uint16) * SPECTR_SIZE);
	SpectrChannel[channel].Index++;

//	IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)((uint8*)(&SpectrChannel[channel].Index)),1);
}
void	Devices_BackgroundAccumulation(uint16 xhuge	*specArr,uint8	channel)
{
	uint16 i = 0;
	float temp = 0,sum = 0;
	__IO float koef = 0;
	uint8	ch = 0;
	
	if(BackgroundSpectr[channel].Index < BACKGROUND_TIME)
	{
		sum = 0;
		
		for(i = 0;i<SPECTR_SIZE;i++){

			BackgroundSpectr[channel].Data[i] = BackgroundSpectr[channel].Data[i] + (specArr[i] * 1.0);	//копим счёт
		
			sum+=BackgroundSpectr[channel].Data[i];
		}
		
		BackgroundSpectr[channel].Index++;
		//IMDB_SendPacket(IMDB_OUTSIDE_DOSE_RATE,(uint8*)((uint32*)(&sum)),4);
	}
	//если завершили накопление спектра
	if((BackgroundSpectr[channel].Ready == 0) && (BackgroundSpectr[channel].Index == BACKGROUND_TIME) )
	{
		temp = 0;

		//завершили накопление фона
		BackgroundSpectr[channel].Ready = 1;
		
		
		for(i = 0;i<SPECTR_SIZE;i++){

			
			BackgroundSpectr[channel].Data[i] = BackgroundSpectr[channel].Data[i] * ( 1.0f / (float)BACKGROUND_TIME);
			temp+=BackgroundSpectr[channel].Data[i];
		}

		IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)((uint8*)(&channel)),1);
		IMDB_SendPacket(IMDB_OUTSIDE_DOSE_RATE,(uint8*)((uint32*)(&temp)),4);
	}
}

void	Devices_ClearBackgroundData()
{
	uint8	i = 0;
	uint16 j = 0;
	
	for(i = 0;i<CHANNELS;i++)
	{
	
		BackgroundSpectr[i].Index = 0;
		BackgroundSpectr[i].Ready = 0;
		for(j = 0;j<SPECTR_SIZE;j++)
				BackgroundSpectr[i].Data[j] = 0.0f;
		
	}
	xmemset(specChannelIndex,0,sizeof(uint16) * CHANNELS);

}

uint16 *	Devices_GetCurrentSpectr(uint8 channel)
{
	uint16	i,j = 0;
	uint16	*pSumSpectr;
	
	pSumSpectr = (uint16*)malloc(sizeof(uint16) * SPECTR_SIZE);
	xmemset(pSumSpectr,0,sizeof(uint16) * SPECTR_SIZE);
	
	for(i = 0;i<MAX_TIME;i++)
	{
		for(j = 0;j<SPECTR_SIZE;j++)
			pSumSpectr[j] += SpectrChannel[channel].Data[i][j]; //specChannels[i][Channel][j];
			
	}
	for(i = 0;i<SPECTR_SIZE;i++)
		pSumSpectr[j] = pSumSpectr[j] * (1.0 / MAX_TIME);
		
	
	
	return pSumSpectr;
}

uint16 Devices_Probe()
{
	uint16 i = 0;
	uint16 result = 0;

		for(i = 0;i<SPECTR_SIZE;i++)
		{

		//	result += specChannels[0][i];
		}
	
	return result;
}*/
/*
*
*
	Команды для работы с БДГП-С
*
*
*/


