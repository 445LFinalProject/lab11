
#include "stateprocessor.h"
//#include "LCDDriver.c"
#define speed 1600
#define CLOSED 0;
#define OPEN 1;
#define ENTERPASSWORD 2;
#define EDITPASSWORD	3;

extern LockState lockState;

void eraseEntireInputBuffer()
{
	for (int i=0; i<4; i++)
	{
		lockState.inputPasswordbuffer[i]=' ';
	}
	lockState.indexInputPasswordBuffer=0;
}

//check whether or not the right password was inputted
void correctPasswordChangeState(bool change){
	if(change)
	{
		lockState.indexState=OPEN;
		//Flag for motor to move 
		lockState.inputPasswordbuffer[0]=' ';
		lockState.indexInputPasswordBuffer=0;
		//	Closed to Open erase buffer input once open 
		//**************OpenPage();
	}else//TODO TEST this case
	{
		lockState.passwordAttempts++;
		if(lockState.passwordAttempts==3)
		{
			//lock and goes back to state
			lockState.indexState=CLOSED;
			lockState.lockInputs=true;
			//CloseDisabledPage();
		}
		else
		{
			//erase inputPasswordbuffer
			//********8DisplayWrongPassword();
		}
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
		//LCD ERASE ASTRICT METHOD 
	}else if (lockState.indexInputPasswordBuffer==0)
	{
		lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=' ';
	}
	/*else generate invalid sound*/
}

//reenter password state determines 2 things 
// one is if the right password has been re entered
// second whether or not to move states
void reEnterNewPasswordState()
{
	static char storeTempNewPassword[4];
	int i;
	if (lockState.indexInputPasswordBuffer==3)
	{
		if (!lockState.reEnterNewPassword)
			{
				//first run through state
				lockState.reEnterNewPassword=true;
				storeTempNewPassword[0]=lockState.inputPasswordbuffer[0];
				storeTempNewPassword[1]=lockState.inputPasswordbuffer[1];
				storeTempNewPassword[2]=lockState.inputPasswordbuffer[2];
				storeTempNewPassword[3]=lockState.inputPasswordbuffer[3];
				lockState.indexState=EDITPASSWORD;
				//**********DisplayEnterAgain();
				removeBufferInput();
				removeBufferInput();
				removeBufferInput();
				removeBufferInput();
				//
			}
		else
		{
			lockState.reEnterNewPassword=false;
			for (i=0; i<4; i++)
				if (storeTempNewPassword[i]!=lockState.inputPasswordbuffer[i])
				{
					eraseEntireInputBuffer();
					//***********DisplayNotMatch();
					return;
				}
			removeBufferInput();
			removeBufferInput();
			removeBufferInput();
			removeBufferInput();
			for (i=0; i<4; i++)
				lockState.passwordBuffer[i] = lockState.inputPasswordbuffer[i];
			lockState.indexState=OPEN;
			//**************OpenPage();
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
		default:
			lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=input;
			lockState.indexInputPasswordBuffer=(lockState.indexInputPasswordBuffer+1)&0x03;
			//*************DisplayAstrick(lockState.indexInputPasswordBuffer);
			break;
	}
	
}
void processOpenState(char input){
	switch(input){
		case 'C':
			lockState.indexState=CLOSED;
			//*************ClosePage();
			//RUN MOTOR FLAG
		break;
		case 'A':
			lockState.indexState=EDITPASSWORD;
			//*************NewPasswordPage();
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
			//*************ClosePage();
			break;
		default:
			lockState.inputPasswordbuffer[lockState.indexInputPasswordBuffer]=input;
			lockState.indexInputPasswordBuffer=(lockState.indexInputPasswordBuffer+1)&0x03;
			//**************DisplayAstrick(lockState.indexInputPasswordBuffer);
			break;
	}
}
void processClosedState(char  input){
	
		if (input=='*')
		{
			lockState.indexState=ENTERPASSWORD;
			//GENERATE VALID SOUND
		}
		/*else
		{
			
			//generate invalid sound
		}*/
}
//TODO: TIMER for Display wifi
void processInput(uint32_t currentState,char input, uint32_t passwordSize){
	uint32_t idk;
	switch(currentState)
	{
		case 0	://CLOSED
			processClosedState(input);
			//************ClosePage();
			break;
		case 1	://OPEN
			processOpenState(input);
			break;
		case 2	://ENTER PASSWORD
			//TODO /implement method for when to many attempts
			//			are performed
			//**************EnterPasswordPage();
			processEnterPasswordState(input);
			break;
		case 3	://EDIT PASSWORD
			processEditPasswordState(input);
			break;
	}
}