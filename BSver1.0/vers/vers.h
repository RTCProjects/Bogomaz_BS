#ifndef _VERS_H
#define _VERS_H

#include "main.h"

typedef  struct
{
	uint8 		frmVer;			//X.X - 4bits
	uint8 		prVer;			//X.X	-	4bits
	uint32		serialNum;	//HEX
}sServiceMsg;

extern const sServiceMsg	ServiceMsg;
#endif
