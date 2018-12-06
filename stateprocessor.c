#include "stateprocessor.h"
#include "LCDDriver.h"
#include "Blynk.h"
#include "Limit_Sensor.h" 
#include "stepper.h"

#define FALSE 0
#define TRUE 1
#define ENTERPASSWORD 2;
#define EDITPASSWORD	3;

extern LockState lockState;

void toggleDoor(void){
	if(getDoorStatus() == CLOSED){
		while(getDoorStatus() != OPEN) door_Open(10*speed);
		updateToOpenState();
	}
	else if(getDoorStatus() == OPEN){
		while(getDoorStatus() != CLOSED) door_Close(10*speed);
		updateToClosedState();
	}
}

void eraseEntireInputBuffer(){
	for (int i=0; i<4; i++)
	{
		lockState.inputPasswordbuffer[i]=' ';
	}
	lockState.indexInputPasswordBuffer=0;
}

//resets index and erases astriks
void resetInputAndDisplay(){
	lockState.inputPasswordbuffer[0]=' ';
	lockState.indexInputPasswordBuffer=0;
	RemoveAstrick(3);
	RemoveAstrick(2);
	RemoveAstrick(1);
	RemoveAstrick(0);
}

void addValueToInputBuffer(char input){
	if (lockState.indexInputPasswordBuffer<4)
	{
		lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=input;
		lockState.indexInputPasswordBuffer=(lockState.indexInputPasswordBuffer+1);
	}
}
//check whether or not the right password was inputted
void correctPasswordChangeState(bool change){
	if(change)
	{
		while(getDoorStatus() != OPEN) door_Open(10*speed);
		lockState.indexState=OPEN;
		lockState.inputPasswordbuffer[0]=' ';
		lockState.indexInputPasswordBuffer=0;
		//	Closed to Open erase buffer input once open 
		OpenPage();
	}else if(lockState.indexInputPasswordBuffer==4)//TODO TEST this case
	{
		lockState.passwordAttempts++;
		resetInputAndDisplay();
		if(lockState.passwordAttempts==3)
		{
			updateToClosedState();
			lockState.lockInputs=true;
			CloseDisabledPage();
		}
		else
		{
			//erase inputPasswordbuffer
			DisplayWrongPassword();
		}
	}//else invalid sound
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
		lockState.passwordAttempts=0;
		return true;
	}
	return false;
}

void removeBufferInput(){
	if (lockState.indexInputPasswordBuffer>0)
	{
		
		lockState.indexInputPasswordBuffer--;
		lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=' ';
		RemoveAstrick(lockState.indexInputPasswordBuffer);
	}else if (lockState.indexInputPasswordBuffer==0)
	{
		lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=' ';
	}
	/*else generate invalid sound*/
}

//reenter password state determines 2 things 
// one is if the right password has been re entered
// second whether or not to move states
void reEnterNewPasswordState(){
	static char storeTempNewPassword[4];
	int i;
	if (lockState.indexInputPasswordBuffer==4)
	{
		if (!lockState.reEnterNewPassword)
			{
				//first run through state
				lockState.reEnterNewPassword=true;
				storeTempNewPassword[0]=lockState.inputPasswordbuffer[0];
				storeTempNewPassword[1]=lockState.inputPasswordbuffer[1];
				storeTempNewPassword[2]=lockState.inputPasswordbuffer[2];
				storeTempNewPassword[3]=lockState.inputPasswordbuffer[3];
				resetInputAndDisplay();
				DisplayEnterAgain();
			}
		else
		{
			lockState.reEnterNewPassword=false;
			for (i=0; i<4; i++)
				if (storeTempNewPassword[i]!=lockState.inputPasswordbuffer[i])
				{
					resetInputAndDisplay();
					DisplayNotMatch();
					return;
				}
			for (i=0; i<4; i++)
				lockState.passwordBuffer[i] = lockState.inputPasswordbuffer[i];
			setState();
			resetInputAndDisplay();
			OpenPage();
		}
	}
}
void processEditPasswordState(char input){
	switch(input){
		case 'D':
			removeBufferInput();
			break;
		case '#':
			//make correct passwords change 
			reEnterNewPasswordState();
			break;
		case 'B':
			toggleDoor();
			resetInputAndDisplay();
			OpenPage();
			break;
		default:
			addValueToInputBuffer(input);
			DisplayAstrick(lockState.indexInputPasswordBuffer);
			break;
	}
	
}
void processOpenState(char input){
	switch(input){
		case 'C':
			lockState.indexState=CLOSED;
			resetInputAndDisplay();
			ClosePage();
			//RUN MOTOR FLAG
		break;
		case 'A':
			lockState.indexState=EDITPASSWORD;
			NewPasswordPage();
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
			break;
		case 'D':
			removeBufferInput();
			break;
		case 'B':
			lockState.indexState=CLOSED;	//might need to be fixed
			resetInputAndDisplay();
			ClosePage();
			break;
		default:
			addValueToInputBuffer(input);
			DisplayAstrick(lockState.indexInputPasswordBuffer);
			break;
	}
}
void processClosedState(char  input){
	
		if (input=='*' && !lockState.lockInputs)
		{
			lockState.indexState=ENTERPASSWORD;
			EnterPasswordPage();
			//GENERATE VALID SOUND
		}
		/*else
		{
			
			//generate invalid sound
		}*/
}
//TODO: TIMER for Display wifi
void processInput(uint32_t currentState,char input, uint32_t passwordSize){
	if(getKeypadAccessStatus() == TRUE && getDoorStatus() != LIMBO){
		switch(currentState)
		{
			//set state with getDoorStatus() at some point
			case 0	://CLOSED
				processClosedState(input);
				break;
			case 1	://OPEN
				processOpenState(input);
				break;
			case 2	://ENTER PASSWORD
				processEnterPasswordState(input);
				break;
			case 3	://EDIT PASSWORD
				processEditPasswordState(input);
				break;
		}
	}
}

// used for Blynk functionality
void stateChangeToRstPassword(void){
	lockState.indexState = 3;
}

void setState(void){
	if(getDoorStatus() == OPEN) lockState.indexState = OPEN;
	else if(getDoorStatus() == CLOSED) lockState.indexState = CLOSED;
}

void updateToOpenState(void){
	lockState.indexState = OPEN;
}

void updateToClosedState(void){
	lockState.indexState = CLOSED;
}
