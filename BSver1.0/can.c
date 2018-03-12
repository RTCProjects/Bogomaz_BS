#include "can.h"
#include "settings.h"
#include "protocol.h"
#include "devices.h"
#include "system.h"
#include "bsl.h"

#include "bdmg.h"
#include "bdgp.h"
#include "bdsp.h"

#include "main.h"

sbit CAN0IR = CAN_IC0^7;

static uint8	txEnable = 0;
static volatile uint16	defferedCounter;

static tCanQueryMessages	canQuery;
/*
MO1 - 521
MO2 - 721

MO3 - 280
MO4 - 722
MO5 - 723

MO6 - 0x000 - 0x004
MO7 - 0x201
MO8 - 0x202
MO9 - 0x203
MO10 - 0x204

*/

void CAN_Interrupt0() interrupt 0x54	
{
	uint16 curIndex = canQuery.Counts - 1;
	
	txEnable = 1;
	
	if(canQuery.Counts > 0){
		if(canQuery.Messages[0].DefferedTime > 0){
			System_Delay(canQuery.Messages[0].DefferedTime);
		}
		
		CAN_SendMessageIT(canQuery.Messages[0].CanID,canQuery.Messages[0].Data,canQuery.Messages[0].Length);

		memcpy((uint8*)&canQuery.Messages[0],(uint8*)&canQuery.Messages[1],sizeof(tCanSendMsg) * (CAN_TX_QUERY_SIZE - 1));
		
		canQuery.Counts--;
		
		if(canQuery.Counts == 0)
			canQuery.QuerySending = 0;
		
	}
}

void CAN_Interrupt1() interrupt 0x55		//индекс 521
{
	/*uint16 regID = ((CAN_Message_16[1].MOARH >> 2) & 0x07FF);
	uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[1].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[1].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[1].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[1].MODATAHH;

	Devices_ReceiveMsgCallback(regID,msgData);
	
	
	if(regID == 0x555)
	{
		if(msgData[0] == 0x09)
		{
			__asm
			{
				CALLS 04H 0E000H
			}
		}
	}*/
		uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[1].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[1].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[1].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[1].MODATAHH;
		
		BDMG_InsertCmd(msgData);
}


void CAN_Interrupt2() interrupt 0x56			//721
{
	uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[2].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[2].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[2].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[2].MODATAHH;
	
	BDMG_InsertData(msgData);
}

void CAN_Interrupt3() interrupt 0x57		//280
{
	uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[3].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[3].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[3].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[3].MODATAHH;
	
	BDGP_InsertCmd(msgData);
}

void CAN_Interrupt4() interrupt 0x59		//723
{
	uint8 msgData[8];
	uint16 regID = ((CAN_Message_16[4].MOARH >> 2) & 0x07FF);
	
		*((uint16*)&msgData[0]) = CAN_Message_16[4].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[4].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[4].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[4].MODATAHH;
	
	BDGP_InsertDataNew(regID,msgData);
}

void CAN_Interrupt5() interrupt 0x5A		//724
{
	uint8 msgData[8];
	uint16 regID = ((CAN_Message_16[5].MOARH >> 2) & 0x07FF);
	
		*((uint16*)&msgData[0]) = CAN_Message_16[5].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[5].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[5].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[5].MODATAHH;
	
	BDGP_InsertDataNew(regID,msgData);
}

void CAN_Interrupt6() interrupt 0x5B		//000 - 004	
{
	/*
	
	*/
	unsigned int far *pReprogWord = (unsigned int far *) 0x50000;
	uint32 serialNumber = 0;
	
	
	uint16 regID = ((CAN_Message_16[6].MOARH >> 2) & 0x07FF);
	uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[6].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[6].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[6].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[6].MODATAHH;
	
	//тестовый код для запуска загрузчика
	if(regID == 0x000){
		if(msgData[0] == 0xD2 && msgData[1] == 0x2D && msgData[2] == 0x00 && msgData[3] == 0x00 &&
			 msgData[4] == 0xFF && msgData[5] == 0xFF && msgData[6] == 0xFF && msgData[7] == 0xFF)
			{
				//на запрос серийного номера, отправим серийный номер текущего устройства
				memcpy(msgData + 4,(uint32*)&SerialNumberBD,4);
				//аппаратный номер блока
				msgData[3] = DEVICE_NUMBER;
				
				CAN_SendMessage(0x000,msgData,8);
				/*__asm
				{
					PUSH R2
					MOV R2 #1234H
					EXTS #05H #1H
					MOV 0000H R2
					POP R2
					
					SRST
				}*/
			}
		if(msgData[0] == 0xD3 && msgData[1] == 0x3D && msgData[2] == 0x00){
			memcpy((uint32*)&serialNumber,(uint8*)msgData + 4,4);
			
			//проверяем соответствие серийного номера и аппаратного
			if(serialNumber == SerialNumberBD && msgData[3] == DEVICE_NUMBER)
			{
				*pReprogWord = 0x1234;
				
				__asm {SRST};
			}
			/*
			msgData[3] - SubIndex
			msgData[4 - 7] - SerialNumber
			*/
		}
	}
	
	BDSP_InsertCmd(regID,msgData);
}

void CAN_Interrupt7() interrupt 0x5C		//201
{
	uint16 regID = ((CAN_Message_16[7].MOARH >> 2) & 0x07FF) - 1;
	
		uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[7].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[7].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[7].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[7].MODATAHH;
	
	//BDSP_InsertData(regID & 0xF,msgData);
	BDSP_InsertData(0,msgData);
}

void CAN_Interrupt8() interrupt 0x60		//202
{
		uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[8].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[8].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[8].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[8].MODATAHH;
	
	BDSP_InsertData(1,msgData);
}

void CAN_Interrupt9() interrupt 0x61		//203	
{
		uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[9].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[9].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[9].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[9].MODATAHH;
	
	BDSP_InsertData(2,msgData);
}

void CAN_Interrupt10() interrupt 0x62	 //204
{
	uint8 msgData[8];

		*((uint16*)&msgData[0]) = CAN_Message_16[10].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[10].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[10].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[10].MODATAHH;
	
	BDSP_InsertData(3,msgData);
}

void CAN_Interrupt11() interrupt 0x63 //0x422 - 0x424
{
	uint8 msgData[8];
	uint16 regID = ((CAN_Message_16[11].MOARH >> 2) & 0x07FF);
	
		*((uint16*)&msgData[0]) = CAN_Message_16[11].MODATALL;
		*((uint16*)&msgData[2]) = CAN_Message_16[11].MODATALH;
		*((uint16*)&msgData[4]) = CAN_Message_16[11].MODATAHL;
		*((uint16*)&msgData[6]) = CAN_Message_16[11].MODATAHH;
	
	BDGP_InsertParametrs(regID,msgData);
}



 
void CAN_Init()
{
	memset((uint8*)&canQuery,0,sizeof(tCanQueryMessages));
	
	txEnable = 1;
	defferedCounter = 0;
	
	DP9 = 0x0008;
	ALTSEL0P9 = 0x0008;
	ALTSEL1P9 = 0x0008;
	
	
	CAN_IC0 = 0x0074;	//72Xh	- TX
	
	CAN_IC1 = 0x005C;	//00Xh
	CAN_IC2 = 0x0060;	//20Xh
	
	CAN_IC3 = 0x0061;	//20Xh
	CAN_IC4 = 0x005D;	//20Xh
	CAN_IC5 = 0x0059;	//20Xh
	
	CAN_IC6 = 0x0062;	//20Xh
	CAN_IC7 = 0x0070;	//20Xh
	CAN_IC8 = 0x0071;	//20Xh
	CAN_IC9 = 0x0072;	//20Xh
	CAN_IC10 = 0x0073;	//20Xh
	CAN_IC11 = 0x006F;	

	NCR0  = 0x0041;		//разрешаем вносить изменения в конфигурацию
	NPCR0 = 0x0003;		//Выбрали P9.2 для приема в CAN узел 0
	NBTR0 = 0x2344;		//скорость узла 500kb/sec при тактировании 16Мгц
	NCR0  = 0x0000;		//Запрет после реконфигурации
	
	/*
		Список областей сообщений
	
		На приём
			3. 00Хh - аварийное сообщение для CAN
	    2. 20Хh - сообщение с данными от БДГП-С
		На передачу
		  1. 000 - CAN сообщение с настраиваемым ID
			2. 722 - данные (прерывание)
			3. 280 - передача старт/стоп
	*/
	//передача
	//000 - универсальный вар.
	CAN_Message_16[0].MOCTRH = 0x0E08;
	CAN_Message_16[0].MOCTRL = 0x0000;
	CAN_Message_16[0].MOIPRL = 0x0000;	/*выбор линии прерываний для передачи - 0*/
  CAN_Message_16[0].MOFCRH = 0x0802; // DLC = 8, запретить прерывание на передачу 
	 
	
	//000 - универсальный приём
	/*CAN_Message_16[1].MOCTRH = 0x0080;
  CAN_Message_16[1].MOCTRL = 0x0000;
  CAN_Message_16[1].MOIPRL = 0x0001;	// выбор линии прерываний по приему - 1
	CAN_Message_16[1].MOFCRH = 0x0801;	// разрешить прерывание на прием*/
	
	//CAN_Message_16[1].MOFCRL = 0x0001;	//выбор FIFO приёма
	//CAN_Message_16[1].MOFGPRH = 0x0001;
	//CAN_Message_16[1].MOFGPRL = 0x0802;
	
	

	CAN_Message_16[1].MOCTRH = 0x0080;
	CAN_Message_16[1].MOCTRL = 0x0000;
	CAN_Message_16[1].MOIPRL = 0x0001;	//выбор линии прерываний для приему - 1
  CAN_Message_16[1].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[2].MOCTRH = 0x0080;
	CAN_Message_16[2].MOCTRL = 0x0000;
	CAN_Message_16[2].MOIPRL = 0x0002;	//выбор линии прерываний для приему - 2
  CAN_Message_16[2].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[3].MOCTRH = 0x0080;
	CAN_Message_16[3].MOCTRL = 0x0000;
	CAN_Message_16[3].MOIPRL = 0x0003;	//выбор линии прерываний для приему - 3
  CAN_Message_16[3].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[4].MOCTRH = 0x0080;
	CAN_Message_16[4].MOCTRL = 0x0000;
	CAN_Message_16[4].MOIPRL = 0x0004;	//выбор линии прерываний для приему - 1
  CAN_Message_16[4].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[5].MOCTRH = 0x0080;
	CAN_Message_16[5].MOCTRL = 0x0000;
	CAN_Message_16[5].MOIPRL = 0x0005;	//выбор линии прерываний для приему - 1
  CAN_Message_16[5].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[6].MOCTRH = 0x0080;
	CAN_Message_16[6].MOCTRL = 0x0000;
	CAN_Message_16[6].MOIPRL = 0x0006;	//выбор линии прерываний для приему - 1
  CAN_Message_16[6].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[7].MOCTRH = 0x0080;
	CAN_Message_16[7].MOCTRL = 0x0000;
	CAN_Message_16[7].MOIPRL = 0x0007;	//выбор линии прерываний для приему - 1
  CAN_Message_16[7].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[8].MOCTRH = 0x0080;
	CAN_Message_16[8].MOCTRL = 0x0000;
	CAN_Message_16[8].MOIPRL = 0x0008;	//выбор линии прерываний для приему - 1
  CAN_Message_16[8].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[9].MOCTRH = 0x0080;
	CAN_Message_16[9].MOCTRL = 0x0000;
	CAN_Message_16[9].MOIPRL = 0x0009;	//выбор линии прерываний для приему - 1
  CAN_Message_16[9].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	CAN_Message_16[10].MOCTRH = 0x0080;
	CAN_Message_16[10].MOCTRL = 0x0000;
	CAN_Message_16[10].MOIPRL = 0x000A;	//выбор линии прерываний для приему - 1
  CAN_Message_16[10].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
	
	//параметры БДГП-Б
	CAN_Message_16[11].MOCTRH = 0x0080;
	CAN_Message_16[11].MOCTRL = 0x0000;
	CAN_Message_16[11].MOIPRL = 0x000B;	//выбор линии прерываний для приему - 1
  CAN_Message_16[11].MOFCRH = 0x0801; // DLC = 8, разрешить прерывание на прием 
/*
	//00X
	CAN_Message_16[3].MOCTRH = 0x0080;
  CAN_Message_16[3].MOCTRL = 0x0000;
  CAN_Message_16[3].MOIPRL = 0x0001;	//выбор линии прерываний по приему - 1
	CAN_Message_16[3].MOFCRH = 0x0800;	//запретить прерываение на прием
	
	//20X
	CAN_Message_16[4].MOCTRH = 0x0080;
  CAN_Message_16[4].MOCTRL = 0x0000;
  CAN_Message_16[4].MOIPRL = 0x0002;	// выбор линии прерываний по приему - 2
	CAN_Message_16[4].MOFCRH = 0x0800;	//запретить прерываение на прием
   */ 
	System_Delay(10);
	
	CAN_Message_16[0].MOARH = 0x8000;	 // идентификатор 000(XXX)h - динамически ИД
  CAN_Message_16[0].MOAMRH = 0x3FFF; //	маска идентификатора	
		//БДМГ
	CAN_Message_16[1].MOARH = 0x9484;	 // 0x8000 идентификатор 521(XXX)h - динамически ИД
  CAN_Message_16[1].MOAMRH = 0x3FFF;// 0x2000; //	маска идентификатора		
	
	CAN_Message_16[2].MOARH = 0x9C84;	 // идентификатор 721
  CAN_Message_16[2].MOAMRH = 0x3FFF; //	маска идентификатора	
		//БДГП
	CAN_Message_16[3].MOARH = 0x8A00;	 // идентификатор 280
  CAN_Message_16[3].MOAMRH = 0x3FFF; //	маска идентификатора	

	CAN_Message_16[4].MOARH = 0x9C8C;	 // идентификатор 723
  CAN_Message_16[4].MOAMRH = 0x3FFF; //	маска идентификатора	
	
	CAN_Message_16[5].MOARH = 0x9C90;	 // идентификатор 724
  CAN_Message_16[5].MOAMRH = 0x3FFF; //	маска идентификатора	
		//БДГП-С
	CAN_Message_16[6].MOARH = 0x8000;	 // идентификатор 000 - 004
  CAN_Message_16[6].MOAMRH = 0x3FE3; //	маска идентификатора	
	
	CAN_Message_16[7].MOARH = 0x8804;	 // идентификатор 200 - 204
  CAN_Message_16[7].MOAMRH = 0x3FFF; //	маска идентификатора	
	
	CAN_Message_16[8].MOARH = 0x8808;	 // идентификатор 202
  CAN_Message_16[8].MOAMRH = 0x3FFF; //	маска идентификатора	
	
	CAN_Message_16[9].MOARH = 0x880C;	 // идентификатор 203
  CAN_Message_16[9].MOAMRH = 0x3FFF; //	маска идентификатора	
	
	CAN_Message_16[10].MOARH = 0x8810;	 // идентификатор 204
  CAN_Message_16[10].MOAMRH = 0x3FFF; //	маска идентификатора	
	
	CAN_Message_16[11].MOARH = 0x9080;	 // идентификатор 420 - 424
  CAN_Message_16[11].MOAMRH = 0x3FE3; //	маска идентификатора	
	
	
	
/*	
	CAN_Message_16[3].MOARH = 0x9480;	 // идентификатор 300h
  CAN_Message_16[3].MOAMRH = 0x3FE3; //	маска идентификатора	(00X, где X - 0,1,2,3,4)
	
	CAN_Message_16[4].MOARH = 0x8800;	 // идентификатор 20Xh	(00X, где X - 0,1,2,3,4)
  CAN_Message_16[4].MOAMRH = 0x3FE3; //	маска идентификатора	
*/
	System_Delay(10);
	
	PANCTR_H = 0x0100;				 // message object 0 -> List 1
  PANCTR = 0x0002;				   
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	 
	 
	PANCTR_H = 0x0101;				 //	message object 1 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	 
	PANCTR_H = 0x0102;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 

	PANCTR_H = 0x0103;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	PANCTR_H = 0x0104;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	PANCTR_H = 0x0105;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 

	PANCTR_H = 0x0106;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	PANCTR_H = 0x0107;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	PANCTR_H = 0x0108;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	PANCTR_H = 0x0109;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	PANCTR_H = 0x010A;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	 
	PANCTR_H = 0x010B;				 //	message object 2 -> List 1
  PANCTR = 0x0002;			    	 
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	 
	  

	CAN_Message_16[0].MOCTRH = 0x0020; 
	CAN_Message_16[0].MOCTRL = 0x0000;
	
	CAN_Message_16[1].MOCTRH = 0x0020; 
	CAN_Message_16[1].MOCTRL = 0x0000;

	CAN_Message_16[2].MOCTRH = 0x0020; 
	CAN_Message_16[2].MOCTRL = 0x0000;
	
	
	CAN_Message_16[3].MOCTRH = 0x0020; 
	CAN_Message_16[3].MOCTRL = 0x0000;
	
	CAN_Message_16[4].MOCTRH = 0x0020; 
	CAN_Message_16[4].MOCTRL = 0x0000;

	CAN_Message_16[5].MOCTRH = 0x0020; 
	CAN_Message_16[5].MOCTRL = 0x0000;
	
	CAN_Message_16[6].MOCTRH = 0x0020; 
	CAN_Message_16[6].MOCTRL = 0x0000;
	
	CAN_Message_16[7].MOCTRH = 0x0020; 
	CAN_Message_16[7].MOCTRL = 0x0000;
	
	CAN_Message_16[8].MOCTRH = 0x0020; 
	CAN_Message_16[8].MOCTRL = 0x0000;
	
	CAN_Message_16[9].MOCTRH = 0x0020; 
	CAN_Message_16[9].MOCTRL = 0x0000;
	
	CAN_Message_16[10].MOCTRH = 0x0020; 
	CAN_Message_16[10].MOCTRL = 0x0000;
	
	CAN_Message_16[11].MOCTRH = 0x0020; 
	CAN_Message_16[11].MOCTRL = 0x0000;
	
	
	
/*	
	CAN_Message_16[3].MOCTRH = 0x0020; 
	CAN_Message_16[3].MOCTRL = 0x0000;
	
	CAN_Message_16[4].MOCTRH = 0x0020; 
	CAN_Message_16[4].MOCTRL = 0x0000;	
*/	
}
void CAN_Process()
{
	//если в очереди есть элементы
	if(canQuery.Counts > 0)
	{
		if(canQuery.QuerySending == 0){
			canQuery.QuerySending = 1;
			CAN0IR = 1;
		}
	}
}


/*
Отправка сообщения в очередь отправки
*/
void CAN_SendMessage(uint16 id,uint8 *pData,uint8 Len)
{
	uint16	curIndex = canQuery.Counts;
	
	canQuery.Messages[curIndex].CanID = id;
	canQuery.Messages[curIndex].Length = Len;
	canQuery.Messages[curIndex].DefferedTime = 0;
		memcpy(canQuery.Messages[curIndex].Data,pData,sizeof(uint8) * 8);
	
	canQuery.Counts++;	
}
void CAN_SendDefferedMessage(uint16 id,uint8 *pData,uint8 Len,uint32 defTime)
{
	uint16	curIndex = canQuery.Counts;
	
	canQuery.Messages[curIndex].CanID = id;
	canQuery.Messages[curIndex].Length = Len;
	canQuery.Messages[curIndex].DefferedTime = defTime;
		memcpy(canQuery.Messages[curIndex].Data,pData,sizeof(uint8) * 8);
	
	canQuery.Counts++;	
}
/*
Отправка сообщения по прерыванию
*/
void CAN_SendMessageIT(uint16 id,uint8 *pData,uint8 Len)
{ 
	
	uint8 *moData = 0;
	uint8	index = 0;

	CAN_Message_16[0].MOFCRH &= 0xF0FF;
	CAN_Message_16[0].MOFCRH |= (uint16) (Len<<8);
	
	CAN_Message_16[0].MOARH &= 0x8000;
	CAN_Message_16[0].MOARH |= (uint16)(id<<2);
	
	
	moData = (uint8*)&CAN_Message_16[0].MODATALL;
	
	for(index = 0;index<Len;index++)
	{
		*(moData + (index)) = *(pData + index);
	}
	
	txEnable = 0;

	CAN_Message_16[0].MOCTRH = 0x0100;	
	
}
/*
void CAN_SendProtocolData(uint8 moNubmer,uint8	*pData, int packSize)
{
	int msgCnt = packSize / 8;
	int lastMsgSize = packSize % 8;
	int i,j = 0;
	
	uint8	arrData[8];
	
	for(i = 0;i < msgCnt;i++)
	{
		for(j = 0;j<8;j++)
			arrData[j] = *(pData + j + (i*8));
		
		CAN_SendMessage(moNubmer,arrData,8);
	}
}
*/
