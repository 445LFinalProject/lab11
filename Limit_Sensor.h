#include <stdint.h>

void Limit_Sensor_Initialization(void(*touchtask)(void), void(*releasetask)(void));
int8_t get_Door_Status(void);
