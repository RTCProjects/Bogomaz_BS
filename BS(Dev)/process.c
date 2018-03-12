#include "process.h"
#include "protocol.h"
#include "devices.h"
#include "settings.h"

//блоки
#include "bdmg.h"
#include "bdgp.h"
#include "bdsp.h"
//
#include "can.h"
#include "system.h"

uint8	processByte;	//байт статуса ИМД_Б

__IO float	counter = 0;
__IO static	uint32	mainCounter = 0;
__IO static uint8		bdgpsRptCounter = 0;	//счётчик повторной инициалиазации БДГП-Са
static uint16	waitCounter = 0;

static tProcessDoseExceeded 		ProcessDoseExceeded;
static tProcessDoseRateExceeded	ProcessDoseRateExceeded;
void	Process_Initializetion(void)
{
		processByte = 0;
		mainCounter = 0;
		waitCounter = 0;
		bdgpsRptCounter = 0;
	
	memset((uint8*)&ProcessDoseExceeded,0,sizeof(tProcessDoseExceeded));
	memset((uint8*)&ProcessDoseRateExceeded,0,sizeof(tProcessDoseRateExceeded));
	

}
void	Process_ResetWaitCounter()
{
	waitCounter = 0;
}
uint16	Process_GetWaitCounter()
{
	return waitCounter;
}

void	Process_SetStatus(IMDB_Status Status)	//ставим статус ИМДБ
{	
	
	processByte = processByte & 0xF0;
	processByte = processByte | Status;
	
}
void	Process_BDGPStatus(Device_Status Status)	//ставим статус БДГП
{
	if(Status)
		SET_BIT(processByte,4);
	else
		CLEAR_BIT(processByte,4);
	
	
}
void	Process_BDMGStatus(Device_Status Status)	//ставим статус БДМГ
{
	if(Status)
		SET_BIT(processByte,5);
	else
		CLEAR_BIT(processByte,5);
}
void	Process_BDPSStatus(Device_Status Status)	//статус для БДГП-С
{
	CLEAR_BIT(processByte,6);
	CLEAR_BIT(processByte,7);
	
	
	if(Status == DEVICE_WAITING)
		SET_BIT(processByte,7);
	if(Status == DEVICE_READY)
		SET_BIT(processByte,6);

}
uint8	Process_GetBDMGState()
{
	if( ((processByte >> 5) & 0x1))return 1;
	else return 0;
}
uint8	Process_GetBDGPState()
{
	if( ((processByte >> 4) & 0x1))return 1;
	else return 0;
}
uint8 Process_GetBDPSStatus()
{
	if( ((processByte >> 6) & 0x1))return 1;			//ready
	else if( ((processByte >> 7) & 0x1))return 2;	//waiting
	else return 0;
}
	

uint8	Process_GetInfo() //получаем байт состояния ИМДБ
{
	return processByte;
}


IMDB_Status	Process_GetOperationMode()
{
	return (processByte & 0x0F);
}

void	Process_MainProc()	interrupt 0x20
{
	float insideDoseRate = 0;
	float outsideDoseRate = 0;
	float	insideDose = 0;
	
	float	fBackgroundQuantile = 0;
	uint8	identificationValue = 0;
	
	//IMDB_SendPacket(IMDB_PING,0,0);
	/*pCanInfo = (uint8*)malloc(sizeof(uint8) * 64);
	
	if(pCanInfo)
	{
		for(i = 0;i<8;i++)
		{
			
			
			*((uint16*)&msgData[0]) = CAN_Message_16[i].MODATALL;
			*((uint16*)&msgData[2]) = CAN_Message_16[i].MODATALH;
			*((uint16*)&msgData[4]) = CAN_Message_16[i].MODATAHL;
			*((uint16*)&msgData[6]) = CAN_Message_16[i].MODATAHH;
			
			memcpy(pCanInfo + ( i * 8),(uint8*)msgData,sizeof(uint8) * 8);
		
			
		}
		IMDB_SendPacket(IMDB_PING,(uint8*)pCanInfo,sizeof(uint8) * 64);
		free(pCanInfo);
	}*/
	
	switch(Process_GetOperationMode())
	{
			case IMDB_INITIALIZATION:
			{
				//3 сек для инициализации
				if(mainCounter == 3)
				{
					
					
					/*
					1. Если есть БДГП, то проверяем МД снаружи.
					2. Если МД меньше 0.1мГр/ч, то включаем HV для ФЭУ
					3. Если БДГП нет, то меряем МД БДГМом и выполняем п2 с учётом КЭ.
					*/
					//ищем спектрометр
					if(Process_GetBDPSStatus())
					{
						if(BDSP_ChannelInitCheck() != CHK_OK)
						{
							Process_BDPSStatus(DEVICE_NOTREADY);
							//ищем БДГП-С повторно
							if(bdgpsRptCounter < BDGPS_RPT_COUNTS){
									mainCounter = 1;
									BDSP_Reset();
									bdgpsRptCounter++;
									
									return;
							}
						}
						else
						{
						
						
							if(Process_GetBDGPState() ){//БДГП обнаружен
								BDGP_Start();	//запускаем БДГП
							}
							else if(Process_GetBDMGState() ){//если его нет, то меряем БДМГом
								BDMG_Start();
								
							}
							//если нет обоих дозиметров то переходим в режим контроля функционирования
							else {
								Process_SetStatus(IMDB_OPERATION_CONTROL);
							}
						}
					}
					else{
						//если спектрометра нет
						if(!Process_GetBDGPState() && !Process_GetBDMGState())//И БД тоже нет, то выдаём статус готовности неготовности
							IMDB_SendReady();
					}
					
					Process_SetStatus(IMDB_OPERATION_CONTROL);
			
				}
			}break;
			
			case IMDB_OPERATION_CONTROL:
			{				
				//ещё 10 сек для измерения внешней МД
				
				//если есть спектрометр, то ждём замера МД, анализируем её и даём команду на работу с ним
				/*
				*/
				if(Process_GetBDPSStatus())
				{				
					if(mainCounter == 10){
						BDGP_Stop();
						BDMG_Stop();
						
						//закончили измерения, теперь получаем внешнюю МД.
						if(Process_GetBDGPState()){
							outsideDoseRate = BDGP_GetAverageDoseRate();
						}
						else if(Process_GetBDMGState()){
							outsideDoseRate = BDMG_GetCurrentDose() * MainSettings.shieldingFactor;	//если нет БДГП, то считаем МД внеш, как МД внутри * КЭ
						}
						else{
							outsideDoseRate = 0;	//TODO получить МД со спектрометра
						}
					}
					//ожидаем стабилижации
					if(mainCounter > 10 && mainCounter < 70)
					{
						if(Process_GetBDPSStatus() == DEVICE_READY)//дождались стабилизации БДГП-С
						{
							Process_SetStatus(IMDB_BACKGROUND_ACCUMULATION);
							BDSP_Start();	//запускаем только спектрометр
							
							IMDB_SendReady();
						}
					}
				}
				//в противном случае сразу переходим в режим штатной работы без спектрометра
				else
				{
					//если есть БДГП-Б, то копим фон
					if(Process_GetBDGPState()/* || Process_GetBDMGState()*/)
					{	
						Process_SetStatus(IMDB_BACKGROUND_ACCUMULATION);
						Process_IMDBStart();
						
						IMDB_SendReady();
					}
					//если нет БДГП-Б но есть БДМГ-Б то сразу переходим в штатный режим работы
					else if(Process_GetBDMGState())
					{
							Process_SetStatus(IMDB_FULLTIME_WORK);
							Process_IMDBStart();
						
							IMDB_SendReady();
					}
				
					else
					{
						
					}
				}
				
			}break;
			
			case IMDB_BACKGROUND_ACCUMULATION:
			{
				
			}break;
			
			case IMDB_FULLTIME_WORK:
			{
				/*
					Режим штатной работы
				
					- контролируем мощности дозы снаружи и внутри
					- в зависимости от МД включаем или выключаем спектрометр
					- вызываем функции рассчёта превышения порога по фону
				*/
				//выссчитываем МД
				
					//если есть спектрометр и у него накоплен фон

					
					//если есть БДГП-Б
					if(Process_GetBDGPState() == DEVICE_READY){					
							outsideDoseRate = BDGP_GetAverageDoseRate();//получаем текущую МД
					
								
					}
					else
							outsideDoseRate = BDMG_GetCurrentDose() * MainSettings.shieldingFactor;
					
					if(Process_GetBDMGState() == DEVICE_READY)
							insideDoseRate = BDMG_GetCurrentDose();
					else
						insideDoseRate = BDGP_GetAverageDoseRate() / MainSettings.shieldingFactor;
					
						insideDose = BDMG_GetDose();
						
							//события порогов по МД
							//порог 1
							if(outsideDoseRate > MainSettings.limitDetect[1]){
								
								if(!ProcessDoseRateExceeded.rRangeFlag[0]){
									IMDB_SendDoseRateExceeded(1);
									ProcessDoseRateExceeded.rRangeFlag[0] = 1;
									}
							}
							else{
								if(ProcessDoseRateExceeded.rRangeFlag[0] == 1){
									ProcessDoseRateExceeded.rRangeFlag[0] = 0;
									IMDB_SendDoseRateExceeded(0);
								}
							}
							//порог 2
							if(outsideDoseRate > MainSettings.limitDetect[2]){
								
								if(!ProcessDoseRateExceeded.rRangeFlag[1]){
									IMDB_SendDoseRateExceeded(2);
									ProcessDoseRateExceeded.rRangeFlag[1] = 1;
								}
							}
							else{
								if(ProcessDoseRateExceeded.rRangeFlag[1] == 1){
									ProcessDoseRateExceeded.rRangeFlag[1] = 0;
									IMDB_SendDoseRateExceeded(0);
								}
							}
							//порог 3
							if(outsideDoseRate > MainSettings.limitDetect[3]){
								
								if(!ProcessDoseRateExceeded.rRangeFlag[2]){
								IMDB_SendDoseRateExceeded(3);
								ProcessDoseRateExceeded.rRangeFlag[2] = 1;
								}
							}
							else{
								if(ProcessDoseRateExceeded.rRangeFlag[2] == 1){
									ProcessDoseRateExceeded.rRangeFlag[2] = 0;
									IMDB_SendDoseRateExceeded(0);
								}
							}
							//порог 4
							if(outsideDoseRate > MainSettings.limitDetect[4]){
								
								if(!ProcessDoseRateExceeded.rRangeFlag[3]){
									IMDB_SendDoseRateExceeded(4);
									ProcessDoseRateExceeded.rRangeFlag[3] = 1;
								}
							}
							else{
								if(ProcessDoseRateExceeded.rRangeFlag[3] == 1){
									ProcessDoseRateExceeded.rRangeFlag[3] = 0;
									IMDB_SendDoseRateExceeded(0);
								}
							}
							/*
							//порог 5
							if(outsideDoseRate > MainSettings.limitDetect[5]){
								if(!ProcessDoseRateExceeded.rRangeFlag[4]){
									IMDB_SendDoseRateExceeded(6);
									ProcessDoseRateExceeded.rRangeFlag[4] = 1;
								}
							}
							else{
								if(ProcessDoseRateExceeded.rRangeFlag[4] == 1){
									ProcessDoseRateExceeded.rRangeFlag[4] = 0;
									IMDB_SendDoseRateExceeded(0);
								}
							}
							*/

							
							//высылаем события порога по Дозе
							if(insideDose > MainSettings.limitDetect[6])
								if(ProcessDoseExceeded.rRangeFlag[0] == 0){
									ProcessDoseExceeded.rRangeFlag[0] = 1;
										IMDB_SendDoseExceeded(1);
								}
								
							if(insideDose > MainSettings.limitDetect[7])
								if(ProcessDoseExceeded.rRangeFlag[1] == 0){
									ProcessDoseExceeded.rRangeFlag[1] = 1;
										IMDB_SendDoseExceeded(2);
								}
						
							if(insideDose > MainSettings.limitDetect[8])
								if(ProcessDoseExceeded.rRangeFlag[2] == 0){
									ProcessDoseExceeded.rRangeFlag[2] = 1;
										IMDB_SendDoseExceeded(3);
								}
							
							if(insideDose > MainSettings.limitDetect[9])
								if(ProcessDoseExceeded.rRangeFlag[3] == 0){
									ProcessDoseExceeded.rRangeFlag[3] = 1;
										IMDB_SendDoseExceeded(4);
								}
								

					
					
			}break;
			
			case IMDB_SPECTR_ACCUMULATION:
			{

					
			}break;
	}
	mainCounter++;
	waitCounter++;
	
	
	if(waitCounter >= WAIT_TIMEOUT){
		/*Process_IMDBStop();
		T0R = 0;
		System_LEDState(LED_BLINKY_LOW);
		*/
	}
	
}

void	Process_IMDBStart()
{
	if(Process_GetBDGPState() == DEVICE_READY)
		BDGP_Start();
	if(Process_GetBDMGState() == DEVICE_READY)
		BDMG_Start();
	if(Process_GetBDPSStatus() == DEVICE_READY)
		BDSP_Start();
}
void	Process_IMDBStop()
{
	BDGP_Stop();
	BDMG_Stop();	
	BDSP_Stop();
}
