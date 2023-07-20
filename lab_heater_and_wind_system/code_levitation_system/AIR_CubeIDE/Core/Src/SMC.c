#include "main.h"
#include "stm32f7xx_hal.h"
#include "VL53L0X.h"
#include "math.h"


extern float ABS(float sigma);
extern float sgn(float sigma);


extern float ref_f,dist_f;
extern float ref;
extern float velocity; 
extern float Ts;
extern float u_k;
extern uint16_t PWM_FAN_offst,PWM_FAN;
extern float velocity_f;
extern int controller_on;
/*******************************************************************************
* Function Name  : Sliding mode controller -SUPER TWISTED CONTROLLER
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float sigma=0;
float c=9.45;
float k1=3.99,k2=14.2;
float sigma_ABS=0,sigma_sqrt;
float dref=0,ref_f_k_1=0;
float w_k=0,w_k_1=0,dw;

void SUPER_TWISTED_CONTROLLER() //SUPER TWISTED CONTROLLER
{

    //Reference filter
	     reference_filter();

      dref= (ref_f-ref_f_k_1)/Ts;
	    ref_f_k_1=ref_f;
	
	    //Sliding variable 
	   sigma=(1*(dref - velocity_f) + c *(ref_f-dist_f)); //Convert to the meter
	
	   //Integration
	  dw =  k2 * sgn(sigma); 
      w_k = w_k_1 + Ts * dw;  //Integration
      w_k_1 = w_k;

	
		sigma_ABS = ABS(sigma);
	  sigma_sqrt=sqrt(sigma_ABS);
	
	  u_k =  k1 *  sigma_sqrt * sgn(sigma) + w_k;
	
 //Output 3200 to 6400 nominal 3880
	    PWM_FAN=PWM_FAN_offst + (int)u_k;
  		TIM2->CCR1=PWM_FAN;  //ON CHANNEL 1 max 20000 //PA0
			
			
		 if (dist_f>940)
     {
		  PWM_FAN=PWM_FAN_offst;
  		TIM2->CCR1=PWM_FAN;
			controller_on=0; //Turn off controller
			ref=200;
		 }	
     
}
