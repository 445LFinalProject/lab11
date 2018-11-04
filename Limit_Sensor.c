#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"

#define	CLOSED 0
#define OPEN 	1
#define LIMBO 2
void (*Door_Close)(void);    // user function to be executed on touch
void (*Door_Open)(void);  // user function to be executed on release

void GPIOLimitArm(void){
	GPIO_PORTF_ICR_R |= 0x03;			//	acknowledge interrupt flag for PF0,1
	GPIO_PORTF_IM_R |= 0x03;			//	arm PF0,1
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x20;      			// (h) enable interrupt NVIC 
}

void Limit_Sensor_Initialization(void(*doorclose)(void), void(*dooropen)(void)){
	  // **** general initialization ****
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
  GPIO_PORTF_DIR_R &= ~0x03;    			// (c) make PF0,1 in
  GPIO_PORTF_AFSEL_R &= ~0x03; 			 	//     disable alt funct on PF0,1
  GPIO_PORTF_DEN_R |= 0x03;     			//     enable digital I/O on PF0,1   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; 	// configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       			//     disable analog functionality on PF0,1
  GPIO_PORTF_IS_R &= ~0x03;     			// 		 PF0,1 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x03;     			//     PF0,1 is NOT both edges
	GPIO_PORTF_IEV_R |= 0x03;						//	PF0,1 is rising edge
  GPIOLimitArm();

  Door_Close = doorclose;           // user function 
  Door_Open = dooropen;       		// user function 
	GPIOLimitArm();
}

void GPIOPortF_Handler(void){
	GPIO_PORTF_IM_R &= ~0x03;				//disabling interrupts for limit sensors
	
	if(GPIO_PORTF_RIS_R&0x01){
		(*Door_Close)();
	}
	
	else if(GPIO_PORTF_RIS_R&0x02){
		(*Door_Open)();
	}
	
	SysTick_Wait10ms(1);
	GPIOLimitArm();
}

uint8_t get_Door_Status(void){
	//make sure limit sensor output when pressed is 1
	if(GPIO_PORTF_DATA_R&0x01) return CLOSED;
	else if(GPIO_PORTF_DATA_R&0x02) return OPEN;
	else return LIMBO;
}
