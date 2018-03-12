#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "main.h"

//Команды отладочного протокола обмена
#define IMD_EVENTS	0x05
#define EVENT_DEBUG_TRACE	0x01

//Команды ответа ИМД-Б
#define IMDB_STATE									0x21	//состояние ИМД=Б
#define IMDB_SHIELDING_FACTOR				0x22	//коэф. экранирования
#define IMDB_SPECTRACC_TIME					0x2B	//время накопления спектр. инф.
#define IMDB_LIMIT_DETECT						0x2C	//порог обнаружения
#define IMDB_START_MEASURE					0x2D	//ответ на запуск измерений в точке
#define IMDB_ERROR									0x80	//ошибка
//DEBUG
#define IMDB_HARDWAREFAIL						0x81	//отказ оборудования	
#define IMDB_PING										0x82	//PING
#define IMDB_MEMORY_ERR							0x83	//Ошибка выделения памяти
#define IMDB_CAN_ERROR							0x84	//ошибки передачи по CAN шине
#define IMDB_SERVICE								0x90	//сервисное сообщение БС
#define IMDB_16BITWORD							0x91



#define IMDB_OUTSIDE_DOSE_RATE			0x23	//МД снаружи ТС
#define IMDB_OUTSIDE_FEATURE				0x24	//признак МД снаружи ТС
#define IMDB_INSIDE_DOSE_RATE				0x25	//МД внутри ТС
#define IMDB_INSIDE_FEATURE					0x26	//признак МД внутри ТС
#define IMDB_INSIDE_DOSE						0x27	//доза внутри ТС
#define IMDB_RADIATION_DIRECT				0x28	//направление на максимум радиационного загрязнения
#define IMDB_ANGULAR_DISTRIBUTION		0x29	//запрос углового распределения потоков гаммаизлучения
#define IMDB_NUCLIDE_IDENTIFICATION	0x2A	//запрос на идентификацию нуклида гаммазилучения
#define IMDB_SPECTR									0x30	//запрос спектра гамма излучения

//команды ИМД-Б
#define IMDB_SEND_READY										0x11	//комплекс готов к работе
#define IMDB_SEND_NUCLIDE_DETECTION				0x12	//обнаружение источника
#define IMDB_SEND_DOSE_RATE_EXCEEDED			0x13	//превышение порога МД
#define IMDB_SEND_DOSE_EXCEEDED						0x14	//превышение порога Д
#define IMDB_SEND_NUCLIDE_IDENTIFICATION	0x15	//идентификация источника гамма излучения
#define IMDB_SEND_CALIBRATION_STATE				0x16	//имдб в режиме калибровки
#define IMDB_SEND_BACKGROUND_SUCCESS			0x17	//сообщение о завершении накопления фона
#define IMDB_SEND_SPECTR_ACCUM_SUCCESS		0x18	//сообщение о завершении измерения в точке

//Команды ИНК-Б
#define INK_GET_STATE								0x01	//запрос состояния
#define INK_SET_STATE								0x02	//задание режима работы
#define INK_DATA_REQUEST						0x03	//запрос измеренных данных
#define INK_SET_SHIELDING_FACTOR		0x04	//установка коэф. экранирования
#define INK_GET_SHIELDING_FACTOR		0x05	//запрос коэф. экранирования
#define INK_SET_ACC_TIME						0x06	//установка времени измерения
#define INK_SET_LIMIT_DETECT				0x07	//установки порога обнаружения]
#define INK_START_MEASURE						0x08	//запуск измерения в спектрометрическом режиме
//DEBUG
#define INK_GET_FIRMWARE_INFO				0x30	//запрос версии ПО
#define INK_RESPONSE								0x20	//подтверждение от ИНК

//Метрология
#define METROLOGY_DATA_REQUEST										0x40	//запрос данных от БД
#define METROLOGY_DATA_RESPONSE										0x50
#define METROLOGY_READPARAM_REQUEST								0x41	//запрос параметров БД
#define METROLOGY_READPARAM_RESPONSE							0x51	//ответ параметры БД
#define METROLOGY_WRITEPARAM_REQUEST							0x42	//запрос параметров на запись в БД
#define METROLOGY_WRITEPARAM_RESPONSE							0x52
#define METROLOGY_STARTBD_REQUEST									0x43	//запрос на запуск БД
#define METROLOGY_STARTBD_RESPONSE								0x53	
#define METROLOGY_DEFAULT_SETTINGS_REQUEST				0x45	//запрос на сброс настроек по умолчанию
#define METROLOGY_DEFAULT_SETTINGS_RESPONSE				0x55
#define METROLOGY_READ_SETTINGS_REQUEST						0x46	//запрос параметров БС
#define METROLOGY_READ_SETTINGS_RESPONSE					0x56	//отправка параметров БС
#define METROLOGY_WRITE_SETTINGS_REQUEST					0x47	//задание параметров БС
#define METROLOGY_WRITE_SETTINGS_RESPONSE					0x57	//отправка подтверждения задания параметров БС
#define METROLOGY_SPECTR_ACCUMMODE_REQUEST				0x48	//запрос на накопление спектров
#define METROLOGY_SPECTR_ACCUMMODE_RESPONSE				0x58	//ответ на команду накопления спектров
#define METROLOGY_SPECTR_ACCUMBLOCKDATA_REQUEST		0x49
#define METROLOGY_SPECTR_ACCUMBLOCKDATA_RESPONSE	0x59

#define METROLOGY_SEND_ACCUMSPECTR_SUCCESS	0x70	//сообщение об окончании накопления спектра

typedef  struct
{
	uint8 		frmVer;			//X.X - 4bits
	uint8 		prVer;			//X.X	-	4bits
	uint32		serialNum;	//HEX
}sServiceMsg;

typedef struct
{
	float 	fMaxAngle;
	float		fDoseRate;
	uint16	ePhotoPeak;
	uint16	iSpdCounter;
}sSpecModeData;

typedef struct
{
	//флаги входящих запросов
	uint8	rState;
	uint8	rOutsideDoseRate;
	uint8	rOutsideFeature;
	uint8	rInsideDoseRate;
	uint8	rInsideFeature;
	uint8	rInsideDose;
	uint8	rAngularDistribution;
	uint8	rMaxAngle;
	
	//флаги исходящих событий
	uint8	rNuclideDetectionCallback;
	uint8	rNuclideEndDetectionCallback;
	//флаги запросов от метрологического БД
	uint8	rMetrologySendCurrentSpectrProcess;	//флаг ожидания передачи 128*4 спектра
	uint8	rMetrologySendCurrentSpectr;	//флаг отправки 4х каналов 128-канального спектра
	uint8	rMetrologySendAccumSpectrBlock;	//флаг отправки блока запрошенного спектра
}sProtocolRequests;

#pragma pack(1)
	typedef struct
	{
			uint8	rangeIndex;
			float	doseRate;
			float	currentAngle;
	}tMetrologyData;
#pragma pack()

typedef enum
{
	E_DETECTION_START	=	0x01,
	E_DETECTION_END		= 0x00
}eDetectionState;	//состояние обнаружения источника

void 	 IMDBProtocol_Init(void);
uint8 *IMDBProtocol_CreatePacket(uint8	cmd,uint8 *pData,uint8 dataSize,uint8 *packSize);
uint8	*IMDBProtocol_GetPacketFromStream(uint8 *pDataBuf,int sDataBuf, int	*pDataBufIndex, uint8	InputByte, int *packSize);

void IMDB_ReceiveByteCallback(uint8	inputByte);
void IMDB_PackageAnalysis(uint8	cmd,uint8 bytesCount,uint8	*pData);
void IMDB_SendPacket(uint8	cmd,uint8	*pData, int dataSize);

void IMDB_SendState(void);
void IMDB_SendReady(void);
void IMDB_SendInsideDose(void);
void IMDB_SendOutsideDoseRate(void);
void IMDB_SendInsideDoseRate(void);
void IMDB_SendInsideDoseFeature(void);
void IMDB_SendOutsideDoseFeature(void);
void IMDB_SendMaxAngle(void);
void IMDB_SendIdentification(void);
void IMDB_SendDoseRateExceeded(uint8	Number);
void IMDB_SendDoseExceeded(uint8	Number);
void IMDB_SendCanTransmitError(uint8	erNum);
void IMDB_SendNuclideDetection(eDetectionState	State);
void	IMDB_SendAngularDistribution(void);
void IMDB_SendError(void);
void	IMDB_Process(void);
void	IMDB_Tick(void);
void	IMDB_NuclideDetectionSignalCallback();
void	IMDB_NuclideEndDetectionSignalCallback();

void IMDB_Metrology_SendBDParametrs(void);						//чтение параметров БД 	51h
void IMDB_Metrology_WriteBDParametrs(uint8	*pData);	//запись параметров БД 	52h
void	IMDB_Metrology_DefaultSettings(void);						//сброс БС по умолчанию 55h
void	IMDB_Metrology_ReadBSParametrs(void);						//чтение параметров БС 	56h
void	IMDB_Metrology_WriteBSParametrs(uint8	*pData);	//запись параметров БС 	57h
void 	IMDB_Metrology_StartBD(uint8	*pData);					//запуск выбранного БД 	53h
void 	IMDB_Metrology_GetBDData(void);									//запрос данных БД			50h

void	IMDB_Metrology_SpectrometryMode(uint8	*pData);	//задание режима наколпения спектров 58h
void	IMDB_Metrology_SendCurrentSpectrData(void);			//отправка 128*4 текущих спектров F0h - F7h
void	IMDB_Metrology_SendAccumSpectrBlockData(uint8	*pData);	//отправка запрошенного блока накопленных спектров
//uint8 * protocol_CreatePackageForOutputStream(uint8 Cmd,uint8 Status, uint16 sData, uint8 far *pData, int *pPackageSize);


#endif
