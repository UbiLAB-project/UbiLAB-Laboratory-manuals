
/**
  ******************************************************************************
  * @file               : TCLab.c (Python)
  * @brief              : TCLab functions (Temperature Control Lab)
  * @STM Code Author    : Andrej Sarjas (andrej.sarjas@um.si)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 Andrej Sarjas.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ANDY under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  **************************************************************************
  **************************************************************************
  *******************************************************************************
  *@ Special attention
  * Link  :  http://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl
  * Author of Temperature Control Lab :  prof. John D. Hedengren  (john.hedengren [at] byu.edu)
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "TCLab.h"
#include "stdio.h"      /* printf, NULL */
#include "stdlib.h"
extern UART_HandleTypeDef huart3; //Communication VCP

/*Variables taken from TCLab script*/

// constants
char String_vers[] = "2.0.1";   // version of this firmware
int  baud = 115200;      // serial baud rate
char sp = ' ';           // command separator
char nl = '\n';          // command terminator

// pin numbers corresponding to signals on the TC Lab Shield





uint8_t newData = false;       // boolean flag indicating new command

int buffer_index=0;
char Buffer[64];              //Receive RX Buffer
int  len=0;
char RX_data;
char cmd[15];                 // Receive string for command from Buffer
char value_str[15];           // Receive string for number  from Buffer
char float_bytes[4];          //Convert to bytes
float val=0.0;                // Float number converted from value_str[]
int cmd_length=0;             //Received command length
int val_length=0;             //Received number length
int   real_time_flag=0;     // Real time flag for controller update from ARM
uint8_t USART3_busy_flag=0;

extern int controller_on;
extern float ref;
extern float Kp, Ti, Td;
extern uint16_t PWM_FAN;
extern char Buffer_data[64];
extern float Kp, Ti, Td;
/***************************************************************************************************/
/***************************************************************************************************/
/* Read with serial interrupt --------> called from 'void USART2_IRQHandler(void)' ---------------------------------*/
void Serial_read_main_Vetrovnik(){


	  ReceiveData();

	  if( newData == true)
	  	{

		  LD1(1);
		  checkAlarm();
	      ParseData();
	      dispatchCommand();
	      CleanData();
	      LD1(0);
	  	}



}

/***************************************************************************************************/
/***************************************************************************************************/



/***************************************************************************************************/
/* Receive function */
void ReceiveData()
{
	/*-- Receive single character -- */
	  RX_data=USART3->RDR;

    /*-- Receive data 13-CR  10-LF new line -- */
	  if ((RX_data != '&') && (RX_data != 13) && (RX_data != 10) && (buffer_index < 64)) {   // (RX_data != 13) && (RX_data != 10)&&(buffer_index < 64)
	      Buffer[buffer_index] = RX_data;
	      buffer_index++;
	    }
	    else {
           if(buffer_index!=0)
	        {newData = true;}
	    }


}

/***************************************************************************************************/
/* Parse function */
void ParseData()
{

	 /*-- Parse received data with 'SPACE' character --*/

		  //HAL_UART_Transmit(&huart2, &Buffer, buffer_index, 10); //Received data

            for(int i=0;i<buffer_index;i++)
            {
                     if(Buffer[i]!=32)  //Separate first command with space
                     {
							  //UpperCase
							  if(Buffer[i]>96 && Buffer[i]<123) //lowerCase characters
							  {
								  cmd[cmd_length]=Buffer[i]-32;
							  } else
							  {
								  cmd[cmd_length]=Buffer[i];
							  }
							  	  	  cmd_length++;

                     }else
                     {
                    	 	 break;
                     }
            }

          /*-- Parse received number after command string and 'SPACE character'--*/
		 if(cmd_length!=buffer_index){
				 for(int i=cmd_length+1;i<buffer_index;i++)
				 {
					 if( (Buffer[i]<47 || Buffer[i]>58) && Buffer[i]!=46) //46='.' ASCII numbers=[47  58]
					 {

						 if(val_length!=0)
							  break;

					 }else
					 {
						 if(val_length==0 && Buffer[i]==46)
							 break;
						 else
						 {
							value_str[val_length]=Buffer[i];
							val_length++;
						 }

					 }

				 }
		 }//END IF

		 /*--Convert to float*/
		 if (val_length!=0)
		 {
			 val=strtof(value_str,NULL);
		 }else
		 {
			 val=0.0;
		 }


}/*END OF FUNCTION*/


/***************************************************************************************************/
/*Dispatch Command*/
void dispatchCommand(void) {
	char command_msg[30];

  if (memcmp(cmd ,"START",5)==0) {   //Start real time
     controller_on=1; //Start Controller
    sendResponse("Start");
  }
  else if (memcmp(cmd ,"STOP",4)==0) {   //STOP real time
	  controller_on=0; //Stop Controller
      sendResponse("Stop");

   } else if (memcmp(cmd ,"REF",3)==0) {//Reference value for temp1
	    //temp1_ref=val;

	   if(val_length!=0)
	   {
				ref=val;
				if(ref<100.0)
					{
						ref=100.0;
					}else if(ref>800.0)
					{
						ref=800.0;
					}
	   }
	   sendFloatResponse(ref);

  } else if (memcmp(cmd ,"KP",2)==0) {//Reference value for temp1

	  if(val_length!=0)
		   {
				Kp=val;  //Kp setting of PID controller
				if(Kp<0.0)
				{
					Kp=0.0;
				}else if(Kp>10.0)
				{
					Kp=10.0;
				}
		   }
	  sendFloatResponse(Kp);

   }else if (memcmp(cmd ,"TI",2)==0) {//Reference value for temp1

	   if(val_length!=0)
	   {
			   Ti=val;  //Ti setting of PID controller
				if(Ti<0.01)
				{
					Ti=0.01;
				}else if(Kp>1400.0)
				{
					Ti=1400.0;
				}
	   }
 	   sendFloatResponse(Ti);


    }
  else if (memcmp(cmd ,"TD",2)==0) {//Reference value for temp1

	  if(val_length!=0)
	 	{
	       Td=val;  //Kp setting of PID controller
				if(Td<0)
				{
					Td=0;
				}else if(Td>3.0)
				{
					Td=3.0;
				}
	 	}
	  	sendFloatResponse(Td);


    }else if(memcmp(cmd ,"PWM",3)==0)
    {
    	if(val_length!=0)
    	{
    		PWM_FAN=(int)val;

    				if(PWM_FAN<8000)
    				    {
    						PWM_FAN=8000;
    					}else if(PWM_FAN>8900)
    					{
    						PWM_FAN=8900;
    					}

    	}
    	sendFloatResponse(PWM_FAN);

    }else if(memcmp(cmd ,"PAR",3)==0) //Send current Controller parameters
    {
    	USART3_busy_flag=1;
    	sprintf(Buffer_data,"P %.2f %.2f %.2f %.2f\n",ref,Kp,Ti,Td);
    	HAL_UART_Transmit(&huart3, Buffer_data, strlen(Buffer_data), 10);
    	USART3_busy_flag=0;

    }
    else if (memcmp(cmd ,"VER",3)==0) {
    sendResponse("TCLab Firmware - NUCLEO ARM 767 WindSystem");
	}
	else if (memcmp(cmd ,"X",1)==0) {
		controller_on=0; //Stop Controller
		sendResponse("Stop");
	 }
	 else if (cmd_length > 0) {
		//sendResponse(cmd);
	 }

}


/****************************************************************************************************/
/* Clean function */
void CleanData()
{
	/*Clean buffers*/
	memset(Buffer, 0, sizeof(Buffer));
	memset(value_str, 0, sizeof(value_str));
	memset(cmd, 0, sizeof(cmd));

    /*Reset counters*/
	cmd_length=0;
	buffer_index = 0;
	val_length = 0;
	newData = false;

}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/*SUPORT FUNCTIONS*/

/* Send response */
void sendResponse(char msg[]) {
    int len=strlen(msg);

    USART3_busy_flag=1;
	HAL_UART_Transmit(&huart3, msg, len, 10);
	HAL_UART_Transmit(&huart3, "\n\r", 2, 10);
	USART3_busy_flag=0;
}

void sendFloatResponse(float num)
{
	USART3_busy_flag=1;
	char sendFloatResponse_msg[10];
	int  num_INT=0, len;

	num_INT=(int)num;
	sprintf(sendFloatResponse_msg,"%.2f\n\r",num);
	len=strlen(sendFloatResponse_msg);
	HAL_UART_Transmit(&huart3, &sendFloatResponse_msg, len, 10);
	USART3_busy_flag=0;
}


/* Heater 2 function */
uint8_t temp_alarm=0;
void checkAlarm(void)
{



}
