//intilize pf6 such that it output a value 

#include <stdint.h>
#include "../../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include <stdbool.h>

#define PB7 (*((volatile uint32_t *)0x40005200))
extern bool togglePD;
void DACInit(uint16_t data)
{
	volatile uint32_t delay;
	SYSCTL_RCGCSSI_R |= 0x02;       // activate SSI1
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};	// ready?
  GPIO_PORTF_AFSEL_R |= 0x0E;     // enable alt funct on PD0,1,3
  GPIO_PORTF_DEN_R |= 0x0E;       // configure PD0,1,3 as SSI
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00002220;
  GPIO_PORTF_AMSEL_R = 0;         // disable analog functionality on PA
	/*
	SYSCTL_RCGCGPIO_R |= 0x000000008;     // 1) activate clock for Port D
	SYSCTL_RCGCSSI_R |= 0x0000000002; 
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	//PD0 TRANSMIT PD1 FSS PD0 CLOCK  pd3 tx 0x0B
  GPIO_PORTD_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port D
  GPIO_PORTD_AFSEL_R |=0xB;
	GPIO_PORTD_DEN_R |= 0xB;
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00002022;
	GPIO_PORTD_AMSEL_R = 0;
	*/
	
	SSI1_CR1_R = 0;
	SSI1_CPSR_R = 0x02;	   //10MHZ = 80M/(cpsdvr*(1+scr))   spsdvr =
	SSI1_CR0_R &= ~(0x0000FFF0) ; //scr=0,sph=0, spo=0 freescale 
	SSI1_CR0_R |= 0x34F;  //F is for 16 bits of transmission and turns spo=1  scr=3
	SSI1_CR1_R = 0x2; //enabled ssi
  

}
//putting clock set to 10MHZ putting data every 25micro seconds in timer handler
void DACOut(uint16_t data)
{
	data &= 0x0FFF;
	while ( (SSI1_SR_R & SSI_SR_TNF)==0){}
	SSI1_DR_R = data;
}


void DACOutTest()
{
	int static i = 0;
		DACOut(i);
	i = (i+1)&0xFFF;
}
