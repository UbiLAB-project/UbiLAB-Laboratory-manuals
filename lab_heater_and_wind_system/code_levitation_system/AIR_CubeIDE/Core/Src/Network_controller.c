#include "main.h"
#include "stm32f7xx_hal.h"
#include "VL53L0X.h"
#include "math.h"
extern void udp_send_custum(); //Send distnace over Internet 

extern float ABS(float sigma);
extern float sgn(float sigma);
extern uint16_t PWM_FAN;
extern uint8_t toggle_receive_flag;
uint8_t toggle_receive_flag_old;
uint16_t PWM_FAN_old=3000;
uint8_t client_response=0;

/*******************************************************************************
* Function Name  : Controller real time function - TIM5 @50ms
* Description    :  all from: void TIM5_IRQHandler(void), file: stm32f7xx_it.c
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Network_controller()
{

	  //Receive flag 
    toggle_receive_flag_old=toggle_receive_flag;
	  PWM_FAN_old=PWM_FAN;
    
	  udp_send_custum(); //Send data to the network


    //Wait for the CLIENT response, STA from the cloud (PC)
	  while(TIM5->CNT<900 && (toggle_receive_flag_old==toggle_receive_flag)) //MAX value 950=50ms
	  {
		 
		}
	
		
	 //Update controller
	 if (toggle_receive_flag_old!=toggle_receive_flag) //Received value from the PC
	 {
		 TIM2->CCR1=PWM_FAN;
		 PWM_FAN_old=PWM_FAN;
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1); //GReen ON
	 }else  //No response from the  PC
	 {
	   TIM2->CCR1=PWM_FAN_old;
	   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1); //GReen OFF
	 }







}