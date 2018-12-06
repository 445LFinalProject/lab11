#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "Timers.h"
#include "UART.h"
#include "Ports.h"
#include "esp8266.h"
#include "Blynk.h"
#include "stepper.h"
#include "Limit_Sensor.h"
#include "matrix.h"
#include "stateprocessor.h"
#include "LCDDriver.h"

void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s


char Buffer[4];
char Password[4]="1234";
uint32_t KeyCount;

LockState lockState = {false,false,0,"1234","    ",0,{0,1,2,3},0,false};
int main(void){
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
  
	uint32_t i=0;
	Matrix_Init();				//Testing matrix
	Output_Init();
	Stepper_Init();
	EnableInterrupts();

  while(1) {
		Buffer[i] = Matrix_InChar();//Testing Matrix
		processInput(lockState.state[lockState.indexState],Buffer[i],0);
		//Display(State)
    i = (i+1)&0x03;
   // WaitForInterrupt(); // low power mode
  }
}
