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

// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num;
uint32_t pin_int;

 // Initialize PortF switches and LEDs
void PortF_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0F;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  //GPIO_PORTF_PUR_R = 0x0F;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x0F;          // 7) enable digital I/O on PF4-0
}
// read switches,
// Bit4 and Bit0 are in negative logic
// returns 0x00,0x01,0x10, or 0x11
uint32_t PortF_Input(void){
  return (GPIO_PORTF_DATA_R&0x11);  // read PF4,PF0 inputs
}
// Output to LEDs
// LEDs on bits 1,2,3
void PortF_Output(uint32_t data){ // write PF3-PF1 outputs
  GPIO_PORTF_DATA_R = data;
}


// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}


// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){int j; char data;
// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');
#endif

// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
    if(pin_num == 0x01)  {
      LED = pin_int;
      PortF_Output(LED<<2); // Blue LED
#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
    }                               // Parse incoming data
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif
  }
}

void SendInformation(void){
  uint32_t thisF;
  thisF = PortF_Input();
// your account will be temporarily halted if you send too much data
  if(thisF != LastF){
    TM4C_to_Blynk(74, thisF);  // VP74
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    ST7735_OutUDec(thisF);
    ST7735_OutChar('\n');
#endif
  }
  LastF = thisF;
}



void blynkPorfF(void)
{
	GPIO_PORTF_DATA_R ^= 0x01;
}


LockState lockState = {false,false,0,"1234","    ",0,{0,1,2,3},0,false};
int main(void){
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
  
	PortF_Init();
	/*
  LastF = PortF_Input();
	Output_Init();        // initialize ST7735
	//ST7735_InitR(INITR_REDTAB);
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

  Timer2A_Init(&Blynk_to_TM4C,800000,4);
  // check for receive data from Blynk App every 10ms
  Timer3A_Init(&SendInformation,40000000,4);
  // Send data back to Blynk App every 1/2 second
  */
	uint32_t i=0;
	Matrix_Init();				//Testing matrix
	InitializeLCD();
	//blynks every two seconds
	Timer5A_Init(blynkPorfF,8000000,1);
	EnableInterrupts();

  while(1) {
		Buffer[i] = Matrix_InChar();//Testing Matrix
		processInput(lockState.state[lockState.indexState],Buffer[i],0);
		//Display(State)
    i = (i+1)&0x03;
   // WaitForInterrupt(); // low power mode
  }

	//stepper motor code calls*/
	Stepper_Init();
  while(1){
    //door_Open(10*speed);   // output every 10ms
  }
}
