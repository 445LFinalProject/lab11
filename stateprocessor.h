#ifndef _STATEPROCESSORH_
#define _STATEPROCESSORH_

#include "stdbool.h"
#include "stdint.h"
struct LockState{
	bool wifiCommunication;
	uint8_t passwordAttempts;
	char passwordBuffer[4];
	char inputPasswordbuffer[4];
	uint8_t indexInputPasswordBuffer;
	uint8_t state[4];
	uint8_t indexState;
	uint8_t lockInputs;
};
typedef struct LockState LockState;

void correctPasswordChangeState(bool change);
bool validComparison();
bool deletePassword();
bool comparePasswords(bool compare);
void removeBufferInput();
void processEditPasswordState(char input);
void processOpenState(char input);
void processEnterPasswordState(char input);
void processClosedState(char  input);
void processInput(uint32_t currentState,char input, uint32_t passwordSize);

typedef struct LockState LockState;
#endif
