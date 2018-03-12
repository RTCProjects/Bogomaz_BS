#ifndef _ASC0_H
#define _ASC0_H

#include "main.h"

void init_asc0(unsigned long freq, unsigned int baud);
void sendbyte(char c);
#endif
