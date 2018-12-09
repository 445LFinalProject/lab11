

#ifndef SPIDRIVER_H
#define SPIDRIVER_H
void DACInit(uint16_t data);
//putting clock set to 10mHZ putting data every 25micro seconds in timer handler
void DACOut(uint16_t data);
void DACOutTest(void);
#endif
