/*
 * TCLab.h
 *
 *  Created on: Oct 1, 2021
 *      Author: Andy
 */

#ifndef SRC_TCLAB_H_
#define SRC_TCLAB_H_

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif


  /*TCLab dependencies*/
	#define false  0
	#define true   1
    #define pinT1  1
	#define pinT2  2

	#define Q1set_PWM TIM1->CCR3   //Heater 1 PWM (0-255)
	#define Q2set_PWM TIM1->CCR2   //Heater 2 PWM (0-255)


    /*Macros*/
	#define min(a,b) (((a)<(b))?(a):(b))
	#define max(a,b) (((a)>(b))?(a):(b))
    #define Alarm_LED(led)   led==1 ? HAL_GPIO_WritePin(TCLabLED_GPIO_Port, TCLabLED_Pin, 1): HAL_GPIO_WritePin(TCLabLED_GPIO_Port, TCLabLED_Pin, 0)


	/*Variables*/
	int ADC_value[2];
	float temp[2];


	/*Functions*/
	void Serial_read_main_TCLab();
	void ReceiveData();
	void ParseData();
	void CleanData();
	void sendResponse(char msg[]);
	float readTemperature(int pin);
	void setHeater1(float qval);
	void setHeater2(float qval);
	void dispatchCommand(void);
	void sendFloatResponse(float num);
	void sendFloatResponse1(float temp1_ref, float temp1,  float u_k1, float temp2_ref, float temp2,  float u_k2);
	void sendFloatResponse2(float temp1_ref, float open_loop_enable, float Kp1, float Ti1, float Td1, float u_k1, float temp2_ref, float Kp2, float Ti2, float Td2, float u_k2,
			float reg1, float k_pz1, float a_pz1, float b_pz1, float ref2, float k_pz2, float a_pz2, float b_pz2);
	void sendBinaryResponse(float num);


#endif /* SRC_TCLAB_H_ */
