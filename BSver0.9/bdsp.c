#include "bdsp.h"
#include "can.h"
#include "process.h"
#include "protocol.h"
#include "settings.h"
#include "system.h"

#include "bdgp.h"

const uint8 mResetBDSP[] = {0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8	mStartBDSP[] = {0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8	mStopBDSP[] = {0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

const uint8 mSpectrSize1024[] = {0x02,0x05,0x00,0x01,0x00,0x00,0x00,0x00};	//128
const uint8 mSpectrSize128[] = {0x02,0x05,0x00,0x04,0x00,0x00,0x00,0x00};	//128

static uint8	channelStartCnt = 0;	//счётчик включаемых каналов
static uint16	radiationMaximum = 0;			//угол на максимум 


static  tBDSPData						BDSPData[CHANNELS];	//каналы спектрометра
static	eSpecMode						BDPSChannelSize;
static	eSpecAccumMode			BDSPAccumMode;
static	eSpecMeasureNumber	BDSPMeasureNumber;	
static	tBDSPParametrs			BDSPParametrs;

void	BDSP_CalculateIntegral()
{
	uint16 	i,j,k;
	uint32	integralSum = 0;
	
	//считаем интеграл для каждого канала
	for(k = 0;k<CHANNELS;k++)
	{
		BDSPData[k].Integral = 0;
		integralSum = 0;
		
		for(i = 0;i</*MainSettings.accumulationTime*/QUERY_SIZE;i++)
			for(j = 0;j<BDPSChannelSize;j++)
				integralSum += BDSPData[k].Data[i][j];
		
		BDSPData[k].Integral = integralSum ;// MainSettings.accumulationTime;
	}

}

uint16	BDSP_MinCounter(uint8	*minChannelIndex)
{
	uint8 i = 0;
	uint32	result = 0xFFFFFFFF;
		
		for(i = 0;i<CHANNELS;i++)
		{
			if(BDSPData[i].Integral < result){
				result = BDSPData[i].Integral;
				*minChannelIndex = i;}
		}
		
	return (uint16)result;
}

uint16 BDSP_MaxCounter(uint8	*maxChannelIndex)
{
	uint8 i = 0;
	uint32	result = 0;
	
		for(i = 0;i<CHANNELS;i++)
		{
			if(BDSPData[i].Integral > result){
				result = BDSPData[i].Integral;
				*maxChannelIndex = i;}
		}

	return (uint16)result;
}

void BDSP_Reset()
{
	uint8	i = 0;
	
	channelStartCnt = 0;
	
	BDPSChannelSize = SIZE_1024;	//размерность спектра - 1024 канала
	BDSPAccumMode = ACCUM_ONLY;
	BDSPMeasureNumber = MEASURE_ONE;
	
	for(i = 0;i<CHANNELS;i++)
	{
		memset(BDSPData[i].Data,0,sizeof(uint16) * QUERY_SIZE * SPECTR_SIZE);
		memset(BDSPData[i].InputData,0,sizeof(uint16) * SPECTR_SIZE * 2);
		memset(BDSPData[i].BackgroundData,0,sizeof(float) * SPECTR_SIZE);
		memset(BDSPData[i].BackgroundData128,0,sizeof(float) * SPECTR_SIZE_128);
		memset(BDSPData[i].AccumSpectrData,0,sizeof(float) * SPECTR_SIZE);
		
		BDSPData[i].DataCounter = 0;
		BDSPData[i].Serial = 0;
		BDSPData[i].Ready = 0;
		BDSPData[i].QueryIndex = 0;
		BDSPData[i].Background = 0;
		BDSPData[i].BackgroundIndex = 0;
		BDSPData[i].AccumSpectrIndex = 0;
		BDSPData[i].ActiveBuffer = 0;
		BDSPData[i].ChangeBufferEvent = 0;
		BDSPData[i].Dose = 0;
		BDSPData[i].AccumSpectr = 0;
	}
	
	memset((uint8*)&BDSPParametrs,0,sizeof(tBDSPParametrs));
	
	CAN_SendMessage(CAN_BDSP_DATA_TX,mResetBDSP,8);
}
void BDSP_StopChannel(uint8	Channel)
{
	CAN_SendMessage(CAN_BDSP_DATA_TX + Channel + 1,mStopBDSP,8);
}

void BDSP_Start()
{
	CAN_SendMessage(CAN_BDSP_DATA_TX + 1,mStartBDSP,8);
	CAN_SendMessage(CAN_BDSP_DATA_TX + 2,mStartBDSP,8);
	CAN_SendMessage(CAN_BDSP_DATA_TX + 3,mStartBDSP,8);
	CAN_SendMessage(CAN_BDSP_DATA_TX + 4,mStartBDSP,8);

}

void BDSP_Stop()
{
	/*
	BDSP_StopChannel(0);
	BDSP_StopChannel(1);
	BDSP_StopChannel(2);
	BDSP_StopChannel(3);
	*/
	CAN_SendMessage(CAN_BDSP_DATA_TX + 1,mStopBDSP,8);
	CAN_SendMessage(CAN_BDSP_DATA_TX + 2,mStopBDSP,8);
	CAN_SendMessage(CAN_BDSP_DATA_TX + 3,mStopBDSP,8);
	CAN_SendMessage(CAN_BDSP_DATA_TX + 4,mStopBDSP,8);	
}
void BDSP_SetSpectrSize(eSpecMode	eMode)
{
	switch(eMode)
	{
		case SIZE_1024:
		{
			CAN_SendMessage(CAN_BDSP_DATA_TX + 1,mSpectrSize1024,8);
			CAN_SendMessage(CAN_BDSP_DATA_TX + 2,mSpectrSize1024,8);
			CAN_SendMessage(CAN_BDSP_DATA_TX + 3,mSpectrSize1024,8);
			CAN_SendMessage(CAN_BDSP_DATA_TX + 4,mSpectrSize1024,8);	

				BDPSChannelSize = SIZE_1024;
		}break;
		
		case SIZE_128:
		{
			CAN_SendMessage(CAN_BDSP_DATA_TX + 1,mSpectrSize128,8);
			CAN_SendMessage(CAN_BDSP_DATA_TX + 2,mSpectrSize128,8);
			CAN_SendMessage(CAN_BDSP_DATA_TX + 3,mSpectrSize128,8);
			CAN_SendMessage(CAN_BDSP_DATA_TX + 4,mSpectrSize128,8);
			
				BDPSChannelSize = SIZE_128;
		}break;
	}
}

void	BDSP_InsertData(uint8	Channel,uint8	*pData)
{
	float	sum = 0,i = 0;
	uint16 time = 0;
	uint16 length = 0;
	
	uint8	activeBuffer = BDSPData[Channel].ActiveBuffer;	//получаем индекс активного буфера
	
					//проверка на задание логического номера
					if(Process_GetOperationMode() == IMDB_INITIALIZATION)
					{
						if(pData[0] == 0x00 && pData[1] == Channel + 1){
								BDSPData[Channel].Ready = 1;
									channelStartCnt++;
								
									if(channelStartCnt == CHANNELS)
										Process_BDPSStatus(DEVICE_WAITING);	//ожидаем стабы
						}
					}
	
					if(pData[0] == 0x07 && pData[2] == 0 && pData[7] == 0)
					{
						time = (pData[4]<<8)|pData[3];			
						length  = (pData[6]<<8)|pData[5]; 

						if(length == BDPSChannelSize && time == 0x01)
						{
						
							BDSPData[Channel].DataCounter = 0;
							BDSPData[Channel].ChangeBufferEvent = 0;
						}
					}

					else
					{
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter ] = (uint16)((pData[1] << 8) | pData[0]);
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter + 1 ] = (uint16)((pData[3] << 8) | pData[2]);
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter + 2 ] = (uint16)((pData[5] << 8) | pData[4]);
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter + 3 ] = (uint16)((pData[7] << 8) | pData[6]);
						
						/*
						specChannelsBuf[channel][specChannelIndex[channel]] 	= (uint16)((msg[1] << 8) | msg[0]);
						specChannelsBuf[channel][specChannelIndex[channel]+1] =  (uint16)((msg[3] << 8) | msg[2]);
						specChannelsBuf[channel][specChannelIndex[channel]+2] =  (uint16)((msg[5] << 8) | msg[4]);
						specChannelsBuf[channel][specChannelIndex[channel]+3] =  (uint16)((msg[7] << 8) | msg[6]);
						
						
						specChannelIndex[channel]+=4;*/
						BDSPData[Channel].DataCounter += 4;
					}

					if(BDSPData[Channel].DataCounter == BDPSChannelSize)
					{
						//1. Проверяем, накоплен ли фон
						//2. Если да, то заполняем очередь спектров
						//3. В фии проверки заполнения очереди спектров проверяем, заполнена ли очередь и выполняем рассчёты
							if(BDSPData[Channel].ActiveBuffer == 1)
								BDSPData[Channel].ActiveBuffer = 0;
							else
								BDSPData[Channel].ActiveBuffer = 1;
							
							BDSPData[Channel].ChangeBufferEvent = 1;	//ставим флаг сработки события смены буфера
						/*
						//проверили, если накопили фон
						if( (BDSPData[0].Background == 1) && (BDSPData[1].Background == 1) && (BDSPData[2].Background == 1) && (BDSPData[3].Background == 1) )
						{
							
							//BDSP_InsertInQuery(BDSPData[Channel].InputData,Channel);
						}
						else
						{
							//BDSP_BackgroundAccumulation(Channel,BDSPData[Channel].InputData);
						}*/

						
						BDSPData[Channel].DataCounter = 0;
					}
}

void	BDSP_InsertCmd(uint8	Channel,uint8	*pData)	//получили сообщение по 00X, аппаратный номер канала - 1 от программного
{
	uint8	Serial = 0;

	
	if(Channel == 0)	//получили аварийное сообщение от всех БД
	{
		//приняли команду с серийным номером
		if(pData[0] == 0x01 && pData[1] == 0x01 && pData[2] == 0x00 & pData[3] == 0x00)
		{
				Serial = pData[4];	//получаем текущий серийный номер
			
			pData[0] = 0x00;
			pData[1] = Serial;	//(channelStartCnt+1);//присваиваем его в ответное сообщение и посылаем его на блок
			
			CAN_SendMessage(CAN_BDSP_DATA_TX,pData,8);
			//channelStartCnt++;
		}
		/*if(pData[0] == 0x01 && pData[1] == 0x01)//получили сообщение от БД
		{
			BDSPData[channelStartCnt].Ready = 1;
			channelStartCnt++;
		}
		if(channelStartCnt == CHANNELS)
			Process_BDPSStatus(DEVICE_WAITING);	//ожидаем стабы*/
		
		
	}
	else	// сообщение успешной стабилизации
	{
		
		if(pData[0] == 0x02 && pData[1] == 0x01)
		{
			
			BDSPData[Channel - 1].Ready = 2;
			

			
			
			
			
			//CAN_SendMessageIT(CAN_BDSP_DATA_TX + Channel,mSpectrSize1024,8);
			//System_Delay(50);
		
			if(BDSPData[0].Ready == 2 && BDSPData[1].Ready == 2 && BDSPData[2].Ready == 2 && BDSPData[3].Ready == 2)
				Process_BDPSStatus(DEVICE_READY);
		}
	}
}
void	BDSP_SetParametrs()
{
		uint8	Channel = 0;
		uint8	MsgData[8];

	for(Channel = 1;Channel<5;Channel++){
	
				MsgData[0] = 0x02;
				MsgData[1] = 0x00;
				MsgData[2] = 0x01;
				MsgData[3] = 0xD0;
				MsgData[4] = 0x07;
				MsgData[5] = 0x00;
				MsgData[6] = 0x00;
				MsgData[7] = 0x00;
			
			
			CAN_SendMessage(CAN_BDSP_DATA_TX + Channel,MsgData,8);
			
				MsgData[0] = 0x02;
				MsgData[1] = 0x01;
				MsgData[2] = 0x01;
				MsgData[3] = 0x34;
				MsgData[4] = 0x07;
				MsgData[5] = 0x00;
				MsgData[6] = 0x00;
				MsgData[7] = 0x00;
			
			
			CAN_SendMessage(CAN_BDSP_DATA_TX + Channel,MsgData,8);
			
				MsgData[0] = 0x02;
				MsgData[1] = 0x02;
				MsgData[2] = 0x01;
				MsgData[3] = 0xE8;
				MsgData[4] = 0x03;
				MsgData[5] = 0x00;
				MsgData[6] = 0x00;
				MsgData[7] = 0x00;
			
			
			CAN_SendMessage(CAN_BDSP_DATA_TX + Channel,MsgData,8);
		}
}


void BDSP_BackgroundAccumulation(uint8	Channel,uint16	*pData)
{
	uint16	i = 0,j = 0,sI = 0;
	
	float temp = 0;
	float foobarMD = 0.667f;
	

	if(BDSPData[Channel].BackgroundIndex < BACKGROUND_TIME){
		for(i = 0;i<SPECTR_SIZE;i++){
			BDSPData[Channel].BackgroundData[i] = BDSPData[Channel].BackgroundData[i] + (pData[i] * 1.0);	//копим счёт
		}
		BDSPData[Channel].BackgroundIndex++;
	}
	
	if((BDSPData[Channel].Background == 0) && (BDSPData[Channel].BackgroundIndex == BACKGROUND_TIME) ){
		//завершили накопление фона
		BDSPData[Channel].Background = 1;

			BDSP_StopChannel(Channel);//останавливаем канал		
				
				System_Delay(300000);
		
			BDSP_SetSpectrSize(SIZE_128);//ставим размер спектра 128 каналов
		//проверяем, накопили ли мы фон у всех каналов
		if((BDSPData[0].Background == 1) && (BDSPData[1].Background == 1) && (BDSPData[2].Background == 1) && (BDSPData[3].Background == 1))
		{
			
			IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)&foobarMD,sizeof(float));//отсылаем успех в накоплении фона
			Process_SetStatus(IMDB_FULLTIME_WORK);//переключаемся в режим штатной работы
			Process_IMDBStart();	//запускаем комплекс	
		}
		//получаем скорость счёта для 128 канального фонового спектра
			sI = 0;
		for(i = 0;i<SPECTR_SIZE_128;i++){
			BDSPData[Channel].BackgroundData128[i] = (BDSPData[Channel].BackgroundData[sI] 	 	 + BDSPData[Channel].BackgroundData[sI + 1] + 
																							  BDSPData[Channel].BackgroundData[sI + 2] + BDSPData[Channel].BackgroundData[sI + 3] + 
																							  BDSPData[Channel].BackgroundData[sI + 4] + BDSPData[Channel].BackgroundData[sI + 5] + 
																							  BDSPData[Channel].BackgroundData[sI + 6] + BDSPData[Channel].BackgroundData[sI + 7]);
			sI+=8;
		}
		temp = 0;
		
		for(i = 0;i<SPECTR_SIZE_128;i++){
			BDSPData[Channel].BackgroundData128[i] = BDSPData[Channel].BackgroundData128[i] * BACKGROUND_KOEF;//( 1.0f / (float)BACKGROUND_TIME);
			
			temp += BDSPData[Channel].BackgroundData128[i];
		}
		//IMDB_SendPacket(IMDB_INSIDE_DOSE,(uint8*)((uint32*)&temp),sizeof(float));
		
		temp = 0;
		
		//получаем скорость счёта 
		for(i = 0;i<SPECTR_SIZE;i++){
			BDSPData[Channel].BackgroundData[i] = BDSPData[Channel].BackgroundData[i] * BACKGROUND_KOEF;//( 1.0f / (float)BACKGROUND_TIME);
			
			temp += BDSPData[Channel].BackgroundData[i];
		}
		//IMDB_SendPacket(IMDB_INSIDE_DOSE,(uint8*)((uint32*)&temp),sizeof(float));
		
		
	}
}
void BDSP_ResetBackgroundData()
{
	uint8	i = 0;
	
	//останавливаем спектрометр
	BDSP_Stop();
	BDSP_SetSpectrSize(SIZE_1024);
	
		for(i = 0;i<CHANNELS;i++){
			BDSPData[i].BackgroundIndex = 0;
			BDSPData[i].Background = 0;
			
			memset(BDSPData[i].BackgroundData,0,sizeof(float) * SPECTR_SIZE);
			memset(BDSPData[i].BackgroundData128,0,sizeof(float) * SPECTR_SIZE_128);
			
		}
	BDSP_Start();
}

void BDSP_InsertInQuery(uint16 xhuge	*pData, uint8	Channel)//фия получения данных в очередь
{
	uint16 i = 0;
	uint16	sBufSize = QUERY_SIZE;//MainSettings.accumulationTime - 1;
	float temp = 0;
	
	if(BDSPData[Channel].QueryIndex > sBufSize)
	{
		memcpy(&BDSPData[Channel].Data[0],&BDSPData[Channel].Data[1],sizeof(uint16) * SPECTR_SIZE * (sBufSize ));
		BDSPData[Channel].QueryIndex-=1;

	}
	
	memcpy(BDSPData[Channel].Data[BDSPData[Channel].QueryIndex],pData,sizeof(uint16) * SPECTR_SIZE);
	BDSPData[Channel].QueryIndex++;
	/*for(i = 0;i<SPECTR_SIZE;i++)
		temp+=BDSPData[Channel].Data[BDSPData[Channel].QueryIndex][i];
	
	IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)((uint8*)(&Channel)),1);
	IMDB_SendPacket(IMDB_INSIDE_DOSE,(uint8*)((uint32*)&temp),sizeof(float));
	*/
	
	
	//IMDB_SendPacket(IMDB_16BITWORD,(uint8*)&BDSPData[Channel].QueryIndex,sizeof(uint16));
	//IMDB_SendPacket(IMDB_16BITWORD,(uint8*)&Channel,sizeof(uint16));
	
}
/*
float	BDSP_BackgroundExcess()	//ф-ия рассчёта превышения по фону
{
	float fQuantile = 0;
	
	uint8	maxIntegralIndex = 0;
	
	uint16	accTime = MainSettings.accumulationTime;
	uint16 	i = 0,j = 0;
	
		BDSP_CalculateIntegral();
		BDSP_MaxCounter(&maxIntegralIndex);
	
	BDSPData[maxIntegralIndex].ppMax = 0;
	BDSPData[maxIntegralIndex].ppBackgroundMax = 0;
	
	for(i = 0;i<accTime;i++){
		for(j = 0;j<SPECTR_SIZE;j++){
			BDSPData[maxIntegralIndex].ppMax += BDSPData[maxIntegralIndex].Data[i][j];
			
			if(i == 0){
				BDSPData[maxIntegralIndex].ppBackgroundMax += BDSPData[maxIntegralIndex].BackgroundData[j];
			}
		}
	}
	
	BDSPData[maxIntegralIndex].ppBackgroundMax = BDSPData[maxIntegralIndex].ppBackgroundMax * accTime;
	
	fQuantile = (BDSPData[maxIntegralIndex].ppMax - BDSPData[maxIntegralIndex].ppBackgroundMax) / (sqrt(BDSPData[maxIntegralIndex].ppBackgroundMax) + 1.0f);
	
	
	
	return fQuantile;
}
*/
uint8 BDSP_BackgroundDetection()	//ф-ия идентификации и превышения МД на фоне
{
	uint8 i = 0;
	uint8	maxIntegralIndex = 0;
	uint8	backgroundCalcFlag = 0;	//рассчёт счёта из скорости счёта фона
	uint16	accTime = QUERY_SIZE;//MainSettings.accumulationTime;
	uint16	startInd = 0, endInd = 0,j = 0,  k = 0;
	
	uint16	maxIntegral = 0;
	

	uint8 iResult = 0;
	
	float fQuantile = 0;
	
	BDSP_CalculateIntegral();
	BDSP_MaxCounter(&maxIntegralIndex);
	
	for(j = 1;j<123;j++){
			BDSPData[maxIntegralIndex].ppMax = 0;
			BDSPData[maxIntegralIndex].ppBackgroundMax = 0;
			
				startInd = 118 - 0.957f * j;
				endInd = 126 - j;
		
			for(i = 0;i<accTime;i++){
					for(k = startInd;k<endInd;k++){
					BDSPData[maxIntegralIndex].ppMax += BDSPData[maxIntegralIndex].Data[i][k];
					if(i == 0)
						BDSPData[maxIntegralIndex].ppBackgroundMax += BDSPData[maxIntegralIndex].BackgroundData128[k];					
				}
			}
			
			BDSPData[maxIntegralIndex].ppBackgroundMax = BDSPData[maxIntegralIndex].ppBackgroundMax * accTime;
			fQuantile = (BDSPData[maxIntegralIndex].ppMax - BDSPData[maxIntegralIndex].ppBackgroundMax) / (sqrt(BDSPData[maxIntegralIndex].ppBackgroundMax) + 1.0f);
			
			if(fQuantile > MainSettings.limitDetect[0]){
					return 1;
		}
	}
	return 0;
}
uint8	BDSP_Identification(uint16	*ePhotoPeak, uint16	*iSpdCounter)//ф-ия идентификации в спектрометрическом режиме
{
	uint8		iResult = 0xFF;
	uint16	maxIntegralIndex = 0;
	uint16	accTime = MainSettings.accumulationTime;
	uint16 	i = 0, j = 0, k = 0;
	uint16	startInd = 0, endInd = 0;
	uint32	maxIntegralValue = 0;
	
	float  fQuantile = 0;
	uint16 ePhoto = 0;		//энергия фотопика
	uint16 spCounter = 0;	//скорость счёта в окне
	//находим спектр с максимальным интегралом
	for(i = 0;i<CHANNELS;i++){
		BDSPData[i].Integral = 0;
			for(j = 0;j<SPECTR_SIZE;j++){
				BDSPData[i].Integral += BDSPData[i].AccumSpectrData[j];
			}
			if(BDSPData[i].Integral > maxIntegralValue){
				maxIntegralValue = BDSPData[i].Integral;
				maxIntegralIndex = i;
			}
	}
	for(j = 1;j<984;j++){
			//считаем квантиль превышения по фону
			BDSPData[maxIntegralIndex].ppMax = 0;
			BDSPData[maxIntegralIndex].ppBackgroundMax = 0;
		
			startInd = 946 - 0.957f * j;
			endInd = 1004 - j;
		
			for(k = startInd;k<endInd;k++){
					BDSPData[maxIntegralIndex].ppMax += BDSPData[maxIntegralIndex].AccumSpectrData[k];
					BDSPData[maxIntegralIndex].ppBackgroundMax += BDSPData[maxIntegralIndex].BackgroundData[k];					
			}
				//теперь высчитываем квантиль
				BDSPData[maxIntegralIndex].ppBackgroundMax = BDSPData[maxIntegralIndex].ppBackgroundMax * accTime;
				fQuantile = (BDSPData[maxIntegralIndex].ppMax - BDSPData[maxIntegralIndex].ppBackgroundMax) / (sqrt(BDSPData[maxIntegralIndex].ppBackgroundMax) + 1.0f);
			
			if(fQuantile > MainSettings.limitDetect[0]){
				if(j >= 746 && j <= 775){
					return iResult = 1;
				}
				else if(j >= 696 && j <= 726){
					return iResult = 2;
				}
				else if(j >= 508 && j <= 546){
					return iResult = 3;
				}
				else if(j >= 852 && j <= 875){
					return iResult = 4;
				}
				else{
					return iResult = 0;
				}
			}
	}
	//получаем энергию фотопика
	*ePhotoPeak = 2925 - 2.9355f * iResult;
	*iSpdCounter = BDSPData[maxIntegralIndex].ppMax * (1.0f / accTime);
	
	return iResult;
}
void BDSP_StartSpectrAccumulation(eSpecAccumMode Mode)
{
	/*
	0. Остановили спектрометр
	1. Произвели очистку очереди данных
	1. Переводим спектрометр в режим 1024 канал
	2. Запускаем спектрометр
	*/
	BDSP_Stop();
	BDSP_ClearData();
	
		BDSPAccumMode = Mode;	//запоминаем режим накопления
		
		Process_SetStatus(IMDB_SPECTR_ACCUMULATION);
	
	BDSP_SetSpectrSize(SIZE_1024);
	BDSP_Start();
	
}
void	BDSP_SpectrAccumulation(uint8	Channel,uint16	*pData)
{
	uint16 i = 0;
	uint8		nuclideIndex = 0;

	
	sSpecModeData	currentData;
	
	
	if(BDSPData[Channel].AccumSpectrIndex < MainSettings.accumulationTime){
		
		for(i = 0;i<SPECTR_SIZE;i++){
			BDSPData[Channel].AccumSpectrData[i] = BDSPData[Channel].AccumSpectrData[i] + pData[i];	//копим счёт
		}
		BDSPData[Channel].AccumSpectrIndex++;
	}
	
	if(BDSPData[Channel].AccumSpectr == 0 && BDSPData[Channel].AccumSpectrIndex >= MainSettings.accumulationTime)
	{
		BDSPData[Channel].AccumSpectr = 1;
		
			
			
				
		
			if(BDSPAccumMode == ACCUM_ONLY){
				
					//BDSP_StopChannel(Channel);//останавливаем канал		
					IMDB_SendPacket(METROLOGY_SEND_ACCUMSPECTR_SUCCESS,(uint8*)&Channel,sizeof(uint8));	//отправка сообщения об успешном накоплении спектра
			}
			
			if( BDSPData[0].AccumSpectr == 1 && 
					BDSPData[1].AccumSpectr == 1 &&
					BDSPData[2].AccumSpectr == 1 &&
					BDSPData[3].AccumSpectr == 1){

						
						
						BDSP_SetSpectrSize(SIZE_128);//ставим размер спектра 128 каналов
						
						
							if(BDSPAccumMode == ACCUM_AND_MEASURE){
								//TODO рассчёт данных для отправки в сообщении об окончании измерения. На данный момент установлена заглушка
								currentData.fMaxAngle = 1.34f;
								currentData.fDoseRate = 0.076f;

								nuclideIndex = BDSP_Identification(&currentData.ePhotoPeak,&currentData.iSpdCounter);
							
								currentData.iSpdCounter = nuclideIndex;	//TODO
							
								IMDB_SendPacket(IMDB_SEND_SPECTR_ACCUM_SUCCESS,(uint8*)&currentData,sizeof(sSpecModeData));
							}
							
							
						Process_SetStatus(IMDB_FULLTIME_WORK);
						//IMDB_SendState();
				
			}
	}
	
	
}

void BDSP_ClearData()
{
	uint8	i = 0;
	
	for(i = 0;i<CHANNELS;i++){
		memset(BDSPData[i].InputData,0,sizeof(uint16) * SPECTR_SIZE * 2);
		memset(BDSPData[i].WorkData,0,sizeof(uint16) * SPECTR_SIZE);
		memset(BDSPData[i].AccumSpectrData,0,sizeof(float) * SPECTR_SIZE);
		
		BDSPData[i].ActiveBuffer = 0;
		BDSPData[i].AccumSpectrIndex = 0;
		BDSPData[i].AccumSpectr = 0;
	}
}

void BDPS_ClearQuery()
{
	uint8	i = 0;
	
		for(i = 0;i<CHANNELS;i++){
			memset(BDSPData[i].Data,0,sizeof(uint16) * SPECTR_SIZE * QUERY_SIZE);
			BDSPData[i].QueryIndex = 0;
		}
}


uint8	*BDSP_GetCurrentSpectr()
{
	uint8	channel = 0;
	
		BDSP_CalculateIntegral();
		BDSP_MaxCounter(&channel);	//получаем индекс канала с макс. счётом
		
	return BDSP_GetChannelSpectr(channel);
}
uint8	*BDSP_GetChannelSpectr(uint8	channel)
{
	
	
	return (uint8*)&BDSPData[channel].WorkData[0];
}
uint8	*BDSP_GetAccumulationSpectr(uint8	Channel)
{
	return (uint8*)&BDSPData[Channel].AccumSpectrData[0];
}


float	*BDSP_GetAngularDiagram()
{
	float	*pAngleDiagramm;
	
	uint16 cntMin = BDSP_MinCounter(0);
	uint16 cntMax = BDSP_MaxCounter(0);
	
	float	sensorArray[12];
	
	BDSP_CalculateIntegral();
	
		memset(sensorArray,0,sizeof(float) * 12);
	
		sensorArray[0] = (uint16)BDSPData[0].Integral;
		sensorArray[4] = (uint16)BDSPData[1].Integral;
		sensorArray[7] = (uint16)BDSPData[2].Integral;
		sensorArray[9] = (uint16)BDSPData[3].Integral;
	
	
	
		pAngleDiagramm	= BDGP_DirectionDiagramm(sensorArray, 12, cntMin,cntMax,&radiationMaximum,SPEC_MODE);	
	
	return pAngleDiagramm;
}
uint16	BDSP_GetMaximumAngle()
{
	return radiationMaximum;
}
float		BDSP_GetDose()
{
	return 0.002f;//BDSPData[0].Dose;
}

uint8	BDSP_GetBackgroundReady()
{
	if( (BDSPData[0].Background == 1) && (BDSPData[1].Background == 1) && (BDSPData[2].Background == 1) && (BDSPData[3].Background == 1) ){
			return 1;
	}
	else
		return 0;
}


void	BDSP_Process()
{
	uint8 i = 0;
	
	/*switch(Process_GetOperationMode())
	{
		case IMDB_FULLTIME_WORK:
		{*/
			for(i = 0;i<CHANNELS;i++)
			{
				//зарегистрировали процесс смены буфера
				if(BDSPData[i].ChangeBufferEvent){
					BDSPData[i].ChangeBufferEvent = 0;
					
					if(BDSPData[i].ActiveBuffer == 1)
						memcpy(BDSPData[i].WorkData,BDSPData[i].InputData[0],sizeof(uint16) * SPECTR_SIZE);
					else
						memcpy(BDSPData[i].WorkData,BDSPData[i].InputData[1],sizeof(uint16) * SPECTR_SIZE);	
					
					/*
					Костыль, из за наводки затираем первые 48 каналов
					*/
					if(BDPSChannelSize == SIZE_128)
						memset(&BDSPData[i].WorkData[0],0,sizeof(uint16) * 6);
					else
						memset(&BDSPData[i].WorkData[0],0,sizeof(uint16) * 48);
					
					
					switch(Process_GetOperationMode())
					{
						case IMDB_FULLTIME_WORK:{
							//проверяем, накоплен ли фон у всех каналов
							if( (BDSPData[0].Background == 1) && (BDSPData[1].Background == 1) && (BDSPData[2].Background == 1) && (BDSPData[3].Background == 1) )	{	
								BDSP_InsertInQuery(BDSPData[i].WorkData,i);
							}
							/*
								TODO Добавить проверку на режим накопления фона 
							
							else
								BDSP_BackgroundAccumulation(i,BDSPData[i].WorkData);
							*/
						}break;
						
						case IMDB_BACKGROUND_ACCUMULATION:{
							//если фон текущего канала не накоплен
							if(BDSPData[i].Background == 0)
								BDSP_BackgroundAccumulation(i,BDSPData[i].WorkData);//записываем фон
						}break;
						
						case IMDB_SPECTR_ACCUMULATION:{
							BDSP_SpectrAccumulation(i,BDSPData[i].WorkData);
						}break;
					}
				}
			}
		/*}break;
		
		case IMDB_SPECTR_ACCUMULATION:
		{
			BDSP_SpectrAccumulation(i,BDSPData[i].WorkData);
		}break;
	}*/
}
uint8	*BDSP_GetParametrs()
{
	return (uint8*)&BDSPParametrs;
}
