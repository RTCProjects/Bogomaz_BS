#ifndef _CAN_H
#define _CAN_H

#include "main.h"

#define CAN_TX_QUERY_SIZE 100

typedef struct
{
	uint16 				CanID;
	uint8	 				Data[8];
	uint8					Length;
	uint32	DefferedTime;
}tCanSendMsg;

typedef struct
{
	tCanSendMsg	Messages[CAN_TX_QUERY_SIZE];
	uint16			Counts;
	uint8				QuerySending;
}tCanQueryMessages;

void CAN_Init(void);
void CAN_SendMessage(uint16 id,uint8 *pData,uint8 Len);
void CAN_SendMessageIT(uint16 id,uint8 *pData,uint8 Len);
void CAN_SendDefferedMessage(uint16 id,uint8 *pData,uint8 Len,uint32 defTime);
void CAN_SendProtocolData(uint8 moNubmer,uint8	*pData, int packSize);
void CAN_Process(void);

#endif
