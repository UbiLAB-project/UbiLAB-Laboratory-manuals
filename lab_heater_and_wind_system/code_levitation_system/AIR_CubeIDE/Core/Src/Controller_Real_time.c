#include "main.h"
#include "stm32f7xx_hal.h"
#include "VL53L0X.h"
#include "math.h"
#include "stdint.h"
#include "string.h"

extern UART_HandleTypeDef huart3; //Communication VCP

void PID_Controller();
extern void SUPER_TWISTED_CONTROLLER();
extern void Network_controller();

extern float xd,xdd;
extern float ref_f,dist_f,velocity_f;
extern float ref;
extern uint16_t PWM_FAN;
extern int controller_on;
extern float Ts; //Controller sampling time
extern uint8_t USART3_busy_flag;


uint16_t PWM_FAN_offst=8750; //wINDsYSTEM 4  37.63g
uint8_t  select=0;

char Buffer_data[64];

float P=0,I=0,D=0,error=0,error_k_1=0,Id=0,u_k=0,gain=1;
//float Kp=0.9, Ti=5, Td=0.1; // Metoda stopnicnega odziva
float Kp=4.2, Ti=3.9, Td=0.7; // Metoda stopnicnega odziva
/*******************************************************************************
* Function Name  : Controller real time function - TIM5 @50ms
* Description    : call from: void TIM5_IRQHandler(void), file: stm32f7xx_it.c
* Input          : dist_f - Floater height
* Output         : None
* Return         : None
*******************************************************************************/

void Controller_Real_time_update()
{
	 /* SYSTEM VARIABLES
	  * Inputs:
	  *      dist_f     =x1     (Height   [mm])
	  *      velocity_f =x2     (Velocity [mm/s])
	  *      ref_f      =xref   (Filtered desired value, with frequency -> 'pole_ref_filter')
	  *      xd         =dxref  (First derivative of Xref)
	  *      xdd        =ddxref (Second derivative of Xref)
	  * Output:
	  *  u_k        = Controller output, goes to  PWM_FAN=(int)u_k
	  *  TIM2->CCR1 = PWM_FAN;  (Controller output PWM-duty [8000-11000])
	  * */


	  if(select==0)
  	   {
		  PID_Controller();

  	   } //Call controller function
	   else if (select==1)
	  {
		   SUPER_TWISTED_CONTROLLER();

	  }else if (select==2)
	  {
		    //Network_controller();  
      }
      else
	  {
    	  select=0;

	  }

	  /*
	  //Send data over USART3
	  if(USART3_busy_flag==0)
	  {
		  sprintf(Buffer_data,"%.2f %.2f %.2f %.2f\n",ref,dist_f,velocity_f,u_k);
		  HAL_UART_Transmit(&huart3, Buffer_data, strlen(Buffer_data), 10);
	  }
	  */
}






/*******************************************************************************
* Function Name  : Controller function
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/



void PID_Controller()
{
	   /* Reference filter  */
	     reference_filter();
	
       error=ref_f-dist_f;

	  	/* PID regulator */
		    /* Proportional part */
			P = Kp * error;

			/* Integral part */
			I =   I + 1/(Ti) * error * Ts; //Integracija

			/* Differential part */
			D =    Td/Ts * (error - error_k_1);
				  error_k_1=error;

	        /* PID-output*/
  			u_k = P  + I  + D;
	  

     /* Controller limiter */
	   if (u_k>130)
		 {
		     u_k=130;
			 
		 }else if(u_k<-130)
		 {
		    u_k=-130;
		 }
		 
		 
	    /* Controller out to PWM  span [8000-11000]*/
	    PWM_FAN=PWM_FAN_offst + (uint16_t) gain * u_k;
  		TIM2->CCR1=PWM_FAN;  //ON CHANNEL 1 //PA0
     


     /* Safety -  */
       if (dist_f>920)
       {
		  PWM_FAN=8000; //Turn off the blower
  		  TIM2->CCR1=8000;
		  I=0;Id=0;error=0;error_k_1=0;
		  controller_on=0; //Turn off controller
		  ref=300;
	  }

}
