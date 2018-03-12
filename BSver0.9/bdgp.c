#include "bdgp.h"
#include "can.h"
#include "process.h"
#include "protocol.h"
#include "float.h"

#include "settings.h"

static float fLikelihoods[360];
static float fAngleDiagramm[48];
static uint8	msgCounterBDGP = 0;



const uint8		startBDGPMsg[] = {0x40};
const uint8		stopBDGPMsg[] = {0x41};
const uint8		resetBDGPMsg[] = {0x42};
//для гранаты
/*
const float directionCoef[] = {
1.0f,
0.999996f,
0.999982f,
0.999961f,
0.99993f,
0.99989f,
0.999842f,
0.999785f,
0.99972f,
0.999646f,
0.999563f,
0.999471f,
0.999371f,
0.999262f,
0.999145f,
0.99902f,
0.998886f,
0.998743f,
0.998592f,
0.998433f,
0.998266f,
0.998091f,
0.997907f,
0.997716f,
0.997516f,
0.997309f,
0.997094f,
0.996871f,
0.99664f,
0.996402f,
0.996156f,
0.995903f,
0.995643f,
0.995375f,
0.9951f,
0.994818f,
0.99453f,
0.994234f,
0.993932f,
0.993623f,
0.993307f,
0.992985f,
0.992657f,
0.992322f,
0.991982f,
0.991635f,
0.991283f,
0.990924f,
0.990561f,
0.990191f,
0.989817f,
0.989437f,
0.989052f,
0.988662f,
0.988267f,
0.987868f,
0.987464f,
0.987055f,
0.986642f,
0.986225f,
0.985804f,
0.985379f,
0.98495f,
0.984518f,
0.984082f,
0.983643f,
0.983201f,
0.982755f,
0.982307f,
0.981856f,
0.981402f,
0.980946f,
0.980487f,
0.980027f,
0.979564f,
0.9791f,
0.978633f,
0.978165f,
0.977696f,
0.977225f,
0.976754f,
0.976281f,
0.975807f,
0.975333f,
0.974858f,
0.974383f,
0.973907f,
0.973432f,
0.972956f,
0.972481f,
0.972005f,
0.971531f,
0.971057f,
0.970583f,
0.970111f,
0.96964f,
0.969169f,
0.968701f,
0.968233f,
0.967767f,
0.967303f,
0.966841f,
0.966381f,
0.965923f,
0.965467f,
0.965013f,
0.964563f,
0.964114f,
0.963669f,
0.963227f,
0.962787f,
0.962351f,
0.961918f,
0.961488f,
0.961062f,
0.96064f,
0.960222f,
0.959807f,
0.959396f,
0.95899f,
0.958588f,
0.952701f,
0.935417f,
0.912921f,
0.886712f,
0.8575f,
0.825714f,
0.791644f,
0.755501f,
0.71745f,
0.67762f,
0.63612f,
0.593042f,
0.548465f,
0.50246f,
0.455088f,
0.406407f,
0.356468f,
0.30532f,
0.253008f,
0.209271f,
0.183096f,
0.163388f,
0.147567f,
0.134443f,
0.123329f,
0.113776f,
0.105474f,
0.098197f,
0.091773f,
0.086069f,
0.080979f,
0.07642f,
0.07232f,
0.068624f,
0.065283f,
0.062256f,
0.059509f,
0.057013f,
0.054743f,
0.052676f,
0.050794f,
0.049079f,
0.047519f,
0.046099f,
0.04481f,
0.04364f,
0.042583f,
0.041629f,
0.040773f,
0.040008f,
0.03933f,
0.038735f,
0.038217f,
0.037775f,
0.037406f,
0.037106f,
0.036875f,
0.036711f,
0.036613f,
0.036581f
};*/
//для спектрометра
const float directionCoef[] = {
1.0000,
1.0000,
1.0000,
0.99949,
0.9961,
0.9948,
0.99357,
0.99241,
0.9913,
0.99025,
0.98925,
0.98829,
0.98736,
0.98647,
0.9856,
0.98474,
0.9839,
0.98307,
0.98224,
0.98141,
0.98057,
0.97971,
0.97884,
0.97794,
0.97701,
0.97604,
0.97504,
0.97399,
0.97289,
0.97174,
0.97053,
0.96925,
0.96791,
0.96649,
0.965,
0.96343,
0.96177,
0.96002,
0.95817,
0.95623,
0.95419,
0.95204,
0.94978,
0.9474,
0.94491,
0.9423,
0.93956,
0.93669,
0.93369,
0.93056,
0.92728,
0.92387,
0.92031,
0.9166,
0.91274,
0.90872,
0.90455,
0.90021,
0.89571,
0.89105,
0.88621,
0.88121,
0.87603,
0.87067,
0.86514,
0.85942,
0.85352,
0.84744,
0.84116,
0.8347,
0.82804,
0.82119,
0.81414,
0.8069,
0.79945,
0.79181,
0.78396,
0.7759,
0.76764,
0.75917,
0.7505,
0.74161,
0.73251,
0.7232,
0.71368,
0.70394,
0.69398,
0.68381,
0.67342,
0.66281,
0.65198,
0.64094,
0.62967,
0.61818,
0.60647,
0.59454,
0.58239,
0.57002,
0.55742,
0.54461,
0.53522,
0.52851,
0.51786,
0.50698,
0.49591,
0.48469,
0.47337,
0.46199,
0.45058,
0.43919,
0.42785,
0.41658,
0.40543,
0.39442,
0.38358,
0.37293,
0.36251,
0.35233,
0.3424,
0.33276,
0.32342,
0.3144,
0.3057,
0.29734,
0.28933,
0.28168,
0.27439,
0.26748,
0.26093,
0.25476,
0.24897,
0.24354,
0.23848,
0.23378,
0.22943,
0.22543,
0.22176,
0.21841,
0.21537,
0.21261,
0.21013,
0.20789,
0.20589,
0.20409,
0.20247,
0.201,
0.19965,
0.1984,
0.19721,
0.19605,
0.193825,
0.19206,
0.19145,
0.19095,
0.19054,
0.19022,
0.18997,
0.1898,
0.18968,
0.18962,
0.1896,
0.18963,
0.18969,
0.18977,
0.18988,
0.19,
0.19013,
0.19027,
0.19041,
0.19055,
0.19068,
0.19081,
0.19092,
0.19102,
0.19111,
0.19118,
0.19122,
0.19125,
0.19126,
0.19125,
0.19122
};

static tBDGPData			BDGPData;
static tBDGPParametrs	BDGPParametrs[2];

static float			doseArray[SLIDER_SIZE];

static uint8			sliderCounter = 0;
static uint8			dynamicMode = 0;
static uint8			sliderMax = 0;
static uint8			currentRangeIndex = 0;	//номер текущего диапазона

uint16	BDGP_MinCounter()
{
	uint8		i	=	0;
	uint16	result = 0xFFFF;
	
		for(i = 0;i<SENSORS;i++){
			if(BDGPData.WorkData[i] < result)
				result = BDGPData.WorkData[i];
		}
	return result;
}
uint16	BDGP_MaxCounter()
{
	uint8		i	=	0;
	uint16	result = 0;
	
		for(i = 0;i<SENSORS;i++){
			if(BDGPData.WorkData[i] > result)
				result = BDGPData.WorkData[i];
		}
	return result;
}

float	BDGP_MinSumSensData()
{
	uint8 i = 0;
	float result = FLT_MAX;
		for(i = 0;i<SENSORS;i++){
			if(BDGPData.fCorrectionSumSensData[i] < result)
				result = BDGPData.fCorrectionSumSensData[i];
		}
	return result;
}
float	BDGP_MaxSumSensData()
{
	uint8 i = 0;
	float result = FLT_MIN;
		for(i = 0;i<SENSORS;i++){
			if(BDGPData.fCorrectionSumSensData[i] > result)
				result = BDGPData.fCorrectionSumSensData[i];
		}
	return result;
}

void 	BDGP_Reset()
{
	msgCounterBDGP = 0;
	sliderCounter = 0;
	dynamicMode = 0;
	sliderMax = 0;
	
	memset(doseArray,0,sizeof(float) * SLIDER_SIZE);
	memset((uint8*)&BDGPData,0,sizeof(tBDGPData));
	memset((uint8*)&BDGPParametrs,0,sizeof(tBDGPParametrs) * 2);
	
	CAN_SendMessage(CAN_BDGP_CMD_TX,resetBDGPMsg,1);
	
	

}


/*
Фия отправки CAN сообщения на запрос параметра из БД
	range - 1,2 / диапазоны блока БДГП
	paramIndex - ИД параметра
	paramSubIndex - Субиндекс параметра
*/
void	BDGP_ParametrRequest(uint8	range,uint8	paramIndex,uint8	paramSubIndex)
{
	uint16	canID = 0x222 + range;
	
	uint8	Data[8];
		
		memset((uint8*)Data,0,sizeof(uint8) * 8);
		
		Data[0] = 0x05;
		Data[1] = paramIndex;
		Data[2] = 0x00;
		Data[3] = paramSubIndex;
	
	CAN_SendMessage(canID,Data,8);
}
/*
Ф-ия отправки CAN сообщения на запись параметра в БД
	range - 1,2 диапазоны блок БДГП
	paramIndex - индекс параметра
	paramSubIndex - субиндекс параметра
	*pParam - указатель на данные с параметром
	dataSize - рамер параметра
*/
void BDGP_WriteParametr(uint8	range,uint8	paramIndex,uint8 paramSubIndex, uint8	*pParam,int dataSize)
{
	uint16	canID = 0x222 + range;
	uint8	Data[8];
	
		memset((uint8*)Data,0,sizeof(uint8) * 8);
	
		Data[0] = 0x04;
		Data[1] = paramIndex;
		Data[2] = 0x00;
		Data[3] = paramSubIndex;
	
		memcpy(Data + 4,(uint32*)(pParam),dataSize);
	
	CAN_SendMessageIT(canID,Data,8);
		
}
//запуск записи параметров в БД
void	BDGP_StartWritingParametrs(uint8	rangeIndex)
{
	BDGP_WriteParametr(1 + rangeIndex,0x93,0,(uint8*)&BDGPParametrs[0].fCorrectionFactors[0],sizeof(float));
}

void 	BDGP_Start()
{
	CAN_SendMessage(CAN_BDGP_CMD_TX,startBDGPMsg,1);
}
void	BDGP_ForcedStart(uint8	rangeNumber)
{
	uint8	pData[2];
	

		pData[0] = 0x40;
		pData[1] = rangeNumber;
	
	CAN_SendMessage(CAN_BDGP_CMD_TX,pData,2);
}

void	BDGP_Stop()
{
	CAN_SendMessage(CAN_BDGP_CMD_TX,stopBDGPMsg,1);
}

void	BDGP_InsertParametrs(uint16	regID,uint8	*pData)
{
	uint32	rangesPack = 0;
	uint8		rangeIndex = 0;

		if(regID == 0x423) rangeIndex = 0;
		else if(regID == 0x424) rangeIndex = 1;
		else return;
	
	//запрос параметра
	if(pData[0] == 0x05)
	{
		switch(pData[1])
		{
			case 0x93:
			{
				memcpy((uint8*)&BDGPParametrs[rangeIndex].fCorrectionFactors[pData[3]],(pData + 4),4);
				
					if(pData[3]<12){
						BDGP_ParametrRequest(rangeIndex + 1,0x93,pData[3]+1);
					}
					else
						BDGP_ParametrRequest(rangeIndex + 1,0x94,0);
					
				
			}break;
			
			case 0x94:
			{
					memcpy((uint8*)&BDGPParametrs[rangeIndex].fDeadTime,(pData + 4),4);
						BDGP_ParametrRequest(rangeIndex + 1,0x95,0);
			}break;
			
			case 0x95:
			{
				memcpy((uint8*)&BDGPParametrs[rangeIndex].fEffectCounters,(pData + 4),4);
						BDGP_ParametrRequest(rangeIndex + 1,0x96,0);
			}break;
			
			case 0x96:
			{
				memcpy((uint8*)&BDGPParametrs[rangeIndex].fSensCounters,(pData + 4),4);
						BDGP_ParametrRequest(rangeIndex + 1,0x97,0);
			}break;
			
			case 0x97:
			{
				memcpy((uint8*)&BDGPParametrs[rangeIndex].rangeMin,pData + 4,2);
				memcpy((uint8*)&BDGPParametrs[rangeIndex].rangeMax,pData + 6,2);
					//аналогичный запрос со второго этажа БД
					if(rangeIndex == 0)
						BDGP_ParametrRequest( 2,0x93,0);
			}break;
		}
	}
	//задание параметра
	if(pData[0] == 0x04)
	{
		switch(pData[1])
		{
			case 0x93:
			{
					if(pData[3]<12){
						BDGP_WriteParametr(rangeIndex + 1,0x93,pData[3] + 1,(uint8*)&BDGPParametrs[rangeIndex].fCorrectionFactors[pData[3] + 1],sizeof(float));
						
					}
					else
						BDGP_WriteParametr(rangeIndex + 1,0x94,0,(uint8*)&BDGPParametrs[rangeIndex].fDeadTime,sizeof(float));
			}break;
			
			case 0x94:
			{
				BDGP_WriteParametr(rangeIndex + 1,0x95,0,(uint8*)&BDGPParametrs[rangeIndex].fEffectCounters,sizeof(float));
			}break;
			
			case 0x95:
			{
				BDGP_WriteParametr(rangeIndex + 1,0x96,0,(uint8*)&BDGPParametrs[rangeIndex].fSensCounters,sizeof(float));
			}break;
			
			case 0x96:
			{
				rangesPack = BDGPParametrs[rangeIndex].rangeMax; 
				rangesPack = rangesPack << 16;
				rangesPack = rangesPack|BDGPParametrs[rangeIndex].rangeMin;
					

					BDGP_WriteParametr(rangeIndex + 1,0x97,0,(uint8*)&rangesPack,sizeof(float));
			}break;
			
			case 0x97:
			{
				if(rangeIndex == 0)
					BDGP_StartWritingParametrs(1);
				else//тут сохраняем
				{
					BDGP_WriteParametr(1,0xE0,0xFF,0,0);
					BDGP_WriteParametr(2,0xE0,0xFF,0,0);
					
				}
					
				
			}break;
		}
	}

}
/*
Ф-ия накопления фона работает тольео в режиме штатной работы и только при отсутствии спектрометра
*/
void	BDGP_BackgroundAccumulation(uint16	*pData)
{
	uint8		i = 0;
	
	float fCurrentData = 0;	
	
	if(BDGPData.BackgroundIndex < BACKGROUND_TIME){
		for(i = 0;i<SENSORS;i++){
			fCurrentData = ((pData[i] * BDGPParametrs[currentRangeIndex].fCorrectionFactors[i]) / (1.0f - pData[i] * BDGPParametrs[currentRangeIndex].fDeadTime));
			
			BDGPData.BackgroundAverageSpeed += fCurrentData;
		}
		BDGPData.BackgroundAverageDose += BDGPData.currentDose;
		BDGPData.BackgroundIndex++;
	}
	if((BDGPData.Background == 0) && (BDGPData.BackgroundIndex == BACKGROUND_TIME) ){
		BDGPData.Background = 1;	
		BDGPData.BackgroundAverageSpeed = BDGPData.BackgroundAverageSpeed / BACKGROUND_TIME;
		BDGPData.BackgroundAverageDose = BDGPData.BackgroundAverageDose / BACKGROUND_TIME;
		
				IMDB_SendPacket(IMDB_SEND_BACKGROUND_SUCCESS,(uint8*)&BDGPData.BackgroundAverageDose,sizeof(float));//отсылаем успех в накоплении фона
				Process_SetStatus(IMDB_FULLTIME_WORK);//переключаемся в режим штатной работы
	}
		
}
void	BDGP_ResetBackgroundData()
{
	BDGPData.BackgroundAverageSpeed = 0;
	BDGPData.BackgroundAverageDose = 0;
	
	BDGPData.BackgroundIndex = 0;
	BDGPData.Background = 0;
}

void	BDGP_InsertDataNew(uint16 regID,uint8	*pData)
{
	uint8	activeBuffer = BDGPData.ActiveBuffer;
	
	if(pData[0] == 0x40 && pData[1] == 0xFF)//поймали первое сообщение
	{
		//определяем номер диапазона
		if(regID == 0x723)
			currentRangeIndex = 0;
		if(regID == 0x724)
			currentRangeIndex = 1;
		//копируем данные МД и средней скорости счёта
		memcpy((uint8*)&BDGPData.counterRate,(uint8*)pData+2,sizeof(uint16));
		memcpy((uint8*)&BDGPData.currentDose,(uint8*)pData+4,sizeof(float));
		
		msgCounterBDGP = 0;
	}
	else
	{
		BDGPData.InputData[activeBuffer][msgCounterBDGP * 4] 			 = (uint16)((pData[1] << 8) | pData[0]);
		BDGPData.InputData[activeBuffer][(msgCounterBDGP * 4) + 1] = (uint16)((pData[3] << 8) | pData[2]);
		BDGPData.InputData[activeBuffer][(msgCounterBDGP * 4) + 2] = (uint16)((pData[5] << 8) | pData[4]);
		BDGPData.InputData[activeBuffer][(msgCounterBDGP * 4) + 3] = (uint16)((pData[7] << 8) | pData[6]);
		
		msgCounterBDGP++;
		/*
		//копируем скорости счёта текущего пакета с данными
		memcpy((uint16*)BDGPData.sensorsCntCopy + (msgCounterBDGP * 4),(uint8*)pData,sizeof(uint16) * 4);
		
		msgCounterBDGP++;
		//есил получили все пакеты со скоростями, то копируем их в рабочий массив
		if(msgCounterBDGP == 3){
			memcpy((uint8*)BDGPData.sensorsCnt,(uint8*)BDGPData.sensorsCntCopy,sizeof(uint16) * SENSORS);
			
			BDGPData.CanPackageEvent = 1;
		}*/
			if(msgCounterBDGP == 3){
				if(BDGPData.ActiveBuffer == 1)
					BDGPData.ActiveBuffer = 0;
				else
					BDGPData.ActiveBuffer = 1;
				
				BDGPData.ChangeBufferEvent = 1;
			}
	}
}

void 	BDGP_InsertCmd(uint8	*pData)
{
	if(pData[0] == 0x42)
	{
		Process_BDGPStatus(DEVICE_READY);
		
		//запрашиваем параметры блока 1 диапазон, коэфициенты, субиндекс - первый коэфицент
		BDGP_ParametrRequest(1,0x93,0);
	}
}
float BDGP_GetAverageDoseRate()
{
	return BDGPData.averageDose;
}
float		BDGP_GetCurrentDoseRate()
{
	return BDGPData.currentDose;
}

uint8	*BDGP_GetParametrs()
{
	return (uint8*)&BDGPParametrs;
}

float	*BDGP_DirectionDiagramm(float	*pData, uint8	SensCount, float cntMin,float cntMax, uint16	*pAngleMax, eAngDiagrammMode Mode)
{
	//float	correctionData[SENSORS];
	uint16	lIndex[SENSORS];
	int16	f = 0,g = 0,b = 0;
	uint8		i = 0,j = 0;
	
	//uint16	cntMin = BDGP_MinCounter();
	//uint16	cntMax = BDGP_MaxCounter();
	float fK = 0.8f - ((cntMin + 1) / (cntMax + 1));	//коэф. разрыхления жопы
	float fZ = 0.0f;
	
	float SignalLevel = (cntMax - cntMin) / (1 - directionCoef[180]);
	float BackLevel = (cntMin - directionCoef[180] * cntMax) / (1 - directionCoef[180]);
		
	//float *fLikelihoods = (float*)malloc(sizeof(float)*360);
	//float	*fAngleDiagramm = (float*)malloc(sizeof(float)*49);
	
	float fLHMin = FLT_MAX;	//минимум функции правдоподобия
	float fLHMax = FLT_MIN;	//максимум функции правдоподобия
	float fNLMax = -100000;//FLT_MIN;	//максимум сжатой функции
	float fNormizeKoef = 0.0f;	//коэффициент нормировки
	float	fCountersSum = 0.0f;	//сумма счетов счётчиков БД за время движка
	float	fLLSum = 0;
	
	float	fLog = 0;	//подлогарифм. функция
	float fFullLog = 0;
	
	memset(fAngleDiagramm,0,sizeof(float) * 48);
	
	//корректировка скоростей счёта
	if(BackLevel <= 0.0f)
		BackLevel = 0.1f;
	if(SignalLevel <= 0.0f)
		SignalLevel = 1.0f;
	
	fLog = (SignalLevel / BackLevel);
	
	//if(fLikelihoods)
	{

	
		for(f = 0;f<360;f++)
		{
			for(i = 0;i<SensCount;i++)
			{
				if( abs(-f + (30 * i)) < 181)
				{
					lIndex[i] = abs(-f + (30 * i));
				}
				else if((360 - f + 30 * i) < 181 )
				{
					lIndex[i] = (360 - f + 30 * i);
				}
				else
				{
					lIndex[i] = abs(360 + f - 30 * i);
				}
			}
			//вычисляем ф-ию правдоподобия
			fLikelihoods[f] = 0;
			
			for(i = 0;i<SensCount;i++)
			{
				if(Mode == CNTR_MODE)
					fLikelihoods[f] += (pData[i] * BDGPParametrs[currentRangeIndex].fCorrectionFactors[i] * log(1 + fLog * directionCoef[lIndex[i]]) - SignalLevel * directionCoef[lIndex[i]]);
				if(Mode == SPEC_MODE)
					fLikelihoods[f] += (pData[i] * log(1 + fLog * directionCoef[lIndex[i]]) - SignalLevel * directionCoef[lIndex[i]]);
			}
		}
		//находим минимум 
		for(f = 0;f<360;f++)
			if(fLikelihoods[f] < fLHMin)
				fLHMin = fLikelihoods[f];

		//находим максимум
		for(f = 0;f<360;f++)
			if(fLikelihoods[f]>fLHMax)
				fLHMax = fLikelihoods[f];
			
		//вычисляем Z
		fZ = (fLHMin - (1.0f - fK) * fLHMax) / (fK * fLHMin);
		//перекалибровка с учетом fZ
		for(f = 0;f<360;f++)
			fLikelihoods[f] = fLikelihoods[f] - fZ * fLHMin;

		//находим максимум и угол
		for(f = 0;f<360;f++)
		{
			if(fLikelihoods[f]>fLHMax){
				fLHMax = fLikelihoods[f];
				*pAngleMax = f;
			}
		}
		
//		if(fAngleDiagramm)
			{
				
				
				for(f = 347;f<=359;f++)
					fAngleDiagramm[0] += fLikelihoods[f];
				for(f = 0;f<=13;f++)
					fAngleDiagramm[0] += fLikelihoods[f];
				
				
				for(f = 354;f<=359;f++)
					fAngleDiagramm[1] += fLikelihoods[f];
				for(f = 0;f<=20;f++)
					fAngleDiagramm[1] += fLikelihoods[f];
				
				g = 2;
				b = 2;
				
				do
				{
					fLLSum = 0;
					
					for(f = b;f<b + 27;f++)
					{
						fAngleDiagramm[g] += fLikelihoods[f];
						//fLLSum += fLikelihoods[f];
					}
					//fAngleDiagramm[g] = (1 / fLHMax) * fLLSum;
					
					if(g%2 == 0)
						b = b + 7;	//7
					else
						b = b + 8;	//8
					
					g++;
				}
				while(g<47);
				
				
					fAngleDiagramm[47] = 0;
				for(f = 339;f<=359;f++)
					fAngleDiagramm[47] += fLikelihoods[f];
				for(f = 0;f<=5;f++)
					fAngleDiagramm[47] += fLikelihoods[f];			
				
				//максимум сжатой функции
				for(g = 0;g<48;g++)
					if(fAngleDiagramm[g] > fNLMax)
						fNLMax = fAngleDiagramm[g];
				
				if(Mode == CNTR_MODE){
					
				for(i = SLIDER_SIZE - sliderMax;i < SLIDER_SIZE;i++)
					for(j = 0;j<SensCount;j++)
						fCountersSum += BDGPData.sensorsCntQuery[i][j];
					
				fCountersSum = fCountersSum / SensCount;
					
				if(fCountersSum < (sliderMax * BDGPData.BackgroundAverageSpeed))
						fNormizeKoef = 0.5f;
				else
						fNormizeKoef = 1.0f - 0.5f * ( (sliderMax * BDGPData.BackgroundAverageSpeed) / fCountersSum );
				}
				if(Mode == SPEC_MODE)
					fNormizeKoef = 0.5f;
				
				for(g = 0;g<48;g++)
					fAngleDiagramm[g] = fAngleDiagramm[g] * fNormizeKoef / fNLMax;
					/*if(fNLMax == FLT_MIN)
						for(g = 0;g<48;g++)
							fAngleDiagramm[g] = 1;
					else			
						for(g = 0;g<48;g++)
							fAngleDiagramm[g] = fAngleDiagramm[g] / fNLMax;*/
				}	
		
		
		//free(fLikelihoods);
	}
	return fAngleDiagramm;
	
}
float	*BDGP_GetAngularDiagram()
{
	uint8	i = 0;
	
	float cntMin = BDGP_MinSumSensData();
	float cntMax = BDGP_MaxSumSensData();
	
	float	*pAngleDiagramm;	

		pAngleDiagramm	= (float*)BDGP_DirectionDiagramm(BDGPData.fCorrectionSumSensData, SENSORS, cntMin,cntMax,&BDGPData.radiationMaximum,CNTR_MODE);
		

	//		memcpy(BDGPData.nLikelihoods,pAngleDiagramm,sizeof(float) * 48);
	return 	pAngleDiagramm;


	//return BDGPData.nLikelihoods;
}

void	BDGP_DataArrayCorrection(uint8	oldSize)
{
	/*
	1. Копирование хвоста массива(размером oldSize) в начало массива
	2. Обнуление остальной части массива со смещением oldSize
	3. Установка параметра sliderCounter в соответствии со значением oldSize
	4. Сброс флага QuerySuccess
	*/
	memcpy(&doseArray[0],&doseArray[SLIDER_SIZE - oldSize - 1],sizeof(float) * oldSize);
	memset(&doseArray[oldSize],0,sizeof(float) * (SLIDER_SIZE - oldSize));
		
	memcpy(BDGPData.sensorsCntQuery[0],BDGPData.sensorsCntQuery[SLIDER_SIZE - oldSize - 1],sizeof(float) * SENSORS * oldSize);
	memset(BDGPData.sensorsCntQuery[oldSize],0,sizeof(float) * SENSORS * (SLIDER_SIZE - oldSize));
	
		sliderCounter = oldSize;
		
		BDGPData.QuerySuccess = 0;
}
void	BDGP_BackgroundIdentification()
{
	uint8	i = 0, j = 0;
	float	fSum = 0;
	

	
		BDGPData.fQuantile = 0;
	
		for(i = 0;i<SENSORS;i++){
			fSum += (BDGPData.fCorrectionSumSensData[i] * sliderMax);
		}
					//берём 'хвост' массива, т.к. там самые свежие значения МД и счетов
					/*for(i = SLIDER_SIZE - sliderMax;i < SLIDER_SIZE;i++){
						for(j = 0;j<SENSORS;j++)
							fSum += BDGPData.sensorsCntQuery[i][j];
					}*/

		
		BDGPData.fQuantile = (fSum - (sliderMax * BDGPData.BackgroundAverageSpeed)) / (sqrt(sliderMax * BDGPData.BackgroundAverageSpeed) + 1);
	
		if(BDGPData.fQuantile > MainSettings.limitDetect[0])
		{
			//ставим флаг идентификации нуклида
			if(!BDGPData.NuclideDetection)
				BDGPData.NuclideDetection = 1;
			
			IMDB_NuclideDetectionSignalCallback();
		}
		else
		{
			//если была идентификация
			if(BDGPData.NuclideDetection){
				BDGPData.NuclideDetection = 0;
				IMDB_NuclideEndDetectionSignalCallback();	//шлём сигнал для отправки сообщения о сброе обнаружения источника
			}
		}
}

void	BDGP_Process()
{
	uint8			i = 0,j=0;
	
	
	
	if(BDGPData.ChangeBufferEvent == 1)
	{
		BDGPData.ChangeBufferEvent = 0;
		
		if(BDGPData.ActiveBuffer == 1)
				memcpy(BDGPData.WorkData,BDGPData.InputData[0],sizeof(uint16) * SENSORS);
		else
				memcpy(BDGPData.WorkData,BDGPData.InputData[1],sizeof(uint16) * SENSORS);	
		
				//////////////////////////////
				//накопление фона для БДГП-yБ//
				//////////////////////////////
				/*
					TO-DO сделать режим накопления фона!
				*/
					//выполняем проверку на текущий рабочий статус системы
				if(Process_GetOperationMode() == IMDB_BACKGROUND_ACCUMULATION){
				//и проверку на наличие спектрометра
					if(Process_GetBDPSStatus() == DEVICE_NOTREADY){
						BDGP_BackgroundAccumulation((uint16*)BDGPData.WorkData);//корректировка счетов происходит при подсчёте фоновой скорости счёта
					}
				}		
		
		
		BDGPData.averageDose = 0;
		//динамический движок получения средней МД
			if(sliderCounter<SLIDER_SIZE){
				doseArray[sliderCounter] = BDGPData.currentDose;
				
				//заполняем очередь счетов с поправкой на попр. коэф и мёртвое время
				
				for(i = 0;i<SENSORS;i++)
					BDGPData.sensorsCntQuery[sliderCounter][i] = ((BDGPData.WorkData[i] * BDGPParametrs[currentRangeIndex].fCorrectionFactors[i]) / (1.0f - BDGPData.WorkData[i] * BDGPParametrs[currentRangeIndex].fDeadTime));
				
				sliderCounter++;
			}
			else
			{
				if(!BDGPData.QuerySuccess)	//накопили очереди МД и счетов
					BDGPData.QuerySuccess = 1;
				
				memcpy(&doseArray[0],&doseArray[1],sizeof(float) * (SLIDER_SIZE - 1));
				memcpy(BDGPData.sensorsCntQuery[0],BDGPData.sensorsCntQuery[1],sizeof(float) * SENSORS * (SLIDER_SIZE - 1));
				
				
				doseArray[SLIDER_SIZE - 1] = BDGPData.currentDose;
				
				for(i = 0;i<SENSORS;i++)
					BDGPData.sensorsCntQuery[SLIDER_SIZE - 1][i] = ((BDGPData.WorkData[i] * BDGPParametrs[currentRangeIndex].fCorrectionFactors[i]) / (1.0f - BDGPData.WorkData[i] * BDGPParametrs[currentRangeIndex].fDeadTime));
			}
				
				memset(BDGPData.fCorrectionSumSensData,0,sizeof(float) * SENSORS);	//очищаем усредненный по движку массив
			
				if(BDGPData.QuerySuccess)
				{
					//если накопили движок МД, то в зависимости от скорости счёта подсчитываем текущую МД
					
					/*
						TO-DO! Программная корректировка массивов движков в соответствии с изменением времения накопления движка в большую сторону
					*/
					if(BDGPData.counterRate < 3){
						/*if(sliderMax == 10)
							BDGP_DataArrayCorrection(sliderMax);*/
						
						sliderMax = 30;
					}
					else if(BDGPData.counterRate > 1 && BDGPData.counterRate < 12){
						/*if(sliderMax == 2)
							BDGP_DataArrayCorrection(sliderMax);*/
						
						sliderMax = 10;
					}
					else if(BDGPData.counterRate > 8)
						sliderMax = 2;
					else;
					//берём 'хвост' массива, т.к. там самые свежие значения МД и счетов
					for(i = SLIDER_SIZE - sliderMax;i < SLIDER_SIZE;i++){
						BDGPData.averageDose += doseArray[i];
						for(j = 0;j<SENSORS;j++)
							BDGPData.fCorrectionSumSensData[j] += BDGPData.sensorsCntQuery[i][j];
					}
			
					
					BDGPData.averageDose = BDGPData.averageDose / sliderMax;
					
						for(j = 0;j<SENSORS;j++)
							BDGPData.fCorrectionSumSensData[j] = BDGPData.fCorrectionSumSensData[j] / sliderMax;					
				}
				else
				{
					//если нет, то выводим усреднение по счётчику накопления
					for(i = 0;i<sliderCounter;i++){
						BDGPData.averageDose += doseArray[i];
						for(j = 0;j<SENSORS;j++)
							BDGPData.fCorrectionSumSensData[j] += BDGPData.sensorsCntQuery[i][j];
						
					}
					
					if(sliderCounter != 0){
						BDGPData.averageDose = BDGPData.averageDose / sliderCounter;
						
						for(j = 0;j<SENSORS;j++)
							BDGPData.fCorrectionSumSensData[j] = BDGPData.fCorrectionSumSensData[j] / sliderCounter;
					}
				}
				BDGPData.Dose += BDGPData.currentDose;
				
				//фоновая идентификация
				if(BDGPData.Background){
					BDGP_BackgroundIdentification();
				}
							
	}
}

float	BDGP_GetMaximumAngle()
{
	float fAngle = (float)BDGPData.radiationMaximum;
	
	return fAngle;
}
float BDGP_GetDose()
{
	return BDGPData.Dose;
}
uint8		BDGP_GetBackgroundState()
{
	return BDGPData.Background;
}

uint8	BDGP_GetOutsideFeature()
{	
		if(BDGPData.currentDose >= 1.3f * BDGPData.averageDose)
			return 1;	//МД увеличилась
		else if(BDGPData.currentDose <= 0.7f * BDGPData.averageDose)
			return 2;	//МД уменьшилась
		else
			return 0;	//МД неизменилась
}

