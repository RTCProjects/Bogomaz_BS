#include "protocol.h"
#include "asc0.h"
#include "process.h"
#include "settings.h"
#include "devices.h"
#include "system.h"

#include "bdmg.h"
#include "bdgp.h"
#include "bdsp.h"

#include "vers/vers.h"
/*
Реализация протокола передачи данных ПО UART для связи ИНК-Б ИМД-Б
*/
#define INT_BUF_SIZE	128
#define RCV_BUF_SIZE 512


uint8	near interruptBuffer[INT_BUF_SIZE];
uint8	receiveBuffer[RCV_BUF_SIZE];

static uint32	readBufInd = 0;
static uint32	writeBufInd = 0;
int rIndex;

float testMS = 0.0f;


//const sServiceMsg	ServiceMsg = {0x12,0x63,79511901};	//XX - FRM, YY - PRTCL, ZZZZZZZZ - Serial

static sProtocolRequests	ProtocolRequests;
static tMetrologyData			MetrologyData;
static uint8 visualSpecBuf[1024];	//массив 1024 байта для 4х 128 канальных спектров(визуализация на ИНК).

__IO static uint32 cmdCnt;

void IMDBProtocol_Init(void)
{
	
	/*uint16	i = 0;
	
	for(i = 0;i<1024;i++)
		spectrArray[i] = i;

	spectrIndex = 0;*/
	testMS = 0;
	readBufInd = 0;	//индекс вставки в буфер из UART
	writeBufInd = 0;
	rIndex = 0;
	cmdCnt = 0;
	memset((uint8*)&ProtocolRequests,0,sizeof(sProtocolRequests));
	memset((uint8*)&MetrologyData,0,sizeof(tMetrologyData));

	memset(interruptBuffer,0,INT_BUF_SIZE);
	memset(receiveBuffer,0,RCV_BUF_SIZE);
	
}

//Фия создания пакета для отправки на ИНК-К
uint8 *IMDBProtocol_CreatePacket(uint8	cmd,uint8 *pData,uint8 dataSize,uint8 *packSize)
{
	uint8	bufferSize = (uint8)(5 + dataSize);
	uint8	*pBuf = (uint8*)malloc(sizeof(uint8) * bufferSize);
	
	uint16	ks = 0,i = 0;
	
	if(pBuf)
	{
		pBuf[0] = 0x55;
		pBuf[1] = bufferSize;
		pBuf[2] = cmd;
		memcpy((pBuf + 3),pData,dataSize);

			for(i = 0;i<bufferSize - 2;i++)
				ks+=pBuf[i];
		
		memcpy((pBuf + (bufferSize - 2)),&ks,2);
		
		*packSize = bufferSize;
	

	}
	return pBuf;
}

/*
	pDataBuf			-	указатель на временный буфер данных
	sDataBuf			-	размер буфера
	pDataBufIndex	-	указатель на итератор временного буфера
	InputByte			-	байт входного потока UART
	packSize			- размер полученного пакета
*/
uint8	* IMDBProtocol_GetPacketFromStream(uint8 *pDataBuf,int sDataBuf, int	*pDataBufIndex, uint8	InputByte, int *packSize)
{
	int index;
  int i,ii;
  int ks = 0,pKS = 0;
	uint16 len;

	uint8 *pD;
	int iD = 0;

	if (sDataBuf<=0)  return 0;
		index = *pDataBufIndex;
	
	 // удаляем все до байта синхронизации
  if ((index>0) && (pDataBuf[0]!=0x55)) {
    ii=0;
    while ((ii<index) && (pDataBuf[ii]!=0x55))  ii++;
    memcpy(&pDataBuf[0],&pDataBuf[ii],index-ii);
    index=index-ii;
  }
	//проверяем переполнение
	if(index >= sDataBuf)
	{
		 memcpy(&pDataBuf[0],&pDataBuf[1],sDataBuf-1);
			index=sDataBuf-1;
		
		 if ((index>0) && (pDataBuf[0]!=0x55)) {
      ii=0;
      while ((ii<index) && (pDataBuf[ii]!=0x55))  ii++;
      memcpy(&pDataBuf[0],&pDataBuf[ii],index-ii);
      index=index-ii;
    }
	}
	



	pDataBuf[index]=InputByte;
		index++;
			*pDataBufIndex=index;
	
	pD=(uint8* )pDataBuf;
	iD=index;
	
	while (1) {
		 if (iD<5)  break;
		
			if(pD[0] == (uint8)0x55){
				len = (uint8)pD[1];
				
				if(iD>=len)
				{
					if(len>0){
					//pKS = (pD[len-2] << 8) | pD[len-1];
					pKS = (pD[len-1] << 8) | pD[len-2];
						
					for(i = 0;i<len-2;i++)
						ks+=pD[i];	//посчитали КС
					
						if(pKS == ks)
						{
							if(packSize)	*packSize = (len - 5);
							
								return (pD);
						}
					}
				}	
				
			}
			pD++;
			iD--;
	}
	return 0;
	
}
//
void IMDB_ReceiveByteCallback(uint8	inputByte)
{

	interruptBuffer[(writeBufInd++)%INT_BUF_SIZE] = inputByte;
	/*if(writeBufInd == BUFFER_SIZE)writeBufInd = 0;
	interruptBuffer[writeBufInd] = inputByte;
		writeBufInd++;
	*/

	//rIntIndexBufIN &= (BUFFER_SIZE - 1);
	/*
	
	static uint8 *pPackage;
	static int sPackage;
	 
		pPackage = IMDBProtocol_GetPacketFromStream(receiveBuffer,BUFFER_SIZE,(int*)&rIndex,inputByte,&sPackage);
			
		if(pPackage)
		{
			
			IMDB_PackageAnalysis(pPackage[2],sPackage,(uint8*)pPackage+3);
			rIndex = 0;
		}*/
}
sbit  P2_8    = P2^8;
//обработка данных, принятых по UART,  и расшифровка команд
//void IMDB_ReceiveByteCallback(uint8	inputByte)
void IMDB_ReceiveByteReading()
{
	
	
	uint16	i = 0;
	uint8 *pPackage;
	uint8  inputByte;
	int sPackage;
	
	//P2_8 = 0;
	
	//inputByte = interruptBuffer[(readBufInd++)%BUFFER_SIZE];
	while(writeBufInd > readBufInd){
		//P2_8 = 1;
		/*
		if(readBufInd == BUFFER_SIZE)readBufInd = 0;
			inputByte = interruptBuffer[readBufInd];
		readBufInd++;	
		*/
		inputByte = interruptBuffer[(readBufInd++)%INT_BUF_SIZE];
		
		pPackage = IMDBProtocol_GetPacketFromStream(receiveBuffer,RCV_BUF_SIZE,(int*)&rIndex,inputByte,&sPackage);
		
	if(pPackage)
	{
		IMDB_PackageAnalysis(pPackage[2],sPackage,(uint8*)pPackage+3);
		rIndex = 0;	
	}
}
	/*
	//перебираем байты из кольцевого буфера
	for(i = 0;i<BUFFER_SIZE;i++){
		inputByte = interruptBuffer[i];
		
		//кормим на вход функции все байты из приёмного буфера
		pPackage = IMDBProtocol_GetPacketFromStream(receiveBuffer,BUFFER_SIZE,(int*)&rIndex,inputByte,&sPackage);
		
		//нашли пакет
		if(pPackage){
			cmdCnt++;
			
			//удалили обработанный пакет из буфера
			memcpy((uint8*)interruptBuffer,(uint8*)(interruptBuffer + i),BUFFER_SIZE - i);
			rIndex = 0;	
			if(rIntIndexBufIN >= i)
				rIntIndexBufIN = rIntIndexBufIN - i;
			else
				rIntIndexBufIN = 0;
			
			IMDB_PackageAnalysis(pPackage[2],sPackage,(uint8*)pPackage+3);
			
			break;
		}
	}*/
}

void IMDB_PackageAnalysis(uint8	cmd,uint8 bytesCount,uint8	*pData)//анализируем пакет от ИНК
{
	uint8	*pSpectr = 0;
	uint8	currentFeature = 0;
	float currentDose = 0;
	float Dose = 0;
	
	
	uint16	spectrIndex = 0;
	
	
	
	switch(cmd)
	{
		case INK_GET_STATE:
		{
			if(!ProtocolRequests.rState){
				ProtocolRequests.rState++;
				Process_ResetWaitCounter();
			}
			//IMDB_SendState();
		}break;
		
		case INK_SET_STATE:
		{
			/*Process_SetStatus(pData[0]);
			
			IMDB_SendState();*/
				//получаем текущий заданный статус
				switch(/*Process_GetOperationMode()*/pData[0] & 0x0F)
				{
					case IMDB_INITIALIZATION:{ System_Reset(); }break;														//режим инициализации
					
					//режим штатной работы
					case IMDB_FULLTIME_WORK:
					{
							//получили запрос на переход в штатный режим
							if(Process_GetBDMGState() == DEVICE_NOTREADY && Process_GetBDGPState() == DEVICE_NOTREADY && Process_GetBDPSStatus() == DEVICE_NOTREADY){
								IMDB_SendError();
								return;
							}
							else if(Process_GetBDPSStatus() == DEVICE_WAITING){
								IMDB_SendError();
								return;
							}
							else{
								Process_SetStatus(pData[0]);
								Process_IMDBStart();
								//IMDB_SendState();
							}
					
						
						
					}break;
					
					//накопление фона
					case IMDB_BACKGROUND_ACCUMULATION:
					{
						/*
							Приняли команду - накопление фона. Алгоритм поведения ПО:
								- Если БДГП-С обнаружен и застабилизирован
									1. Обнуляем фоновый спектр БДГП-С
									2. Обнуляем флаг накопления
									3. Запускаем накопление фона
								- В противном случае
									1. Обнуляем фоновый счёт БДГП-Б
									2. Обнуляем флаг накопления
									3. Запускаем накопление фона
						*/
						if(Process_GetBDPSStatus() == DEVICE_READY){
							BDSP_ResetBackgroundData();
							
							Process_SetStatus(pData[0]);
							Process_IMDBStart();
						}
						else if(Process_GetBDGPState() == DEVICE_READY){						
							BDGP_ResetBackgroundData();
							
							Process_SetStatus(pData[0]);
							Process_IMDBStart();
						}
						else
							IMDB_SendError();
						
					}break;
										
					case IMDB_SETTINGS_MODE:
					{
						if(Process_GetOperationMode() == IMDB_FULLTIME_WORK){
							Process_IMDBStop();
							Process_SetStatus(pData[0]);
						}
						else
							IMDB_SendError();
					}break;
					
					case IMDB_SPECTR_ACCUMULATION:
					{
						BDPS_ClearQuery();
						Process_SetStatus(pData[0]);
					}break;
					
					case IMDB_CALIBRATION_I:
					{	
						//останавливаем ИМДБ
						Process_IMDBStop();
							//выдерживаем паузу для окончания передачи пакета данных
	
						BDSP_StartCalbiration(0);
						Process_SetStatus(pData[0]);
					}break;
					
					case IMDB_CALIBRATION_II:
					{
						//останавливаем ИМДБ
						Process_IMDBStop();
							//выдерживаем паузу для окончания передачи пакета данных

						BDSP_StartCalbiration(1);
						Process_SetStatus(pData[0]);
					}break;
					
					case IMDB_CALIBRATION_III:
					{
						//останавливаем ИМДБ
						Process_IMDBStop();
							//выдерживаем паузу для окончания передачи пакета данных

						BDSP_StartCalbiration(2);
						Process_SetStatus(pData[0]);
					}break;
					
					case IMDB_CALIBRATION_IV:
					{
						//останавливаем ИМДБ
						Process_IMDBStop();
							//выдерживаем паузу для окончания передачи пакета данных
						BDSP_StartCalbiration(3);
						Process_SetStatus(pData[0]);
					}break;
					
					
				}
			
			//IMDB_SendState();
		}break;
		
		case INK_DATA_REQUEST:
		{
				switch(pData[0])
				{
						case 0x01:	//запрос МД снаружи
						{
							if(!ProtocolRequests.rOutsideDoseRate)
								ProtocolRequests.rOutsideDoseRate++;
							//IMDB_SendOutsideDoseRate();
						}break;
						
						case 0x02: //запрос признака МД снаружи
						{
							if(!ProtocolRequests.rOutsideFeature)
								ProtocolRequests.rOutsideFeature++;
							//IMDB_SendOutsideDoseFeature();
						}break;	
						
						case 0x03: //запрос МД внутри
						{
							if(!ProtocolRequests.rInsideDoseRate)
								ProtocolRequests.rInsideDoseRate++;
							//IMDB_SendInsideDoseRate();
						}break;
						
						case 0x04: 	//запрос признака МД внутри
						{
							if(!ProtocolRequests.rInsideFeature)
								ProtocolRequests.rInsideFeature++;
							//IMDB_SendInsideDoseFeature();
						}break;
						
						case 0x05: //запрос дозы внутри
						{
							if(!ProtocolRequests.rInsideDose)
								ProtocolRequests.rInsideDose++;
							//IMDB_SendInsideDose();
							
						}break;	
						case 0x06: //запрос угла на максимум 
						{
							if(!ProtocolRequests.rMaxAngle)
								ProtocolRequests.rMaxAngle++;
							//IMDB_SendMaxAngle();
						} break;
							
						case 0x07: //запрос углового распределения
						{		
							if(!ProtocolRequests.rAngularDistribution)
								ProtocolRequests.rAngularDistribution++;
						}break;
						
						case 0x08:	//идентификация
						{
							//IMDB_SendIdentification();
						} break;
						//запрос спектра
						case 0x09:
						{
							spectrIndex = 0;
							
							pSpectr = (uint8*)BDSP_GetCurrentSpectr();
							
							do
							{
									IMDB_SendPacket(IMDB_SPECTR + spectrIndex,(uint8 *)(pSpectr + (spectrIndex * 128) ),128);
							

								spectrIndex++;
								
								_srvwdt_();
							}
							while(spectrIndex < SPECTR_SIZE / 64);
							
						}break;
						default: /*IMDB_SendPacket(IMDB_ERROR,0,0);*/ break;
				}
				
		}break;
		
		case INK_GET_SHIELDING_FACTOR: { IMDB_SendPacket(IMDB_SHIELDING_FACTOR,(uint8*)((uint32*)(&MainSettings.shieldingFactor)),4);  }break;
		
		case INK_SET_SHIELDING_FACTOR:
		{
			memcpy(&MainSettings.shieldingFactor,pData,4);
				SETTINGS_Save();
			IMDB_SendPacket(IMDB_SHIELDING_FACTOR,(uint8*)((uint32*)(&MainSettings.shieldingFactor)),4);
		}break;
		
		case INK_SET_ACC_TIME:
		{
			memcpy(&MainSettings.accumulationTime,pData,2);
				SETTINGS_Save();
			IMDB_SendPacket(IMDB_SPECTRACC_TIME,(uint8*)((uint16*)(&MainSettings.accumulationTime)),2);
		}break;
		
		case INK_SET_LIMIT_DETECT:
		{
			memcpy((&MainSettings.limitDetect) + (pData[0] - 1),pData + 1,4);
			
			SETTINGS_Save();
			
			IMDB_SendPacket(IMDB_LIMIT_DETECT,(uint8*)pData,bytesCount);
		}break;
		
		case INK_START_MEASURE:
		{
			/*//приняли команду запуска измерений
			if(Process_GetBDPSStatus() == DEVICE_READY){
				BDSP_StartSpectrAccumulation(pData[0]);
				
				IMDB_SendPacket(IMDB_START_MEASURE,(uint8*)&pData[0],sizeof(uint8));
			}
			*/
			IMDB_SendStartMeasureInPoint(pData[0]);
		}break;
		
		case INK_GET_FIRMWARE_INFO:
		{
			IMDB_SendPacket(IMDB_SERVICE,(uint8*)&ServiceMsg,sizeof(sServiceMsg));
		}break;
		
		case METROLOGY_READPARAM_REQUEST:
		{
			ProtocolRequests.rMetrologyReadBDParametrs++;
		}break;
		
		case METROLOGY_WRITEPARAM_REQUEST:
		{
			IMDB_Metrology_WriteBDParametrs(pData);
			ProtocolRequests.rMetrologyWriteBDParametrs++;
				//IMDB_Metrology_WriteBDParametrs(pData);
		}break;
		
		case METROLOGY_DEFAULT_SETTINGS_REQUEST:
		{
			IMDB_Metrology_DefaultSettings();
		}break;
		
		case METROLOGY_READ_SETTINGS_REQUEST:
		{
			IMDB_Metrology_ReadBSParametrs();
		}break;
		
		case METROLOGY_WRITE_SETTINGS_REQUEST:
		{
			IMDB_Metrology_WriteBSParametrs(pData);
		}break;
		
		//команда инициализации БД для получения данных метрологии
		case METROLOGY_STARTBD_REQUEST:
		{
			IMDB_Metrology_StartBD(pData);
		}break;
		
		//команда запроса данных от БД
		case METROLOGY_DATA_REQUEST:
		{
			IMDB_Metrology_GetBDData();
		}break;
		
		//включение режимов
		case METROLOGY_SPECTR_ACCUMMODE_REQUEST:
		{
			IMDB_Metrology_SpectrometryMode(pData);
		}break;
		
		case METROLOGY_SPECTR_ACCUMBLOCKDATA_REQUEST:
		{
			ProtocolRequests.rMetrologySendAccumSpectrBlock.requestFlag = 1;
				memcpy((uint8*)(&ProtocolRequests.rMetrologySendAccumSpectrBlock) + 1,pData,sizeof(uint8) * 5);	
				
			//IMDB_Metrology_SendAccumSpectrBlockData(pData);
		}break;
		
		default:
		{
			/*IMDB_SendPacket(IMDB_ERROR,0,0);*/
		}break;
		
	}
}
void IMDB_SendIdentification()
{
		IMDB_SendPacket(IMDB_NUCLIDE_IDENTIFICATION,0,0);
}

void IMDB_SendState()
{
	uint8	stateByte = Process_GetInfo();
		IMDB_SendPacket(IMDB_STATE,&stateByte,1);
}
void IMDB_SendReady()
{
	uint8	stateByte = Process_GetInfo();
		//IMDB_SendPacket(IMDB_SEND_READY,&stateByte,1);
}

void IMDB_SendInsideDose()
{
	float Dose = 0;
	
	if(Process_GetBDMGState())
		Dose = BDMG_GetDose();
	else{
		if(MainSettings.shieldingFactor != 0){
			if(Process_GetBDPSStatus() == DEVICE_READY)
				Dose = BDSP_GetDose() / MainSettings.shieldingFactor;
			else
				Dose = BDGP_GetDose() / MainSettings.shieldingFactor;
			}
	}
	IMDB_SendPacket(IMDB_INSIDE_DOSE,(uint8*)((uint32*)&Dose),sizeof(float));
}

//получаем МД снаружи
void IMDB_SendOutsideDoseRate()
{
	float doseRateVal = 0;
	

	if(Process_GetBDPSStatus() == DEVICE_READY)
		doseRateVal = BDSP_GetDoseRate();
	else{
		if(Process_GetBDGPState())
			doseRateVal = BDGP_GetAverageDoseRate();
		else
			doseRateVal = BDMG_GetCurrentDose() * MainSettings.shieldingFactor;
	}
	
	IMDB_SendPacket(IMDB_OUTSIDE_DOSE_RATE,(uint8*)((uint32*)(&doseRateVal)),sizeof(float));
}
//МД внутри
void IMDB_SendInsideDoseRate()
{
	float doseRateVal = 0;
		
	if(Process_GetBDMGState())
		doseRateVal = BDMG_GetCurrentDose();
	else
	{
		if(Process_GetBDPSStatus() == DEVICE_READY)
			doseRateVal = BDSP_GetDoseRate() / MainSettings.shieldingFactor;
		else
			doseRateVal = BDGP_GetAverageDoseRate() / MainSettings.shieldingFactor;
	}
	
	IMDB_SendPacket(IMDB_INSIDE_DOSE_RATE,(uint8*)((uint32*)(&doseRateVal)),sizeof(float));
}
/*
Проверка признака МД TO-DO исправить если подклчюен только один блок
*/

void 	IMDB_SendInsideDoseFeature()
{
	uint8	currentFeature = BDMG_GetInsideFeature();
	
		IMDB_SendPacket(IMDB_INSIDE_FEATURE,&currentFeature,sizeof(uint8));
}
void	IMDB_SendOutsideDoseFeature()
{
	uint8	currentFeature = BDGP_GetOutsideFeature();
		
		IMDB_SendPacket(IMDB_OUTSIDE_FEATURE,(uint8*)((uint32*)(&currentFeature)),sizeof(uint8));
}
void IMDB_SendMaxAngle()
{
	float maxAngle = 0;
	
			if(Process_GetBDPSStatus() == DEVICE_READY)
				maxAngle = BDSP_GetMaximumAngle();
			else if(Process_GetBDGPState() == DEVICE_READY)
				maxAngle = BDGP_GetMaximumAngle();	 
			else {
				IMDB_SendPacket(IMDB_ERROR,0,0);
				return;
			}
	
		IMDB_SendPacket(IMDB_RADIATION_DIRECT,(uint8*)((uint32*)(&maxAngle)),sizeof(float));
}
void IMDB_SendDoseExceeded(uint8	Number)
{
	IMDB_SendPacket(IMDB_SEND_DOSE_EXCEEDED,&Number,sizeof(uint8));
}
void IMDB_SendDoseRateExceeded(uint8	Number)	//превышение порога МД
{
	IMDB_SendPacket(IMDB_SEND_DOSE_RATE_EXCEEDED,&Number,sizeof(uint8));
	
}
void IMDB_SendNuclideDetection(eDetectionState	State)
{	
	IMDB_SendPacket(IMDB_SEND_NUCLIDE_DETECTION,(uint8*)&State,sizeof(uint8));
}
void IMDB_SendNuclideIdentification(eNuclideNumber Nuclide)
{
	IMDB_SendPacket(IMDB_SEND_NUCLIDE_IDENTIFICATION,(uint8*)&Nuclide,sizeof(uint8));
}

void IMDB_SendCanTransmitError(uint8	erNum)
{
	IMDB_SendPacket(IMDB_CAN_ERROR,&erNum,sizeof(uint8));
}
void IMDB_SendMeasureInPointSuccess()
{
	sSpecModeData CurrentSpecModeData = BDSP_GetMeasureInPointData();
	
	IMDB_SendPacket(IMDB_SEND_SPECTR_ACCUM_SUCCESS,(uint8*)&CurrentSpecModeData,sizeof(sSpecModeData));
}

void IMDB_SendPacket(uint8	cmd,uint8	*pData, int dataSize)
{
	uint8	packSize = 0,i=0;
	uint8	*pPackage = IMDBProtocol_CreatePacket(cmd,pData,dataSize,&packSize);
	
	if(pPackage)
	{
		for(i = 0;i<packSize;i++)
			sendbyte(pPackage[i]);
		
		free(pPackage);
	}
}



void	IMDB_SendAngularDistribution()
{	
	float	*fpData;
	
	//если нет спектрометра
	if(Process_GetBDPSStatus() == DEVICE_NOTREADY)
	{
		//и есть БДГП-Б
		if( Process_GetBDGPState() == DEVICE_READY)
		{
			//и накоплен фон
			if(BDGP_GetBackgroundState()){
				//рассчёт круговой диаграммы
					fpData = BDGP_GetAngularDiagram();//отправляем диаграмму углового распределения
					
					IMDB_SendPacket(IMDB_ANGULAR_DISTRIBUTION,(uint8*)fpData,sizeof(float) * 48);
			}
			else
				IMDB_SendPacket(IMDB_ERROR,0,0);
		}
		else
			IMDB_SendPacket(IMDB_ERROR,0,0);
	}
	//обнаружен готовый и застабилизированный спектрометр
	else if(Process_GetBDPSStatus() == DEVICE_READY)
	{
		if(BDSP_GetBackgroundReady()){
			BDSP_CalculateAngularDiagram(SIZE_128);	//сичтаем диаграмму для 128 канального
			
			fpData = BDSP_GetAngularDiagram();//отправляем диаграмму углового распределения
			if(fpData)
				IMDB_SendPacket(IMDB_ANGULAR_DISTRIBUTION,(uint8*)fpData,sizeof(float) * 48);
			else
				IMDB_SendPacket(IMDB_ERROR,0,0);
		}
		else
			IMDB_SendPacket(IMDB_ERROR,0,0);
	}
	else
		IMDB_SendPacket(IMDB_ERROR,0,0);
	

}
void IMDB_SendAccumSpectrSuccess(uint8	Channel)
{
	IMDB_SendPacket(METROLOGY_SEND_ACCUMSPECTR_SUCCESS,(uint8*)&Channel,sizeof(uint8));	
}
void IMDB_SendCalibrationState(eCalibrationStatus Status)
{
	IMDB_SendPacket(IMDB_SEND_CALIBRATION_STATE,(uint8*)&Status,sizeof(uint8));
}

void	IMDB_Process()
{

	uint8	i = 0;
	uint8	BDMGReady = Process_GetBDMGState();
	uint8	BDGPReady = Process_GetBDGPState();
	uint8	BDSPReady = Process_GetBDPSStatus();
		if(BDSPReady == 2)
			BDSPReady = 1;
	
	
	IMDB_ReceiveByteReading();
	
	if(ProtocolRequests.rState){
		IMDB_SendState();
		ProtocolRequests.rState = 0;
	}
	
	if(ProtocolRequests.rOutsideDoseRate){
		IMDB_SendOutsideDoseRate();
		ProtocolRequests.rOutsideDoseRate = 0;
	}
	
	if(ProtocolRequests.rOutsideFeature){
		IMDB_SendOutsideDoseFeature();
		ProtocolRequests.rOutsideFeature = 0;
	}
	
	if(ProtocolRequests.rInsideDoseRate){
		IMDB_SendInsideDoseRate();
		ProtocolRequests.rInsideDoseRate = 0;
	}
	
	if(ProtocolRequests.rInsideFeature){
		IMDB_SendInsideDoseFeature();
		ProtocolRequests.rInsideFeature = 0;
	}
	
	if(ProtocolRequests.rInsideDose){
		IMDB_SendInsideDose();
		ProtocolRequests.rInsideDose = 0;
	}
	
	if(ProtocolRequests.rAngularDistribution){
		IMDB_SendAngularDistribution();	
		ProtocolRequests.rAngularDistribution = 0;
	}
	
	if(ProtocolRequests.rMaxAngle){
		IMDB_SendMaxAngle();	
		ProtocolRequests.rMaxAngle = 0;
	}
	
	if(ProtocolRequests.rMetrologySendCurrentSpectrProcess){
		IMDB_Metrology_SendCurrentSpectrData();
	}
	
	if(ProtocolRequests.rMetrologySendAccumSpectrBlock.requestFlag){
		IMDB_Metrology_SendAccumSpectrBlockData((uint8*)(&ProtocolRequests.rMetrologySendAccumSpectrBlock) + 1);
		
		ProtocolRequests.rMetrologySendAccumSpectrBlock.requestFlag = 0;
	}
	
	//пришёл запрос на чтение параметров из блоков
	if(ProtocolRequests.rMetrologyReadBDParametrs){
		IMDB_Metrology_AllBDParametrRequest();
			ProtocolRequests.rMetrologyReadBDParametrs = 0;
	}
	if(ProtocolRequests.rMetrologyWriteBDParametrs){
		IMDB_Metrology_AllBDWriteParametr();
		ProtocolRequests.rMetrologyWriteBDParametrs = 0;
	}
	

	//анализируем все флаги устройств
	if(( ((!BDMGReady)||(BDMGReady && ProtocolRequests.rBDMGReadParametrsCallback)) &&
			 ((!BDGPReady)||(BDGPReady && ProtocolRequests.rBDGPReadParametrsCallback)) &&
			 ((!BDSPReady)||(BDSPReady && ProtocolRequests.rBDPSReadParametrsCallback))) && (BDMGReady || BDGPReady || BDSPReady))
	{
		IMDB_Metrology_SendBDParametrs();
		IMDB_ResetAllBDSignalCallback();
	}
	
	
	//анализируем все флаги устройств
	if( ( ((!BDMGReady)||(BDMGReady && ProtocolRequests.rBDMGWriteParametrsCallback)) &&
			  ((!BDGPReady)||(BDGPReady && ProtocolRequests.rBDGPWriteParametrsCallback)) &&
			  ((!BDSPReady)||(BDSPReady && ProtocolRequests.rBDPSWriteParametrsCallback))) && (BDMGReady || BDGPReady || BDSPReady))
	{
		IMDB_SendPacket(METROLOGY_WRITEPARAM_RESPONSE,0,0);
		IMDB_ResetAllBDSignalCallback();
	}	

	
	
	//CALLBACK-И
	//обнаружение источника
	if(ProtocolRequests.rNuclideDetectionCallback){
		IMDB_SendNuclideDetection(E_DETECTION_START);
		ProtocolRequests.rNuclideDetectionCallback = 0;
	}
	//окончание обнаружения источника
	if(ProtocolRequests.rNuclideEndDetectionCallback){
		IMDB_SendNuclideDetection(E_DETECTION_END);
		ProtocolRequests.rNuclideEndDetectionCallback = 0;
	}
	//идентификация источника
	if(ProtocolRequests.rNuclideIdentificationCallback.requestFlag){
		IMDB_SendNuclideIdentification(ProtocolRequests.rNuclideIdentificationCallback.nuclideIndex);
		ProtocolRequests.rNuclideIdentificationCallback.requestFlag = 0;
	}
	//окончание идентификации источника
	if(ProtocolRequests.rNuclideIdentificationEndCallback){
		IMDB_SendNuclideIdentification(E_NO_NUCLIDE);
		ProtocolRequests.rNuclideIdentificationEndCallback = 0;
	}
	//завершение измерения в точке
	if(ProtocolRequests.rMeasureInPointEndCallback){
		IMDB_SendMeasureInPointSuccess();
		ProtocolRequests.rMeasureInPointEndCallback = 0;
	}
	
	for(i = 0;i<CHANNELS;i++)
	{
		if(ProtocolRequests.rSpectrAccumChanSuccess[i]){
			IMDB_SendAccumSpectrSuccess(i);
			ProtocolRequests.rSpectrAccumChanSuccess[i] = 0;
			break;
		}
	}

	
}
/**********************************************************************************
/**********************************************************************************
*****************ФУНКЦИИ ДЛЯ УСТАНОВКИ В ОЧЕРЕДЬ СООБЩЕНИЙ ОТ  ИМД-Б***************
**********************************************************************************
**********************************************************************************/
void	IMDB_ResetAllBDSignalCallback()
{
	ProtocolRequests.rBDMGReadParametrsCallback = 0;
	ProtocolRequests.rBDGPReadParametrsCallback = 0;
	ProtocolRequests.rBDPSReadParametrsCallback = 0;
	
	ProtocolRequests.rBDMGWriteParametrsCallback = 0;
	ProtocolRequests.rBDGPWriteParametrsCallback = 0;
	ProtocolRequests.rBDPSWriteParametrsCallback = 0;
}

void	IMDB_BDMGGetParametrsSignalCallback()
{
	ProtocolRequests.rBDMGReadParametrsCallback = 1;
}

void	IMDB_BDGPGetParametrsSignalCallback()
{
	ProtocolRequests.rBDGPReadParametrsCallback = 1;
}

void	IMDB_BDPSGetParametrsSignalCallback()
{
	ProtocolRequests.rBDPSReadParametrsCallback = 1;
}

void	IMDB_BDPSWriteParametrsSignalCallback()
{
	ProtocolRequests.rBDPSWriteParametrsCallback = 1;
}

void	IMDB_BDGPWriteParametrsSignalCallback()
{
	ProtocolRequests.rBDGPWriteParametrsCallback = 1;
}

void	IMDB_BDMGWriteParametrsSignalCallback()
{
	ProtocolRequests.rBDMGWriteParametrsCallback = 1;
}

void IMDB_NuclideDetectionSignalCallback()
{
	ProtocolRequests.rNuclideDetectionCallback = 1;
}
void IMDB_NuclideEndDetectionSignalCallback()
{
	ProtocolRequests.rNuclideEndDetectionCallback = 1;
}
void IMDB_SendAccumSpectrSuccessSignalCallback(uint8	Channel)
{
	ProtocolRequests.rSpectrAccumChanSuccess[Channel] = 1;
}
void	IMDB_NuclideIdentificationSignalCallback(uint8	Nuclide)
{
	ProtocolRequests.rNuclideIdentificationCallback.requestFlag = 1;
	ProtocolRequests.rNuclideIdentificationCallback.nuclideIndex = Nuclide;
}
void	IMDB_NuclideIdentificationEndSignalCallback()
{
	ProtocolRequests.rNuclideIdentificationEndCallback = 1;
}

void IMDB_SendMeasureInPointEndSignalCallback()
{
	ProtocolRequests.rMeasureInPointEndCallback = 1;
}

/**********************************************************************************
/**********************************************************************************
*****************ФУНКЦИИ ДЛЯ МЕТРОЛОГИЧЕСКОГО ПРОТОКОЛА****************************
**********************************************************************************
**********************************************************************************/
void IMDB_Metrology_AllBDParametrRequest()
{
	/*
	Алгоритм запроса параметров от блоков детектирования
	*/
	
	//запрашиваем параметры от БД
	if(Process_GetBDMGState() == DEVICE_READY){
		BDMG_ParametrRequest(0x94,0);//запрашиваем первый параметр, остальные выдаются по цепочке
	}
	if(Process_GetBDGPState() == DEVICE_READY){
		BDGP_ParametrRequest(1,0x93,0);//запрашиваем первый параметр, остальные выдаются по цепочке
	}
	//спектрометр либо готов либо в режиме ожидания
	if(Process_GetBDPSStatus() != DEVICE_NOTREADY){
		//тут запрос параметров спектрометра
		BDSP_ParametrRequests();
	}
}
void IMDB_Metrology_AllBDWriteParametr()
{
	//запрашиваем параметры от БД
	if(Process_GetBDMGState() == DEVICE_READY){
		BDMG_StartWritingParametrs();
		
		System_Delay(100000);
	}
	if(Process_GetBDGPState() == DEVICE_READY){
		BDGP_StartWritingParametrs(0);
		System_Delay(100000);
	}
	//спектрометр либо готов либо в режиме ожидания
	if(Process_GetBDPSStatus() != DEVICE_NOTREADY){
		//тут запрос параметров спектрометра
		BDSP_StartWritingParametrs();
		
	}
}

void IMDB_Metrology_SendBDParametrs()
{
	
	
	uint16	sizeBDMGParam = sizeof(tBDMGParametrs);
	uint16	sizeBDGPParam = sizeof(tBDGPParametrs);
	uint16	sizeBDSPParam = sizeof(tBDSPParametrs);
	
	uint8		*pParamPack;
	uint16	PackSize = sizeBDMGParam + sizeBDGPParam * 2 + sizeBDSPParam;

		pParamPack = (uint8*)malloc(PackSize);
	
		if(pParamPack){
			memcpy(pParamPack,BDMG_GetParametrs(),sizeBDMGParam);
			memcpy(pParamPack + sizeBDMGParam,BDGP_GetParametrs(),sizeBDGPParam * 2);
			memcpy(pParamPack + sizeBDMGParam + sizeBDGPParam * 2,BDSP_GetParametrs(),sizeBDSPParam);
			
			IMDB_SendPacket(METROLOGY_READPARAM_RESPONSE,pParamPack,PackSize);
			
		
			free(pParamPack);
		}

	
}
//запись параметров метрологии
void IMDB_Metrology_WriteBDParametrs(uint8	*pData)
{
	uint16	sizeBDMGParam = sizeof(tBDMGParametrs);
	uint16	sizeBDGPParam = sizeof(tBDGPParametrs);
	uint16	sizeBDSPParam = sizeof(tBDSPParametrs);
	
	if(pData)
	{
		memcpy(BDMG_GetParametrs(),pData,sizeBDMGParam);
		memcpy(BDGP_GetParametrs(),pData + sizeBDMGParam,sizeBDGPParam * 2);
		memcpy(BDSP_GetParametrs(),pData + sizeBDMGParam + sizeBDGPParam * 2,sizeBDSPParam);
	}
	else
		IMDB_SendPacket(IMDB_ERROR,0,0);
	
}
void	IMDB_Metrology_DefaultSettings()
{
	SETTINGS_Default();
	IMDB_SendPacket(METROLOGY_DEFAULT_SETTINGS_RESPONSE,0,0);
	
	System_Reset();
}
//чтение параметров из БС
void	IMDB_Metrology_ReadBSParametrs()
{
	IMDB_SendPacket(METROLOGY_READ_SETTINGS_RESPONSE,(uint8*)&MainSettings,sizeof(tSettings));
}
void IMDB_Metrology_WriteBSParametrs(uint8	*pData)
{
	uint8	i = 0;
	
	if(pData)
	{
		memcpy((uint8*)&MainSettings,(uint8*)pData,sizeof(tSettings));
		
		
		SETTINGS_Save();
		IMDB_SendPacket(METROLOGY_WRITE_SETTINGS_RESPONSE,0,0);
	}
	else
		IMDB_SendPacket(IMDB_ERROR,0,0);
}
void IMDB_SendError()
{
	IMDB_SendPacket(IMDB_ERROR,0,0);
}
void	IMDB_SendStartMeasureInPoint(uint8	PointNumber)
{
	if(Process_GetBDPSStatus() == DEVICE_READY)
	{
		if(PointNumber == 1 || PointNumber == 2){			
			Process_SetStatus(IMDB_SPECTR_ACCUMULATION);
				BDSP_SetMeasureNumber(PointNumber);
				BDSP_StartSpectrAccumulation(ACCUM_AND_MEASURE);
			
			IMDB_SendPacket(IMDB_START_MEASURE,(uint8*)&PointNumber,sizeof(uint8));
		}
		else
			IMDB_SendError();
	}
	else
		IMDB_SendError();
}

void IMDB_Metrology_StartBD(uint8	*pData)
{
	/*
	1. Проверяем, в каком режиме находится ИМДБ, если не в штатном и не в режиме настройки, то шлём еррор
			если в штатном, то переключаем в настройку и проверяем на надичие БД
		1 - БДГП-С
		2 - БДГП-Б I
		3 - БДГП-Б II
		4 - БДМГ-Б I
		5 - БДМГ-Б II
		6 - БДМГ-Б III
	*/
	uint8	bdIndex = pData[0];	//получаем индекс БД
	
	if(bdIndex > 0 && bdIndex < 7){
		//при приёме останавливаем все блоки
		Process_IMDBStop();
		//запоминаем индекс выбранного диапазона БД
		MetrologyData.rangeIndex = bdIndex;	
		
		if(Process_GetOperationMode() == IMDB_FULLTIME_WORK)	//метрология в режиме настройки
		{
			switch(bdIndex)
			{
				case 1:BDSP_Start();break;
				case 2:BDGP_ForcedStart(2);break;
				case 3:BDGP_ForcedStart(3);break;
				case 4:BDMG_ForcedStart(1);break;
				case 5:BDMG_ForcedStart(2);break;
				case 6:BDMG_ForcedStart(3);break;
				
			}
			

			
			IMDB_SendPacket(METROLOGY_STARTBD_RESPONSE,(uint8*)&bdIndex,sizeof(uint8));
		}
		else
			IMDB_SendError();
	}
	else
		IMDB_SendError();
	
}
void IMDB_Metrology_GetBDData()
{
		if(MetrologyData.rangeIndex == 1)
		{
			//считаем угол
			BDSP_CalculateAngularDiagram(SIZE_128);
			
			MetrologyData.doseRate = BDSP_GetDoseRate();
			MetrologyData.currentAngle = (uint16)BDSP_GetMaximumAngle();
		}
		if(MetrologyData.rangeIndex == 2 || MetrologyData.rangeIndex == 3)
		{
			BDGP_GetAngularDiagram();
				MetrologyData.doseRate = BDGP_GetCurrentDoseRate();
				MetrologyData.currentAngle = (uint16)BDGP_GetMaximumAngle();
		}
		if(MetrologyData.rangeIndex == 4 || MetrologyData.rangeIndex == 5 || MetrologyData.rangeIndex == 6)
		{
			MetrologyData.doseRate = BDMG_GetCurrentDose();
			MetrologyData.currentAngle = 0;
		}
	

	
	IMDB_SendPacket(METROLOGY_DATA_RESPONSE,(uint8*)&MetrologyData,sizeof(tMetrologyData));
	
}
void	IMDB_Metrology_SpectrometryMode(uint8	*pData)
{
	/*
	Получили команду на передачу/накопление спектров
	*/
	switch(pData[0])
	{
		case 0:{
			
			
			Process_SetStatus(IMDB_FULLTIME_WORK);
						
				ProtocolRequests.rMetrologySendCurrentSpectr = 0;
			
			IMDB_SendPacket(METROLOGY_SPECTR_ACCUMMODE_RESPONSE,(uint8*)pData,3);
		}break;	//отмена операции
		
		case 1:{
			Process_SetStatus(IMDB_FULLTIME_WORK);
				ProtocolRequests.rMetrologySendCurrentSpectr = 1;	//ставим флаг отправки спектров 4х спектров по 128 каналам
				ProtocolRequests.rMetrologySendCurrentSpectrProcess = 1;
			
			IMDB_SendPacket(METROLOGY_SPECTR_ACCUMMODE_RESPONSE,(uint8*)pData,3);
		}break;
		
		case 2:{
			Process_SetStatus(IMDB_SPECTR_ACCUMULATION);
			ProtocolRequests.rMetrologySendCurrentSpectr = 0;	
			
				MainSettings.accumulationTime = (uint16) (pData[2]<<8)|pData[1];
				BDSP_StartSpectrAccumulation(ACCUM_ONLY);
			
			IMDB_SendPacket(METROLOGY_SPECTR_ACCUMMODE_RESPONSE,(uint8*)pData,3);
		}break;
		
		default: IMDB_SendError(); break;
	}
}

void	IMDB_Metrology_SendCurrentSpectrData()
{

	uint16	spectrIndex = 0;
	
	//если нет запроса на передачу спектра то выходим из функции
	if(!ProtocolRequests.rMetrologySendCurrentSpectr)
		return;
	
	ProtocolRequests.rMetrologySendCurrentSpectrProcess = 0;
	
	//visualSpecBuf = (uint8*)malloc(sizeof(uint16) * 128 * 4);
	
	//if(visualSpecBuf)
	{
	//	memset((uint8*)visualSpecBuf,0,sizeof(uint8) * SPECTR_SIZE);
	
		
		
		memcpy( visualSpecBuf,			 (uint16*)BDSP_DEBUG_GetQuerySumSpectr(0),sizeof(uint16) * 128);
		memcpy((visualSpecBuf + 256),(uint16*)BDSP_DEBUG_GetQuerySumSpectr(1),sizeof(uint16) * 128);
		memcpy((visualSpecBuf + 512),(uint16*)BDSP_DEBUG_GetQuerySumSpectr(2),sizeof(uint16) * 128);
		memcpy((visualSpecBuf + 768),(uint16*)BDSP_DEBUG_GetQuerySumSpectr(3),sizeof(uint16) * 128);
		
		
		spectrIndex = 0;
		
								do
								{
									IMDB_SendPacket(0xF0 + spectrIndex,(uint8 *)(visualSpecBuf + (spectrIndex * 128) ),128);

									spectrIndex++;
									
									_srvwdt_();
								}
								while(spectrIndex < 8);
								
		
	}
	ProtocolRequests.rMetrologySendCurrentSpectrProcess = 1;
							
}
void	IMDB_Metrology_SendAccumSpectrBlockData(uint8	*pData)
{
	uint8 i = 0;
	
	uint8		*pSendData;
	uint8		Channel = pData[0];	//номер канала
	
	uint16	Start = (uint16)(pData[2] << 8)|pData[1];	//начальнйый индекс спектра
	uint16	Count	= (uint16)(pData[4] << 8)|pData[3];		//кол-во спектров

	
	if(Channel >= 0 && Channel < 4){
		if(Start >= 0 && Start < 1024){
			if(Count > 0 && Count < 62){
				if((Start + Count) <= 1024){
				uint8	*pSpectrData = BDSP_GetAccumulationSpectr(Channel);
				
						pSendData = (uint8*)malloc(sizeof(uint32) * Count + sizeof(uint8) * 5);

						memcpy((uint8*)pSendData,(uint8*)pData,sizeof(uint8) * 5);
						memcpy((uint8*)(pSendData+5),pSpectrData + (Start * 4),sizeof(uint32) * Count);

						

						IMDB_SendPacket(METROLOGY_SPECTR_ACCUMBLOCKDATA_RESPONSE,pSendData,sizeof(uint32) * Count + sizeof(uint8) * 5);
						//IMDB_SendError();
						free(pSendData);
				
				}
				else
					IMDB_SendError();
			}
			else
				IMDB_SendError();
		}
		else
			IMDB_SendError();
	}
	else
		IMDB_SendError();		
}

/**********************************************************************************
/**********************************************************************************
**********************ФУНКЦИИ ДЛЯ ОТЛАДОЧНОГО ПРОТОКОЛА****************************
**********************************************************************************
**********************************************************************************/
void	IMDB_DEBUG_SendDataMsg(uint8	*pData,uint8	Index,uint8	DataType)
{
	#ifdef DEBUG_MSG_ENABLE
		uint8	pSendData[6];
		
			pSendData[0] = Index;
			pSendData[1] = DataType;
			
		memcpy((uint8*)(pSendData + 2),(uint8*)pData,sizeof(float));
		
		IMDB_SendPacket(IMDB_DEBUG_DATA_MSG,pSendData,sizeof(uint8) *6);
	#endif
}

