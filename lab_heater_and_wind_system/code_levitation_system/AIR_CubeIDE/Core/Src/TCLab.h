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


    /*Macros*/
	#define min(a,b) (((a)<(b))?(a):(b))
	#define max(a,b) (((a)>(b))?(a):(b))
    #define Alarm_LED(led)   led==1 ? HAL_GPIO_WritePin(TCLabLED_GPIO_Port, TCLabLED_Pin, 1): HAL_GPIO_WritePin(TCLabLED_GPIO_Port, TCLabLED_Pin, 0)


	/*Variables*/

	/*Functions*/
	void Serial_read_main_Vetrovnik();
	void ReceiveData();
	void ParseData();
	void CleanData();
	void dispatchCommand(void);
	void sendResponse(char msg[]);
	void sendFloatResponse(float num);
#endif /* SRC_TCLAB_H_ */
