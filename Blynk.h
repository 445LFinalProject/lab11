#include <stdint.h>
volatile static uint32_t LED;      // VP1
volatile static uint32_t LastF;    // VP74
void TM4C_to_Blynk(uint32_t pin,uint32_t value);
void Blynk_to_TM4C(void);
void SendInformation(void);

#ifndef _PORTFH_
#define _PORTFH_
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10    // on the left side of the Launchpad board
#define SW2       0x01    // on the right side of the Launchpad board
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
// Initialize PortF switches and LEDs
void PortF_Init(void);
// read switches,
// Bit4 and Bit0 are in negative logic
// returns 0x00,0x01,0x10, or 0x11
uint32_t PortF_Input(void);

// Output to LEDs
// LEDs on bits 1,2,3
void PortF_Output(uint32_t data);
#endif
