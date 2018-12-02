
#include "stateprocessor.h"
#define speed 1600
#define CLOSED 0;
#define OPEN 1;
#define ENTERPASSWORD 2;
#define EDITPASSWORD	3;

extern LockState lockState;

//This function will change
//TODO else statement, erase completely password
void correctPasswordChangeState(bool change){
	if(change)
	{
		lockState.indexState=OPEN;
		//Flag for motor to move 
		lockState.inputPasswordbuffer[0]=' ';
		lockState.indexInputPasswordBuffer=0;
		//	Closed to Open erase buffer input once open 
		
	}else
	{
		lockState.passwordAttempts++;
		if(lockState.passwordAttempts==3)
		{
			//lock and goes back to state
			lockState.indexState=CLOSED;
			lockState.lockInputs=true;
		}
		/*else TODO
		{
			//erase inputPasswordbuffer
			//same state but show in display incorrect password
		}*/
	}
}
bool validComparison(){
	//	come back to see if we can make a variable in lockstate such that
	//	we can add more values to the buffer 
	return lockState.indexInputPasswordBuffer==4;
}

bool comparePasswords(bool compare){
	if (compare)
	{
		for (int i=0; i<4; i++)
		{
			if (lockState.passwordBuffer[i]!=lockState.inputPasswordbuffer[i])
				return false;
		}
	}
	return true;
}
void removeBufferInput(){
	if (lockState.indexInputPasswordBuffer>0)
	{
		lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=' ';
		lockState.indexInputPasswordBuffer--;
		//lcd must be notified.
	}else if (lockState.indexInputPasswordBuffer==0)
	{
		lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=' ';
	}
	/*else generate invalid sound*/
}
static uint8_t editTry=0;
//access global variable
void compareEditPasswords()
{
	
}
void correctEditPasswordChangeState(){
	
}
//TODO: check in which state one is.
//when pressing # check what state one is. Two possibilities.
//first check in input is correct.
//if in state one save password and dont change
//else if in two check if input is correct with past
//						if it is then change lockState.passwordBuffer 
//						else if it is not correct then don't do anything
void processEditPasswordState(char input){
	switch(input){
		case 'D':
			removeBufferInput();
			break;
		case '#':
			//make correct passwords change 
			
			//correctEditPasswordChangeState(compareEditPasswords(validComparison()));
			break;
		//default:
			//valid sound
	}
	
}
void processOpenState(char input){
	switch(input){
		case 'C':
			lockState.indexState=CLOSED;
		break;
		case 'A':
			lockState.indexState=EDITPASSWORD;
		break;
		default:
			//generate invalid sound
			break;
	}

}
void processEnterPasswordState(char input){
	switch(input){
		case '#':
			correctPasswordChangeState(comparePasswords(validComparison()));
		case 'D':
			removeBufferInput();
			break;
		case 'B':
			lockState.indexState=CLOSED;
			break;
		default:
			lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=input;
			lockState.indexInputPasswordBuffer=(lockState.indexInputPasswordBuffer+1)&0x03;
			break;
	}
}
void processClosedState(char  input){
	
		if (input=='*')
		{
			lockState.indexState=ENTERPASSWORD;
			//GENERATE VALID SOUND
		}
		/*
		else
			generate invalid sound
		*/
}
void processInput(uint32_t currentState,char input, uint32_t passwordSize){
	uint32_t idk;
	switch(currentState)
	{
		case 0	://CLOSED
			processClosedState(input);
			break;
		case 1	://OPEN
			processOpenState(input);
			break;
		case 2	://ENTER PASSWORD
			//TODO /implement method for when to many attempts
			//			are performed
			processEnterPasswordState(input);
			break;
		case 3	://EDIT PASSWORD
			processEditPasswordState(input);
			break;
	}
}