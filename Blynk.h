#include <stdint.h>
volatile static uint32_t LED;      // VP1
volatile static uint32_t LastF;    // VP74
void TM4C_to_Blynk(uint32_t pin,uint32_t value);
void Blynk_to_TM4C(void);
void SendInformation(void);
