
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
extern ADC_HandleTypeDef  hadc1;
extern TIM_HandleTypeDef  htim1;  //PWM Q1, Q2
extern UART_HandleTypeDef huart3; //Communication VCP

/*Variables taken from TCLab script*/

// constants
char String_vers[] = "2.0.1";   // version of this firmware
int  baud = 115200;      // serial baud rate
char sp = ' ';           // command separator
char nl = '\n';          // command terminator

// pin numbers corresponding to signals on the TC Lab Shield
//uint8_t pinT1   = 0;         // T1 (PA0)
//uint8_t pinT2   = 2;         // T2 (PA4)
//uint8_t pinQ1   = 3;         // Q1 (PB3)
//uint8_t pinQ2   = 5;         // Q2 (PB4)
//uint8_t pinLED1 = 9;         // LED1 (PC7)

// temperature alarm limits
uint8_t limT1   = 50;       // T1 high alarm (°C)
uint8_t limT2   = 50;       // T2 high alarm (°C)

// LED1 levels
//uint8_t hiLED   =  60;       // hi LED
//uint8_t loLED   = hiLED/16;  // lo LED

long ledTimeout = 0;           // when to return LED to normal operation
float LED = 100;               // LED override brightness
float P1 = 200;                // heater 1 power limit in units of pwm. Range 0 to 255
float P2 = 100;                // heater 2 power limit in units in pwm, range 0 to 255
float Q1 = 0;                  // last value written to heater 1 in units of percent
float Q2 = 0;                  // last value written to heater 2 in units of percent
int alarmStatus;               // hi temperature alarm status
uint8_t newData = false;       // boolean flag indicating new command
int n =  10;                   // number of samples for each temperature measurement
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
float temp1_ref=30;            //Temperature reference for Heater Q1
float temp2_ref=0;            //Temperature reference for Heater Q2
int   real_time_flag=0;     // Real time flag for controller update from ARM

float Kp1=10.0f; Ti1=20.0f; Td1=1.0f;
float Kp2=5.0f; Ti2=20.0f; Td2=10.0f;
float k_pz1=0.1f, a_pz1=0.0f, b_pz1=0.0f;
float k_pz2=0.1f, a_pz2=0.0f, b_pz2=0.0f;
float reg1=1;
float reg2=1;
float u_k1=0.0f; u_k2=0.0f;
float open_loop_enable1=0.0f;
float open_loop_enable2=0.0f;
float open_loop_enable;

/***************************************************************************************************/
/***************************************************************************************************/
/* Read with serial interrupt --------> called from 'void USART2_IRQHandler(void)' ---------------------------------*/
void Serial_read_main_TCLab(){



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
	  if ((RX_data != 13) && (RX_data != 10) && (RX_data != 38) && (buffer_index < 64)) {   // (RX_data != 13) && (RX_data != 10)&&(buffer_index < 64)
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
                     if(Buffer[i]!=32)  //Separte first command with space
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

          /*-- Parse received number after commad string and 'SPACE character'--*/
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
    setHeater1(0);
    setHeater2(0);
    real_time_flag=1;
    sendResponse("Start");
  }
  else if (memcmp(cmd ,"STOP",4)==0) {   //STOP real time
      setHeater1(0);
      setHeater2(0);
      real_time_flag=0;
      sendResponse("Stop");
    }
  else if (memcmp(cmd ,"LED",3)==0) {
//    ledTimeout = millis() + 10000;
//    LED = max(0, min(100, val));
//
     sendResponse("100");
  }
  else if (memcmp(cmd ,"P1",2)==0) {
    P1 = max(0, min(255, val));
    sprintf(command_msg,"%d",(int)P1);
    sendResponse(command_msg);
  }
  else if (memcmp(cmd ,"P2",2)==0) {
    P2 = max(0, min(255, val));
    sprintf(command_msg,"%d",(int)P2);
     sendResponse(command_msg);
  }
  else if (memcmp(cmd ,"REF1",4)==0) {//Reference value for temp1
	    temp1_ref=val;
	    //sendFloatResponse(temp1_ref);
  }
  else if (memcmp(cmd ,"REF2",4)==0) {//Reference value for temp2
	    temp2_ref=val;
	    //sendFloatResponse(temp2_ref);
  }
  else if (memcmp(cmd ,"Q1",2)==0) {
    //setHeater1(val);
	  if(open_loop_enable1==1)
	  {
		  u_k1=val;
	  }
    //sendFloatResponse(Q1);
  }
  else if (memcmp(cmd ,"Q1B",3)==0) {
    setHeater1(val);
    sendBinaryResponse(Q1);
  }
  else if (memcmp(cmd ,"Q2",2)==0) {
    //setHeater2(val);
	  if(open_loop_enable2==1)
	 	  {
		  	  u_k2=val;
	 	 }
    //sendFloatResponse(Q2);
  }
  else if (memcmp(cmd ,"Q2B",3)==0) {
    setHeater1(val);
    //sendBinaryResponse(Q2);
  }
  else if (memcmp(cmd ,"R1",2)==0) {
    sendFloatResponse(Q1);
  }
  else if (memcmp(cmd ,"R2",2)==0) {
    sendFloatResponse(Q2);
  }
  else if (memcmp(cmd ,"SCAN",4)==0) {
    sendFloatResponse(readTemperature(pinT1));
    sendFloatResponse(readTemperature(pinT2));
    sendFloatResponse(Q1);
    sendFloatResponse(Q2);
  }
  else if (memcmp(cmd ,"T1",2)==0) {
    sendFloatResponse(readTemperature(pinT1));
  }
  else if (memcmp(cmd ,"T1B",3)==0) {
    sendBinaryResponse(readTemperature(pinT1));
  }
  else if (memcmp(cmd ,"T2",2)==0) {
    sendFloatResponse(readTemperature(pinT2));
  }
  else if (memcmp(cmd ,"T2B",3)==0) {
    sendBinaryResponse(readTemperature(pinT2));
  }
  else if (memcmp(cmd ,"VER",3)==0) {
    sendResponse("TCLab Firmware - NUCLEO ARM 767 V1");
  }
  else if (memcmp(cmd ,"X",1)==0) {
    setHeater1(0);
    setHeater2(0);
    sendResponse("Stop");
  }
  else if (memcmp(cmd ,"KP1",3)==0) {
	  Kp1=val;
	  //sendFloatResponse(Kp1);
  }
  else if (memcmp(cmd ,"TI1",3)==0) {
	  Ti1=val;
	  //sendFloatResponse(Ti1);
  }
  else if (memcmp(cmd ,"TD1",3)==0) {
	  Td1=val;
	  //sendFloatResponse(Td1);
  }
  else if (memcmp(cmd ,"KP2",3)==0) {
	  Kp2=val;
	  //sendFloatResponse(Kp2);
  }
  else if (memcmp(cmd ,"TI2",3)==0) {
	  Ti2=val;
	  //sendFloatResponse(Ti2);
  }
  else if (memcmp(cmd ,"TD2",3)==0) {
	  Td2=val;
	  //sendFloatResponse(Td2);
  }
  else if (memcmp(cmd ,"SD",2)==0) {

	  if((open_loop_enable1==1)&&(open_loop_enable2==1))
	  {
		  open_loop_enable=11;
	  }
	  else if((open_loop_enable2==1)&&(open_loop_enable1==0))
		{
				  open_loop_enable=1;
		}
	  else if((open_loop_enable1==1)&&(open_loop_enable2==0))
	  {
	 		  open_loop_enable=10;
	 }
	  else
	  {
	 		  open_loop_enable=0;
	 	  }

	  sendFloatResponse2(temp1_ref, open_loop_enable, Kp1, Ti1, Td1, u_k1, temp2_ref, Kp2, Ti2, Td2, u_k2,
			  reg1, k_pz1, a_pz1, b_pz1, reg2, k_pz2, a_pz2, b_pz2);
  }
  else if (memcmp(cmd ,"OL1",3)==0) {
	  open_loop_enable1=val;
  }
  else if (memcmp(cmd ,"OL2",3)==0) {
 	  open_loop_enable2=val;
   }
  else if (memcmp(cmd ,"REG1",4)==0) {
 	  reg1=val;
   }
  else if (memcmp(cmd ,"REG2",4)==0) {
 	  reg2=val;
   }
  else if (memcmp(cmd ,"C1K",3)==0) {
 	  k_pz1=val;
   }
  else if (memcmp(cmd ,"C1A",3)==0) {
 	  a_pz1=val;
   }
  else if (memcmp(cmd ,"C1B",3)==0) {
 	  b_pz1=val;
   }
  else if (memcmp(cmd ,"C2K",3)==0) {
 	  k_pz2=val;
   }
  else if (memcmp(cmd ,"C2A",3)==0) {
 	  a_pz2=val;
   }
  else if (memcmp(cmd ,"C2B",3)==0) {
 	  b_pz2=val;
   }
  else if (cmd_length > 0) {
    setHeater1(0);
    setHeater2(0);
    sendResponse(cmd);
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
	HAL_UART_Transmit(&huart3, msg, len, 10);
	HAL_UART_Transmit(&huart3, "\n\r", 2, 10);
}

void sendFloatResponse(float num)
{
	char sendFloatResponse_msg[10];
	int  num_INT=0, len;

	num_INT=(int)num;
	sprintf(sendFloatResponse_msg,"%.2f\n",num);
	len=strlen(sendFloatResponse_msg);
	HAL_UART_Transmit(&huart3, &sendFloatResponse_msg, len, 10);

}

void sendFloatResponse1(float temp1_ref, float temp1,  float u_k1, float temp2_ref, float temp2,  float u_k2)
{
	char sendFloatResponse_msg[40];
	int  len;

	sprintf(sendFloatResponse_msg,"%.2f %.2f %.2f %.2f %.2f %.2f\n",temp1_ref, temp1,  u_k1, temp2_ref, temp2,  u_k2);
	len=strlen(sendFloatResponse_msg);
	HAL_UART_Transmit(&huart3, &sendFloatResponse_msg, len, 10);

}

void sendFloatResponse2(float temp1_ref, float open_loop_enable, float Kp1, float Ti1, float Td1, float u_k1, float temp2_ref, float Kp2, float Ti2, float Td2, float u_k2,
		float reg1, float k_pz1, float a_pz1, float b_pz1, float ref2, float k_pz2, float a_pz2, float b_pz2)
{
	char sendFloatResponse_msg[120];
	int  len;

	sprintf(sendFloatResponse_msg,"DS %.1f %.0f %.1f %.3f %.3f %.0f %.1f %.2f %.3f %.3f %.0f %.0f %.1f %.4f %.4f %.0f %.1f %.4f %.4f\n",
			temp1_ref, open_loop_enable, Kp1, Ti1, Td1, u_k1, temp2_ref, Kp2, Ti2, Td2, u_k2, reg1, k_pz1, a_pz1, b_pz1, ref2, k_pz2, a_pz2, b_pz2);
	len=strlen(sendFloatResponse_msg);
	HAL_UART_Transmit(&huart3, &sendFloatResponse_msg, len, 10);

}

void sendBinaryResponse(float num)
{
	char sendBinaryResponse_msg[4];
	memcpy(sendBinaryResponse_msg, (unsigned char*) (&num), 4);
	HAL_UART_Transmit(&huart3, &sendBinaryResponse_msg, 4, 10);
	HAL_UART_Transmit(&huart3, "\n\r", 2, 10);

}

/* Read temperature sensor*/
float readTemperature(int pin)
{


	if(pin == 1)
	{

	    ADC_Select_CH3();
		for(int i=0;i<10;i++) //Average filter
		{
           HAL_ADC_Start(&hadc1);
	 	   HAL_ADC_PollForConversion(&hadc1, 100);
	 	   ADC_value[0] += HAL_ADC_GetValue(&hadc1);
	 	   HAL_ADC_Stop(&hadc1);
		}
        temp[0]=ADC_value[0]/10.0;
	 	ADC_value[0]=0;

	 	//return temp[0]=(temp[0] * 3.3/4095 - 0.5)/0.01 ; //TMP36 has a 0.5V offset, scale factor 10mV/C
        return temp[0]=(temp[0] * 3.3/4095 - 0.424)/0.00625 ; //LM60 has a 0.424V offset, scale factor 6.25mV/C

	}else if(pin== 2)
	{
		ADC_Select_CH13();
		for(int i=0;i<10;i++) //Average filter
		{
           HAL_ADC_Start(&hadc1);
	 	   HAL_ADC_PollForConversion(&hadc1, 100);
	 	   ADC_value[1] += HAL_ADC_GetValue(&hadc1);
	 	   HAL_ADC_Stop(&hadc1);
		}
	 	temp[1]=ADC_value[1]/10.0;
	 	ADC_value[1]=0;
	 	//return temp[1]=(temp[1] * 3.3/4095 - 0.5)/0.01 ; //TMP36 has a 0.5V offset, scale factor 10mV/C
        return temp[1]=(temp[1] * 3.3/4095 - 0.424)/0.00625 ; //LM60 has a 0.424V offset, scale factor 6.25mV/C
	}

}


/* Heater 1 function */
void setHeater1(float qval)
{
	qval=2.125*qval; /*Scaling factor 0-120% (Simulink) to 0-255 PWM */
	if(qval>255) /*MAX limiter*/
	{
		Q1set_PWM=255;
		Q1=255;
	}else if(qval<0)/*MIN limiter*/
	{
		Q1set_PWM=0;
		Q1=0;
	}else
	{
		Q1set_PWM=(int)qval;
		Q1=qval;
	}

}
/* Heater 2 function */
void setHeater2(float qval)
{

	if(qval>255) /*MAX limiter*/
	{
		Q2set_PWM=255;
		Q2=255;
	}else if(qval<0)/*MIN limiter*/
	{
		Q2set_PWM=0;
		Q2=0;

	}else
	{
		Q2set_PWM=(int)qval;
		Q2=qval;
	}

}

/* Heater 2 function */
uint8_t temp_alarm=0;
void checkAlarm(void)
{

	if(readTemperature(pinT1)>70)//overheating
	{
		setHeater1(0);
		LD3(1);
	}
	else if(readTemperature(pinT2)>70) //overheating
	{
		 setHeater2(0);
		 LD3(1);
	}
    else if(readTemperature(pinT1)>limT1 || readTemperature(pinT2)>limT2)
	{

		Alarm_LED(1);
		temp_alarm=1;

	}else if( (readTemperature(pinT1)<limT1-3 && readTemperature(pinT2)<limT2-3) && temp_alarm==1)
	{
		//HAL_GPIO_WritePin(TCLabLED_GPIO_Port, TCLabLED_Pin, 0);
		Alarm_LED(0);
		LD3(0);
		temp_alarm=0;
	}else if(temp_alarm==0)
	{

		Alarm_LED(0);
	}


}
