#include "asc0.h"
#include "protocol.h"

static bit volatile transmit_finish; 
static bit volatile empty_TBUF;
static bit volatile recive_finish;            


void serial_TX_irq (void) interrupt S0TINT = 42 {
  transmit_finish = 1;           
}

void serial_TX_BUF_irc (void) interrupt S0TBINT = 71 {
  empty_TBUF = 1;
}

void serial_RX_irq (void) interrupt S0RINT = 43 {
 
   
	IMDB_ReceiveByteCallback(S0RBUF);
	
	recive_finish = 1;    
	
}

//char putchar (char c) {           // substitute function for putchar 
void sendbyte(char c){
  while (!transmit_finish & !empty_TBUF & !S0TIR & !S0TBIR);
  _atomic_(0); 
  if (transmit_finish) transmit_finish = 0;
  if (empty_TBUF) empty_TBUF = 0;
  _endatomic_();
  _atomic_(0); 
  S0TIR = 0;
  S0TBIR = 0;
  _endatomic_();
  _atomic_(0);                   // start un-interruptable code 
  S0TBUF = c;
  _endatomic_();
//  return(c);
}
/*
char _getkeyserial_RX (void) { // substitute function for _getkey 
  while (!recive_finish & !S0RIR);
  _atomic_(0);                   // start un-interruptable code 
  recive_finish = 0;
  S0RIR = 0;      
  _endatomic_();			   // end un-interruptable code
  return(S0RBUF);
}
   */
void init_asc0 (unsigned long freq, unsigned int baud) {
 
	
	S0BG = (unsigned int)(freq / (32*(unsigned long) baud)) - 1;  // baudrate reload
  DP3 |= 0x0400;                                       // Set TXD for output 
  DP3 &= 0xF7FF;                                      // Set RXD for input 
  ALTSEL0P3 |= 0x0400;                                 // Configure port pins for serial interface 0    
  P3 |= 0x0400;                                       // Set TXD high 
  S0CON = 0x8011;
  S0RIC = 0x0044;                                     // Enable serial receive interrupt 
  S0TIC = 0x0045;                                     // Enable serial transmit interrupt 
  transmit_finish = 1;
}
