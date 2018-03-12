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

const uint8	mSpectrDoseRateOn[] = {0x02,0x07,0x00,0x01,0x00,0x00,0x00,0x00};

const uint8	mSpectrGetAlignFctr[] = {0x04,0x08,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8	mSpectrGetDeadTime[] = {0x04,0x09,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8	mSpectrGetCorrFctr[] = {0x04,0x10,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8 mSpectrStartCalibration[] = {0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static uint8	NuclideDetection = 0;
static uint8	NuclideIdentification = 0;
static uint8	channelStartCnt = 0;	//счётчик включаемых каналов
static uint16	radiationMaximum = 0;			//угол на максимум 


static  tBDSPData						BDSPData[CHANNELS];	//каналы спектрометра
static	eSpecMode						BDPSChannelSize;
static 	sSpecModeData				MeasureInPntData;
static	eSpecAccumMode			BDSPAccumMode;

static	tBDSPParametrs			near BDSPParametrs;

static	float								fQuantileArray[SPECTR_SIZE];
static 	float								fAccumAverageDoseRate;
static uint8		rWritingParametrRequest;
static 	float								fBackgroundIntergal;	//интеграл по фону

static uint16								iJIndexNuclideIdentification;	//индекс окна нуклида при измерении в точке

static float								fpBDSPAngleDiagramm[48];

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

float	BDSP_CalculateQuerySum(uint8	Channel)
{
	uint16	i = 0,j = 0;
	float Result = 0;
	
	for(i = 0;i<QUERY_SIZE;i++)
		for(j = 0;j<SPECTR_SIZE_128;j++){
			Result += BDSPData[Channel].Data[i][j];
		}
	/*
	if(BDSPData[Channel].QueryIndex > 0)
			Result = Result / BDSPData[Channel].QueryIndex;
		*/
	return Result;
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
	NuclideDetection = 0;
	NuclideIdentification = 0;
	
	fAccumAverageDoseRate = 0;
	fBackgroundIntergal = 0;
	
	iJIndexNuclideIdentification = 0;
		
	BDPSChannelSize = SIZE_1024;	//размерность спектра - 1024 канала
	BDSPAccumMode = ACCUM_ONLY;

	
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
		BDSPData[i].Detected = 0;
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
	memset(fpBDSPAngleDiagramm,0,sizeof(float) * 48);
	memset((uint8*)&MeasureInPntData,0,sizeof(sSpecModeData));
	
	
	rWritingParametrRequest = 0;	//флаг запроса на запись параметров
	
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
void BDSP_SetSpectrChannelSize(eSpecMode eMode,uint8	Channel)
{
	

	
	switch(eMode)
	{
		case SIZE_1024:
		{
			CAN_SendMessage(CAN_BDSP_DATA_TX + Channel,mSpectrSize1024,8);
		}break;
		case SIZE_128:
		{
			CAN_SendMessage(CAN_BDSP_DATA_TX + Channel,mSpectrSize128,8);
		}break;
	}
	
}
/*
функция очистки текущих данных, полученных от спектрометра
*/
void	BDSP_ClearWorkData()
{
	uint8	i = 0;
	//очищаем рабочий массив с данными спектров
	for(i = 0;i<CHANNELS;i++)
		memset(BDSPData[i].WorkData,0,sizeof(float) * SPECTR_SIZE);
}

void BDSP_SetSpectrSize(eSpecMode	eMode)
{
	BDSP_SetSpectrChannelSize(eMode,1);
	BDSP_SetSpectrChannelSize(eMode,2);
	BDSP_SetSpectrChannelSize(eMode,3);
	BDSP_SetSpectrChannelSize(eMode,4);
	
	BDPSChannelSize = eMode;

}


void	BDSP_InsertData(uint8	Channel,uint8	*pData)
{
	
	
	float incorrDoseRate = 0;
	
	float	sum = 0,i = 0;
	uint16 time = 0;
	uint16 length = 0;
	
	uint8	activeBuffer = BDSPData[Channel].ActiveBuffer;	//получаем индекс активного буфера
	uint8	*pParam = 0;
			
					//проверка команд калибровки
					if( (Process_GetOperationMode() == IMDB_CALIBRATION_I)|
							(Process_GetOperationMode() == IMDB_CALIBRATION_II)|
							(Process_GetOperationMode() == IMDB_CALIBRATION_III)|
							(Process_GetOperationMode() == IMDB_CALIBRATION_IV)){
								
								if(pData[0] == 0x06){
									if(pData[1] > 1 && pData[1] < 7){
										IMDB_SendCalibrationState( ((pData[1] - 2) * 4) + (Channel+1));
										//дополнительно посылаем сообщение об окончании калибровки канала
										if(pData[1] == 6){
											if(Channel == 0){
												IMDB_SendCalibrationState(21);
												Process_IMDBStart();
												Process_SetStatus(IMDB_FULLTIME_WORK);
											}
											if(Channel == 1){
												IMDB_SendCalibrationState(22);
												Process_IMDBStart();
												Process_SetStatus(IMDB_FULLTIME_WORK);
											}
											if(Channel == 2){
												IMDB_SendCalibrationState(23);
												Process_IMDBStart();
												Process_SetStatus(IMDB_FULLTIME_WORK);
											}
											if(Channel == 3){
												IMDB_SendCalibrationState(24);
												Process_IMDBStart();
												Process_SetStatus(IMDB_FULLTIME_WORK);
											}
										}
											
									}
								}
								
							}
							
				
					//проверка на задание логического номера
					if(Process_GetOperationMode() == IMDB_INITIALIZATION)
					{						
						if(pData[0] == 0x00 && pData[1] == Channel + 1){
								BDSPData[Channel].Ready = 1;
									//channelStartCnt++;
																
									//if(channelStartCnt == CHANNELS)
									if(BDSP_ChannelInitCheck() == CHK_OK)
									{
										Process_BDPSStatus(DEVICE_WAITING);	//ожидаем стабы
										//задаём 1024 канала
										BDSP_SetSpectrSize(SIZE_1024);											
									}
						}
					}
					if(Process_GetOperationMode() != IMDB_FULLTIME_WORK)
					{
						//анализируем пакет параметров

							//параметр  коэфициент выравнивания
							if(pData[0] == 0x02 && pData[1] == 0x08 && pData[2] == 0x02){
								if(rWritingParametrRequest == 0){
									memcpy((uint8*)&BDSPParametrs.fAligmentFactors[Channel],(uint8*)(pData + 3),sizeof(float));
									
									if(Channel < CHANNELS - 1){
										BDSP_ParametrRequest(Channel + 1,0x08);
									}
									else{
										BDSP_ParametrRequest(0,0x09);
									}
								}
								else{
									if(Channel < CHANNELS - 1){
										BDSP_WriteParametr(Channel + 1,0x08,(uint8*)&BDSPParametrs.fAligmentFactors[Channel + 1],sizeof(float));
									}
									else{
										BDSP_WriteParametr(0,0x09,(uint8*)&BDSPParametrs.fDeadTime[0],sizeof(float));
									}
								}
							}
							//параметр мертвое время
							if(pData[0] == 0x02 && pData[1] == 0x09 && pData[2] == 0x02){
								if(!rWritingParametrRequest){
									memcpy((uint8*)&BDSPParametrs.fDeadTime[Channel],(uint8*)pData+3,sizeof(float));
									
									if(Channel < CHANNELS - 1){
										BDSP_ParametrRequest(Channel + 1,0x09);
									}
									else{
										BDSP_ParametrRequest(0,0x0A);
									}
								}
								else{
									if(Channel < CHANNELS - 1){
										BDSP_WriteParametr(Channel + 1,0x09,(uint8*)&BDSPParametrs.fDeadTime[Channel + 1],sizeof(float));
									}
									else{
										BDSP_WriteParametr(0,0x0A,(uint8*)&BDSPParametrs.fCorrectionFactor,sizeof(float));
									}
								}
							}
							//параметр попр коэфициент
							if(pData[0] == 0x02 && pData[1] == 0x0A && pData[2] == 0x02){
								if(!rWritingParametrRequest){
									memcpy((uint8*)&BDSPParametrs.fCorrectionFactor,(uint8*)pData+3,sizeof(float));
								
									if(Channel < CHANNELS - 1){
										BDSP_ParametrRequest(Channel + 1,0x0A);
									}
									else{
										//BDSP_ParametrRequest(0,0x10);
										IMDB_BDPSGetParametrsSignalCallback();
									}
								}
								else{
									if(Channel < CHANNELS - 1){
										BDSP_WriteParametr(Channel + 1,0x0A,(uint8*)&BDSPParametrs.fCorrectionFactor,sizeof(float));
									}
									else{
										rWritingParametrRequest = 0;
										IMDB_BDPSWriteParametrsSignalCallback();
									}
								}
							}
						

					}
	
					if(pData[0] == 0x07 && pData[2] == 0 && pData[7] == 0)
					{
						time 		= (pData[4] << 8) | pData[3];			
						length 	= (pData[6] << 8) | pData[5]; 

						if(length == BDPSChannelSize && time == 0x01)
						{
						
							BDSPData[Channel].DataCounter = 0;
							BDSPData[Channel].ChangeBufferEvent = 0;
						}
					}

					else if(BDSPData[Channel].DataCounter < BDPSChannelSize)
					{
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter ] = (uint16)((pData[1] << 8) | pData[0]);
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter + 1 ] = (uint16)((pData[3] << 8) | pData[2]);
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter + 2 ] = (uint16)((pData[5] << 8) | pData[4]);
						BDSPData[Channel].InputData[activeBuffer][ BDSPData[Channel].DataCounter + 3 ] = (uint16)((pData[7] << 8) | pData[6]);
						
						BDSPData[Channel].DataCounter += 4;
						
					}
					else if(BDSPData[Channel].DataCounter == BDPSChannelSize)
					{
						memcpy(&BDSPData[Channel].DoseRate,pData,sizeof(float));


						//BDSPData[Channel].DoseRate = incorrDoseRate;// * 0.001f;
						BDSPData[Channel].Dose += (BDSPData[Channel].DoseRate * DOSE_TO_HOUR);
						//BDSPData[Channel].Dose = BDSPData[Channel].Dose * DOSE_TO_HOUR;
						
						BDSPData[Channel].DataCounter += 4;
					}
					
					if(BDSPData[Channel].DataCounter == BDPSChannelSize + 4)
					{
						//1. Проверяем, накоплен ли фон
						//2. Если да, то заполняем очередь спектров
						//3. В фии проверки заполнения очереди спектров проверяем, заполнена ли очередь и выполняем рассчёты
						
						/*
						08 08 2017 - зануляем первые каналы в обоих буферах
						*/
						#ifdef DEBUG_BDSP_40ZERO
							if(BDPSChannelSize == SIZE_128){
								memset(BDSPData[Channel].InputData[0],0,sizeof(uint16) * 5);
								memset(BDSPData[Channel].InputData[1],0,sizeof(uint16) * 5);
							}
							if(BDPSChannelSize == SIZE_1024){
								memset(BDSPData[Channel].InputData[0],0,sizeof(uint16) * 40);
								memset(BDSPData[Channel].InputData[1],0,sizeof(uint16) * 40);
							}
						#endif
						
							if(BDSPData[Channel].ActiveBuffer == 1)
								BDSPData[Channel].ActiveBuffer = 0;
							else
								BDSPData[Channel].ActiveBuffer = 1;
							
							BDSPData[Channel].ChangeBufferEvent = 1;	//ставим флаг сработки события смены буфера


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
				
				BDSPData[Serial - 1].Detected = 1;	//ставим флаг обнаружения канала
			
			pData[0] = 0x00;
			pData[1] = Serial;	//(channelStartCnt+1);//присваиваем его в ответное сообщение и посылаем его на блок
			
			//шлём сообщение с заданием лог. номера
			//if(Serial != 4)
				CAN_SendMessageIT(CAN_BDSP_DATA_TX,pData,8);
		}

	}
	else	// сообщение успешной стабилизации
	{
		if(pData[0] == 0x02 && pData[1] == 0x01)
		{
			
			BDSPData[Channel - 1].Ready = 2;
			//включаем передачу МД от канала БД
			
			CAN_SendMessage(CAN_BDSP_DATA_TX + Channel,mSpectrDoseRateOn,8);
			
			if(BDSPData[0].Ready == 2 && BDSPData[1].Ready == 2 && BDSPData[2].Ready == 2 && BDSPData[3].Ready == 2){
				//запрашиваем параметры
				BDSP_ParametrRequests();
				//блок готтов
				Process_BDPSStatus(DEVICE_READY);
			}
		}
	}
}
void	BDSP_StartCalbiration(uint8 Channel)
{
	CAN_SendDefferedMessage(CAN_BDSP_DATA_TX + Channel + 1,mSpectrStartCalibration,8,CALIBRATION_DEFFERED_TIME);
}


void	BDSP_ParametrRequests()
{
	/*
	Запрос мертвого времени и поправочного коэфициента
	*/	
	rWritingParametrRequest = 0;	
	BDSP_ParametrRequest(0,0x08);

}
void	BDSP_StartWritingParametrs()
{
	rWritingParametrRequest = 1;
	//пишем первый параметр
		BDSP_WriteParametr(0,0x08,(uint8*)&BDSPParametrs.fAligmentFactors[0],sizeof(float));
	
}


void	BDSP_WriteParametr(uint8	Channel,uint8	paramIndex,uint8	*pParam,int dataSize)
{
	uint8	msgData[8];
		
		memset(msgData,0,sizeof(uint8) * 8);
	
		msgData[0] = 0x02;				//код операции
		msgData[1] = paramIndex;	//номер параметра
		msgData[2] = 0x02;				//размер параметра (4 байта)
	
	memcpy((uint8*)(msgData + 3),(uint32*)(pParam),dataSize);
	
	CAN_SendMessage(CAN_BDSP_DATA_TX + Channel + 1,msgData,8);
}
void	BDSP_ParametrRequest(uint8 Channel,uint8 paramIndex)
{
	uint8	msgData[8];
		memset(msgData,0,sizeof(uint8) * 8);
	
	msgData[0] = 0x04;				//код операции
	msgData[1] = paramIndex;	//номер параметра
	msgData[2] = 0x00;				//размер параметра (4 байта)
	
	CAN_SendMessage(CAN_BDSP_DATA_TX + Channel + 1,msgData,8);
}
eChanChkStatus		BDSP_ChannelInitCheck()
{
	eChanChkStatus Status = CHK_OK;
	/*
	функция проверки инициализации каналов спектрометра
		после сброса БДГП-С, данный блоки присылает команду перезапуска со своим серийным номером
		данная команда выставляет флаги обнаружения канала спектрометра
	
		после получения команды о перезапуске, БС отсылает команду задания лог. номера.
		регистрация ответа на задание лог. номера выставляет флаг ожидания у текущего канала БДГП-С
	
		для выполнения условия корректной инициализации каналов спектрометра проверяется следующие условия
	0. Флагов Detection = 1 должно == CHANNELS
	1. Флаги Detection == Ready
	*/
	if(!BDSPData[0].Detected & !BDSPData[1].Detected & !BDSPData[2].Detected & !BDSPData[3].Detected){
		Status = CHK_INVALID_CHN_CNH;
	}
	else{
		if( !((BDSPData[0].Detected & BDSPData[0].Ready) &
				 (BDSPData[1].Detected & BDSPData[1].Ready) &
				 (BDSPData[2].Detected & BDSPData[2].Ready) &
				 (BDSPData[3].Detected & BDSPData[3].Ready)))
			Status = CHK_INVALID_CHN_WAIT;
	} 
	
	return Status;
}

void BDSP_BackgroundAccumulation(uint8	Channel,uint16	*pData)
{
	uint16	i = 0,j = 0,sI = 0;
	
	float temp = 0;

	if(BDSPData[Channel].BackgroundIndex < BACKGROUND_TIME){
		for(i = 0;i<SPECTR_SIZE;i++){
			BDSPData[Channel].BackgroundData[i] = BDSPData[Channel].BackgroundData[i] + (pData[i] * 1.0);	//копим счёт
		}
		BDSPData[Channel].BackgroundAverageDose += BDSPData[Channel].DoseRate;	//копим фоновую дозу

		BDSPData[Channel].BackgroundIndex++;
	}
	
	if((BDSPData[Channel].Background == 0) && (BDSPData[Channel].BackgroundIndex == BACKGROUND_TIME) ){
		//завершили накопление фона
		BDSPData[Channel].Background = 1;
		BDSPData[Channel].BackgroundAverageDose = BDSPData[Channel].BackgroundAverageDose / BACKGROUND_TIME;	//получаем фоновую МД
		
			//BDSP_StopChannel(Channel);//останавливаем канал		
				
			//BDSP_SetSpectrChannelSize(SIZE_128,Channel);//BDSP_SetSpectrSize(SIZE_128);//ставим размер спектра 128 каналов
		
		//проверяем, накопили ли мы фон у всех каналов
		if((BDSPData[0].Background == 1) && (BDSPData[1].Background == 1) && (BDSPData[2].Background == 1) && (BDSPData[3].Background == 1))
		{
			//выссчитываем среднюю фоновую МД по всем 4 каналам
			fAccumAverageDoseRate = BDSPData[0].BackgroundAverageDose + BDSPData[1].BackgroundAverageDose +
															BDSPData[2].BackgroundAverageDose + BDSPData[3].BackgroundAverageDose;
			
			fAccumAverageDoseRate = fAccumAverageDoseRate / CHANNELS;
			
			BDSP_SetSpectrSize(SIZE_128);
			Process_IMDBStart();			
			
			IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)&fAccumAverageDoseRate,sizeof(float));//отсылаем успех в накоплении фона
			Process_SetStatus(IMDB_FULLTIME_WORK);//переключаемся в режим штатной работы

			
			//Process_IMDBStart();	//запускаем комплекс	
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
		
		//получаем интеграл по фону со всех 4х каналов
		fBackgroundIntergal += temp;
		
		IMDB_DEBUG_SendDataMsg((uint8*)&temp,100,4);
		
		temp = 0;
		
		//получаем скорость счёта 
		for(i = 0;i<SPECTR_SIZE;i++){
			BDSPData[Channel].BackgroundData[i] = BDSPData[Channel].BackgroundData[i] * BACKGROUND_KOEF;//( 1.0f / (float)BACKGROUND_TIME);
			
			temp += BDSPData[Channel].BackgroundData[i];
		}
		IMDB_DEBUG_SendDataMsg((uint8*)&temp,101,4);
		
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
			BDSPData[i].BackgroundAverageDose = 0;
			
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
	
	if(BDSPData[Channel].QueryIndex < sBufSize)
	{
		memcpy(BDSPData[Channel].Data[BDSPData[Channel].QueryIndex],pData,sizeof(uint16) * SPECTR_SIZE);
		BDSPData[Channel].QueryIndex++;
	}
	else
	{
		memcpy(&BDSPData[Channel].Data[0],&BDSPData[Channel].Data[1],sizeof(uint16) * SPECTR_SIZE * (sBufSize - 1));
		
		memcpy(&BDSPData[Channel].Data[sBufSize - 1],pData,sizeof(uint16) * SPECTR_SIZE);
	}
}


uint8	BDSP_Identification128()
{
	float	fQuantile = 0;
	uint16	*pSumQueryData = 0;
	uint8 jNuclideIndex = 0;
	uint8	maxIntegralIndex = 0;
	uint8	i = 0, j = 0, k = 0, start = 0, end = 0;
	uint16	accTime = MainSettings.accumulationTime;
	
	BDSP_CalculateIntegral();
	BDSP_MaxCounter(&maxIntegralIndex);
	
	memset(fQuantileArray,0,sizeof(float) * SPECTR_SIZE_128);
	
	for(j = 1;j<=126;j++)
	{
		BDSPData[maxIntegralIndex].ppMax = 0;
		BDSPData[maxIntegralIndex].ppBackgroundMax = 0;
		
		start = 116 - 0.91 * j;
		end = 129 - j;
		
			//рассчитываем интегралы по окнам
			for(i = 0;i<QUERY_SIZE;i++)
			{
				for(k = start;k<=end;k++)
				{
					BDSPData[maxIntegralIndex].ppMax += BDSPData[maxIntegralIndex].Data[i][k - 1];
					if(i == 0)
						BDSPData[maxIntegralIndex].ppBackgroundMax += BDSPData[maxIntegralIndex].BackgroundData128[k - 1];
				}
			}
			BDSPData[maxIntegralIndex].ppBackgroundMax = BDSPData[maxIntegralIndex].ppBackgroundMax * QUERY_SIZE;
			//рассчёт квантиля
			fQuantile = (BDSPData[maxIntegralIndex].ppMax - BDSPData[maxIntegralIndex].ppBackgroundMax - 1.0f) / (sqrt(BDSPData[maxIntegralIndex].ppBackgroundMax) + 1.0f);
			if(fQuantile < 0)
				fQuantile = 0;
			
			//если есть превышение квантиля, то записываем его в массив квантилей
			if(fQuantile > MainSettings.limitDetect[0])
				fQuantileArray[j-1] = fQuantile;
			
	}
	/*
	Обнаружение
	*/
	//проверяем наличие превышений в массиве квантилей
	for(j = 1;j<=126;j++)
	{
		//если нет флага обнаружения
		if(!NuclideDetection){
			//если есть превышение квантиля
			if(fQuantileArray[j-1] > MainSettings.limitDetect[0]){
				//ставим флаг обнаружения и шлём событие обнаруженя
				NuclideDetection = 1;
				IMDB_NuclideDetectionSignalCallback();
			}
		}
		//если установлен флаг обнаружения
		else
		{
			//если в массиве квантилей есть превышение порога, то выходим из цикла
			if(fQuantileArray[j-1] > MainSettings.limitDetect[0])
				break;
			//если нет и это последний проверенный квантиль, то в массиве нет вообще превышений, шлём сигнал о сбросе обнаружения
			else
			{
				if(j == 126){
					NuclideDetection = 0;
					
					IMDB_NuclideEndDetectionSignalCallback();
					IMDB_NuclideIdentificationEndSignalCallback();
				}
			}
		}
	}
	/*
	Идентификация
		проводится только если есть обнаружение
	*/
	if(NuclideDetection == 1)
	{
		//получаем суммарный спектр канала с максимальным суммарным счётом
		pSumQueryData = (uint16*)BDSP_DEBUG_GetQuerySumSpectr(maxIntegralIndex);
		
		for(j = 120;j>=4;j--){
			if( (pSumQueryData[j] > 20) &&  (pSumQueryData[j] > (0.8f * (pSumQueryData[j+2]+pSumQueryData[j+3]) + 2)) &&
																			(pSumQueryData[j] > (0.8f * (pSumQueryData[j-2]+pSumQueryData[j-3]) + 2)) ) {
																				
																				jNuclideIndex = j;
																				
																				if(!NuclideIdentification)
																					NuclideIdentification = 1;
																				break;
																			}
		}
		
		if(NuclideIdentification)
		{
			IMDB_DEBUG_SendDataMsg((uint8*)&jNuclideIndex,7,1);
			//Cs137
			if(jNuclideIndex >=26 && jNuclideIndex <= 29){
				IMDB_NuclideIdentificationSignalCallback(1);
			}
			//Cs-134
			else if(jNuclideIndex >=32 && jNuclideIndex <= 36){
				IMDB_NuclideIdentificationSignalCallback(2);
			}
			//Co-60
			else if(jNuclideIndex >=53 && jNuclideIndex <= 57){
				IMDB_NuclideIdentificationSignalCallback(3);
			}
			//I-131
			else if(jNuclideIndex >=12 && jNuclideIndex <= 16){
				IMDB_NuclideIdentificationSignalCallback(4);
			}
			else{
				IMDB_NuclideIdentificationSignalCallback(0xFF);
			}
			NuclideIdentification = 0;
		}
		/*
		for(j = 5;j<=121;j++)
		{

			if(fQuantileArray[j-1] > MainSettings.limitDetect[0] + 1)
			{
				//
				//TO-DO это надо исправить!
				//
				if(fQuantileArray[j-1] > (0.33f * (fQuantileArray[j-3] + fQuantileArray[j-4] + fQuantileArray[j-5]))){
					if(fQuantileArray[j-1] > (0.33f * (fQuantileArray[j+1] + fQuantileArray[j+2] + fQuantileArray[j+3]))){
						jNuclideIndex = j;
						if(!NuclideIdentification)
							NuclideIdentification = 1;
						break;
					}
				}
			}
		}
		if(NuclideIdentification)
		{
			IMDB_DEBUG_SendDataMsg((uint8*)&jNuclideIndex,7,1);
			
			if(jNuclideIndex >=98 && jNuclideIndex <= 102){
				IMDB_NuclideIdentificationSignalCallback(1);
			}
			else if(jNuclideIndex >=91 && jNuclideIndex <= 95){
				IMDB_NuclideIdentificationSignalCallback(2);
			}
			else if(jNuclideIndex >=67 && jNuclideIndex <= 73){
				IMDB_NuclideIdentificationSignalCallback(3);
			}
			else if(jNuclideIndex >=110 && jNuclideIndex <= 114){
				IMDB_NuclideIdentificationSignalCallback(4);
			}
			else{
				IMDB_NuclideIdentificationSignalCallback(0xFF);
			}
			NuclideIdentification = 0;
		}*/
		
	}
	
	return 1;
}


uint8	BDSP_Identification(uint16	*ePhotoPeak, float	*iSpdCounter)//ф-ия идентификации в спектрометрическом режиме
{
	
	static uint16 photoPeakEnergy = 0;		//энергия фотопика
	static float wndNuclideCnt = 0;	//скорость счёта в окне
	float	fQuantile = 0;
	__IO uint8 temp1 = 0;
	uint16	leftBrd = 0;
	uint16	rightBrd = 0;
	
	uint16 jNuclideIndex = 0;
	uint8	maxIntegralIndex = 0;
	uint32	maxIntegralValue = 0;
	uint16	i = 0, j = 0, k = 0, start = 0, end = 0;
	uint16	accTime = MainSettings.accumulationTime;
	
	/////
	static float near fMaxBackground = 0;
	static float near fMaxData[10];
	/////
	
	memset(fMaxData,0,sizeof(float) * 10);
	
	for(i = 0;i<CHANNELS;i++){
		BDSPData[i].Integral = 0;
			for(j = 0;j<SPECTR_SIZE;j++){
				BDSPData[i].Integral += BDSPData[i].AccumSpectrData[j];
			}
			if(BDSPData[i].Integral > maxIntegralValue){
				maxIntegralValue = BDSPData[i].Integral;
				maxIntegralIndex = (uint8)i;
			}
	}
	
	/*
		Выполняем поиск и идентификацию только на измерении в точке 1
	*/
	if(MeasureInPntData.MeasureNumber == 1){
	
		memset(fQuantileArray,0,sizeof(float) * SPECTR_SIZE);
		
		
		NuclideDetection = 0;
		
		
		for(k = 1000;k>=21;k--){
			memset(fMaxData,0,sizeof(float) * 10);
			fMaxBackground = 0;
			
			for(i = k - 1;i<= k + 1;i++)
				fMaxData[0] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			
			for(i = k - 5;i<= k - 3;i++)
				fMaxData[1] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			fMaxData[1] = 1.0f * (float)fMaxData[1] + 1.0f;
			
			for(i = k + 3;i<= k + 5;i++)
				fMaxData[2] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			fMaxData[2] = 1.0f * (float)fMaxData[2] + 1.0f;
			
			for(i = k - 9;i<= k - 7;i++)
				fMaxData[4] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			fMaxData[4] = 1.0f * (float)fMaxData[4] + 1.0f;
			
			for(i = k + 7;i<= k + 9;i++)
				fMaxData[5] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			fMaxData[5] = 1.0f * (float)fMaxData[5] + 1.0f;
			
			for(i = k - 7;i<= k - 5;i++)
				fMaxData[6] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			fMaxData[6] = 1.0f * (float)fMaxData[6] + 1.0f;
			
			for(i = k + 5;i<= k + 7;i++)
				fMaxData[7] += BDSPData[maxIntegralIndex].AccumSpectrData[i];
			fMaxData[7] = 1.0f * (float)fMaxData[7] + 1.0f;
			
			for(i = k - 1;i<= k + 1;i++)
				fMaxBackground += BDSPData[maxIntegralIndex].BackgroundData[i];
			fMaxBackground = fMaxBackground * accTime;
			
			fMaxData[3] = fMaxBackground + (MainSettings.limitDetect[0] * sqrt(fMaxBackground) + 1.5f);
			
			/*
			0 - [-1 +1]
			1 - [-5 -3]
			2 - [+3 +5]
			3 - фон
			4 - [-9 -7]
			5 - [+7 +9]
			6 - [-7 -5]
			7 - [+5 +7]
			*/
			/*
				if(k == 300)
				{
					temp1 = 1;
				}
			*/
			/*if(	(fMaxData[0] > 20.0f) &&
					(fMaxData[0] > fMaxData[1]) &&
					(fMaxData[0] > fMaxData[2]) &&
					(fMaxData[0] > fMaxData[4]) &&
					(fMaxData[0] > fMaxData[5]) &&
					(fMaxData[0] > fMaxData[3]))*/
				if((fMaxData[0] > 20.0f) &&
					 (fMaxData[0] > fMaxData[1])&&
					 (fMaxData[0] > fMaxData[2])&&
					 (fMaxData[0] > fMaxData[4])&&
				   (fMaxData[0] > fMaxData[5])&&
					 (fMaxData[0] > fMaxData[6])&&
					 (fMaxData[0] > fMaxData[7])&&
					 (fMaxData[1] > fMaxData[4])&&
					 (fMaxData[2] > fMaxData[5])&&
					 (fMaxData[0] > fMaxData[3]))
					{
							//IMDB_DEBUG_SendDataMsg((uint8*)&fMaxBackground,211,4);
							/*IMDB_DEBUG_SendDataMsg((uint8*)&k,8,2);
							IMDB_DEBUG_SendDataMsg((uint8*)&fMaxData[0],200,4);
							IMDB_DEBUG_SendDataMsg((uint8*)&fMaxData[1],201,4);
							IMDB_DEBUG_SendDataMsg((uint8*)&fMaxData[2],202,4);
							IMDB_DEBUG_SendDataMsg((uint8*)&fMaxBackground,203,4);
							*/
							jNuclideIndex = k;
							
							NuclideDetection = 1;
							IMDB_NuclideDetectionSignalCallback();
					
						break;
					}
		}
		if(NuclideDetection == 1)
		{
			if(!NuclideIdentification)
				NuclideIdentification = 1;
			
			if(NuclideIdentification){
					IMDB_DEBUG_SendDataMsg((uint8*)&jNuclideIndex,8,2);
				
				if(jNuclideIndex >=210 && jNuclideIndex <= 230){
					IMDB_NuclideIdentificationSignalCallback(1);
				}
				else if(jNuclideIndex >=260 && jNuclideIndex <= 270){
					IMDB_NuclideIdentificationSignalCallback(2);
				}
				else if(jNuclideIndex >=440 && jNuclideIndex <= 450){
					IMDB_NuclideIdentificationSignalCallback(3);
				}
				else if(jNuclideIndex >=115 && jNuclideIndex <= 125){
					IMDB_NuclideIdentificationSignalCallback(4);
				}
				else{
					//IMDB_NuclideIdentificationSignalCallback(0xFF);
					IMDB_NuclideEndDetectionSignalCallback();
					IMDB_NuclideIdentificationEndSignalCallback();					
				}
				NuclideIdentification = 0;		
			}
		}		
	}//END MeasureInPntData.MeasureNumber == 1

/*
	photoPeakEnergy = 2910 - 2.865 * jNuclideIndex;
	
	for(j = (915 - 0.91 * jNuclideIndex);j<= 1025 - jNuclideIndex;j++)
		wndNuclideCnt += BDSPData[maxIntegralIndex].AccumSpectrData[j - 1];
	wndNuclideCnt = wndNuclideCnt / accTime;
*/
	//при измерении в точке 2 восставливаем скорость счета в окне и энергию фотопика из j-ого первого шага
	if(MeasureInPntData.MeasureNumber == 2)
		jNuclideIndex = iJIndexNuclideIdentification;
	
	//получаем энергию фотопика
	photoPeakEnergy = /*2925 - */3.0f * jNuclideIndex;
	//получаем границы окна фотопика
	leftBrd = (/*946 - */jNuclideIndex - 20);
	rightBrd = (/*1004 - */jNuclideIndex + 20);
	//получаем скорость счёта в окне фотопика
		wndNuclideCnt = 0;
		
			for(j = leftBrd;j<=rightBrd;j++)
				wndNuclideCnt += BDSPData[maxIntegralIndex].AccumSpectrData[j - 1];
		
		wndNuclideCnt = wndNuclideCnt / accTime;
	
	*ePhotoPeak = photoPeakEnergy;
	*iSpdCounter = wndNuclideCnt;

	return jNuclideIndex;
	
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
	uint16	photoPeak = 0;
	float		spdCounter = 0.0f;
	float 	fCurrentDoseRate = 0;
	
	
	
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
					//IMDB_SendPacket(METROLOGY_SEND_ACCUMSPECTR_SUCCESS,(uint8*)&Channel,sizeof(uint8));	//отправка сообщения об успешном накоплении спектра
				IMDB_SendAccumSpectrSuccessSignalCallback(Channel);
			}
			
			if( BDSPData[0].AccumSpectr == 1 && 
					BDSPData[1].AccumSpectr == 1 &&
					BDSPData[2].AccumSpectr == 1 &&
					BDSPData[3].AccumSpectr == 1){
						
		
						
						
						BDSP_SetSpectrSize(SIZE_128);//ставим размер спектра 128 каналов
						
						
							if(BDSPAccumMode == ACCUM_AND_MEASURE){
								//TODO рассчёт данных для отправки в сообщении об окончании измерения. На данный момент установлена заглушка
								//если у нас первое измерение
								fCurrentDoseRate = BDSP_GetDoseRate();
								
								if(MeasureInPntData.MeasureNumber == 1)
								{
									/*
										получаем рассчёт идентификации
										считаем угловое распределение для получения угла
										получаем МД снаружи
										получаем угол
										получаем фотопик
										получаем скорость счёта в окне фотопика
									*/
										iJIndexNuclideIdentification = BDSP_Identification(&photoPeak,&spdCounter);	//запоминаем индекс окна
									
									BDSP_CalculateAngularDiagram(SIZE_1024);
									
									MeasureInPntData.fMaxAngle 		= BDSP_GetMaximumAngle();
									MeasureInPntData.fDoseRate 		= fCurrentDoseRate;
									MeasureInPntData.ePhotoPeak 	= photoPeak;
									MeasureInPntData.iSpdCounter	= spdCounter;
									
									IMDB_SendMeasureInPointEndSignalCallback();
								}
								if(MeasureInPntData.MeasureNumber == 2)
								{	
									BDSP_Identification(&photoPeak,&spdCounter);	//запоминаем индекс окна
									
									BDSP_CalculateAngularDiagram(SIZE_1024);
									
									MeasureInPntData.fMaxAngle = BDSP_GetMaximumAngle();
									MeasureInPntData.fDoseRate = fCurrentDoseRate;
									MeasureInPntData.iSpdCounter = spdCounter;
									
									IMDB_SendMeasureInPointEndSignalCallback();
								}
							}				
							
						Process_SetStatus(IMDB_FULLTIME_WORK);
						BDSP_ClearWorkData();
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
uint8	*BDSP_DEBUG_GetQuerySumSpectr(uint8	channel)
{
	uint8	i = 0,j = 0;
	
	memset(&BDSPData[channel].DebugData,0,sizeof(uint16) * SPECTR_SIZE_128);
	
	for(i = 0;i<QUERY_SIZE;i++)
		for(j = 0;j<SPECTR_SIZE_128;j++)
			BDSPData[channel].DebugData[j] += BDSPData[channel].Data[i][j];
	
	return 	BDSPData[channel].DebugData;
}

uint8	*BDSP_GetChannelSpectr(uint8	channel)
{	
	return (uint8*)&BDSPData[channel].WorkData[0];
}
uint8	*BDSP_GetAccumulationSpectr(uint8	Channel)
{
	return (uint8*)&BDSPData[Channel].AccumSpectrData[0];
}

void	BDSP_CalculateAngularDiagram(eSpecMode Mode)
{
	static float	*pAngleDiagramm;
	
	static float cntMin = 0;
	static float cntMax = 0;
	uint16 i = 0;
	static float sensorArray[4];
	
	/*
		В зависимости от выбранного режима рассчёта угловой диаграммы считаем либо по сумме движка, либо по сумме накопленного спектра
	*/
	if(Mode == SIZE_128)
	{	
		for(i = 0;i<CHANNELS;i++){
			sensorArray[i] = BDSP_CalculateQuerySum(i);
		}
	}
	else
	{
		for(i = 0;i<CHANNELS;i++){
			sensorArray[i] = BDSPData[i].Integral;
		}
	}
		
	cntMin = 100000.0f;
	cntMax = 0.0f;
	
	for(i = 0;i<CHANNELS;i++){
		if(sensorArray[i] > cntMax)
			cntMax = sensorArray[i];
		
		if(sensorArray[i]<cntMin)
			cntMin = sensorArray[i];
		
	}
		pAngleDiagramm	= BDGP_DirectionDiagramm(sensorArray, CHANNELS, cntMin,cntMax,&radiationMaximum,SPEC_MODE);	
	
	memcpy((uint8*)&fpBDSPAngleDiagramm,(uint8*)pAngleDiagramm,sizeof(float) * 48);
	//return pAngleDiagramm;
}

float	*BDSP_GetAngularDiagram()
{/*
	float	*pAngleDiagramm;
	
	static float cntMin = 0;
	static float cntMax = 0;
	uint16 i = 0;
	static float sensorArray[4];
	
	for(i = 0;i<CHANNELS;i++){
		sensorArray[i] = BDSP_CalculateQuerySum(i);
	}

	cntMin = 100000.0f;
	cntMax = 0.0f;
	
	for(i = 0;i<CHANNELS;i++){
		if(sensorArray[i] > cntMax)
			cntMax = sensorArray[i];
		
		if(sensorArray[i]<cntMin)
			cntMin = sensorArray[i];
		
	}
		pAngleDiagramm	= BDGP_DirectionDiagramm(sensorArray, CHANNELS, cntMin,cntMax,&radiationMaximum,SPEC_MODE);	
	
	return pAngleDiagramm;*/
	
	return fpBDSPAngleDiagramm;
}


uint16	BDSP_GetMaximumAngle()
{
	return radiationMaximum;
}
float		BDSP_GetDose()
{
	float fDose = 0;
	uint8	i = 0;
	
	for(i = 0;i<CHANNELS;i++){
		fDose += BDSPData[i].Dose;
	}
	fDose = fDose / CHANNELS;
	
	return fDose;
}
//ф-ия возврата максимальной МД
float		BDSP_GetDoseRate()
{
	float fDoseRate = 0;
	uint8	i = 0;
	
	for(i = 0;i<CHANNELS;i++){
		if(BDSPData[i].DoseRate > fDoseRate)
			fDoseRate = BDSPData[i].DoseRate;
	}
	return fDoseRate;
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
	uint8	changeBufferEventCnt = 0;
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
					
					
					switch(Process_GetOperationMode())
					{
						case IMDB_FULLTIME_WORK:{
							//проверяем, накоплен ли фон у всех каналов
							if( (BDSPData[0].Background == 1) && (BDSPData[1].Background == 1) && (BDSPData[2].Background == 1) && (BDSPData[3].Background == 1) )	{	
								BDSP_InsertInQuery(BDSPData[i].WorkData,i);
								//выполняем фоновую идентификацию
								
								changeBufferEventCnt++;
								//получили данные от всех 4 каналов
								if(changeBufferEventCnt == CHANNELS){
									//проверяем, накоплен ли движок
										if(BDSP_GetAccumQueryState()){
											BDSP_Identification128();

										}
										
									changeBufferEventCnt = 0;
								}
								
									
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
void	BDSP_SetMeasureNumber(uint8	MeasureNum)
{
	MeasureInPntData.MeasureNumber = MeasureNum;
}

uint8	*BDSP_GetParametrs()
{
	return (uint8*)&BDSPParametrs;
}
uint8	BDSP_GetAccumQueryState()
{
	uint8	Result = 1;
	uint8	i = 0;
	
		for(i = 0;i<CHANNELS;i++){
			if(BDSPData[i].QueryIndex != QUERY_SIZE){
				Result = 0;
				break;
			}
		}
	
	return Result;
}
float	BDSP_GetBackgroundQueryIntegral()
{
	return fBackgroundIntergal * QUERY_SIZE;
}
sSpecModeData			BDSP_GetMeasureInPointData()
{
		return MeasureInPntData;
}
