#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "Timers.h"
#include "UART.h"
#include "PortF.h"
#include "esp8266.h"
#include "Blynk.h"
#include "stepper.h"

void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#define speed 1600
int main(void){       
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
  PortF_Init();
  LastF = PortF_Input();
	Output_Init();        // initialize ST7735
#ifdef DEBUG3
  ST7735_OutString("EE445L Lab 4D\nBlynk example\n");
#endif
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\rBlynk example");
#endif
  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C,800000); 
  // check for receive data from Blynk App every 10ms
  Timer3_Init(&SendInformation,40000000); 
  // Send data back to Blynk App every 1/2 second
  EnableInterrupts();

  /*while(1) {   
    WaitForInterrupt(); // low power mode
  }*/
	
	/*stepper motor code calls*/
	Stepper_Init();
  Stepper_CW(speed);   // Pos=1; GPIO_PORTD_DATA_R=9
  Stepper_CW(speed);   // Pos=2; GPIO_PORTD_DATA_R=5
  Stepper_CW(speed);   // Pos=3; GPIO_PORTD_DATA_R=6
  Stepper_CW(speed);   // Pos=4; GPIO_PORTD_DATA_R=10
  Stepper_CW(speed);   // Pos=5; GPIO_PORTD_DATA_R=9
  Stepper_CW(speed);   // Pos=6; GPIO_PORTD_DATA_R=5
  Stepper_CW(speed);   // Pos=7; GPIO_PORTD_DATA_R=6
  Stepper_CW(speed);   // Pos=8; GPIO_PORTD_DATA_R=10
  while(1){
    Stepper_CW(10*speed);   // output every 10ms
  }
	
}
