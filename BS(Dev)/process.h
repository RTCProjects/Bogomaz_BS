#ifndef _PROCESS_H
#define _PROCESS_H

#define BDGPS_RPT_COUNTS	3//кол-во повторных запросов БДГП-Са

#include "main.h"

typedef enum
{
	IMDB_INITIALIZATION 						= 0x00,
	IMDB_OPERATION_CONTROL					= 0x01,
	IMDB_FULLTIME_WORK							= 0x02,
	IMDB_SETTINGS_MODE		 					= 0x03,
	IMDB_SPECTR_ACCUMULATION	 			= 0x04,	//спектрометрический режим
	IMDB_CALIBRATION_I		 					= 0x05,
	IMDB_CALIBRATION_II		 					= 0x06,
	IMDB_CALIBRATION_III		 				= 0x07,
	IMDB_CALIBRATION_IV				 			= 0x08,
	IMDB_BACKGROUND_ACCUMULATION	 	= 0x09,
}IMDB_Status;

typedef enum
{
	DEVICE_WAITING = 2,
	DEVICE_READY = 1,
	DEVICE_NOTREADY = 0
}Device_Status;

typedef struct
{
	uint8	rRangeFlag[4];
}tProcessDoseExceeded;

typedef struct
{
	uint8	rRangeFlag[5];
}tProcessDoseRateExceeded;

void	Process_IMDBStart(void);
void	Process_IMDBStop(void);

void	Process_Initializetion(void);
void	Process_SetStatus(IMDB_Status Status);
void	Process_BDGPStatus(Device_Status Status);
void	Process_BDMGStatus(Device_Status Status);
void	Process_BDPSStatus(Device_Status Status);
void		Process_ResetWaitCounter(void);
uint16	Process_GetWaitCounter(void);

uint8	Process_GetInfo(void);

uint8	Process_GetBDMGState(void);
uint8	Process_GetBDGPState(void);
uint8 Process_GetBDPSStatus(void);

IMDB_Status	Process_GetOperationMode();
/*
void 	Process_BackgroundAccumulation(uint16	*specArr,uint8	channel);
void 	Process_SpectrAccumulation(uint16	*specArr,uint8	channel);

uint16	*Process_GetCurrentSpectr(uint8	Channel);*/
#endif
