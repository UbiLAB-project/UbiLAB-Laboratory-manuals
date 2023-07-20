#include "main.h"
#include "stm32f7xx_hal.h"
#include "VL53L0X.h"
#include "math.h"

void SMC_Differenciator();

uint16_t PWM_FAN=8000;
extern VL53L0X sensor1;
uint16_t dist;
int controller_on=0;
int controller_on_flag=0;
float Ts=0.0498; //Controller sampling time
float ABS(float sigma);
float sgn(float sigma);


/*Filter states*/
float pole_ref_filter=-0.3; //use -5
float xd=0,xdd=0, xd_k_1=0, xdd_k_1=0;
float ref_f=0,ref_angle_k_1=0,ref_in_k=0,ref_in_k_1=0;
float ref=350;
/*******************************************************************************
* Function Name  : Real time functions
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float pole_filter=-10;
float pole_filter_velocity=-10;
float Tsf=0.0264; //Sampling time for filter
float dist_f=0,dist_f_k=0,dist_f_k_1=0;
uint16_t dist_in_k_1=0, dist_old=0, dist_real=0;
float velocity_diff=0,velocity_f=0, velocity_f_k_1=0,velocity_k_1=0;

void Real_time()
{
   
  //Read ToF
	
		 
		  dist_real=readRangeContinuousMillimeters(&sensor1); //Read time 1.3ms
	    
	     if( dist_real>8000) //I2c problem!!
			 {
			   dist=dist_old;
			 }else{
			 
				 dist=dist_real;
			   dist_old=dist;
				 
			 }
	
	      //Filter sampling time
	     dist_f_k = exp((double) pole_filter * Tsf ) *dist_f_k_1 + (1-exp((double) pole_filter * Tsf ))*(double)dist_in_k_1;
	   
			 
			 //HIGH PASS FILTER - DIFFERENTIATOR
			 velocity_diff = (dist_f_k - dist_f_k_1)/Tsf;
			 velocity_f= exp((double) pole_filter_velocity * Tsf ) *velocity_f_k_1 + (1-exp((double) pole_filter_velocity * Tsf ))*(double)velocity_k_1;
			 
			 
       SMC_Differenciator();	
			
			  //Time shift for distance 
       dist_f_k_1= dist_f_k;   
       dist_in_k_1 = dist;
	     dist_f=dist_f_k;

			 
			 //Time shift for velocity
			 velocity_f_k_1=velocity_f;
			 velocity_k_1  = velocity_diff;
			 //LD1(0);
      //HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0); //Toggle green


}




/*******************************************************************************
* Function Name  : Reference input filter
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


void reference_filter()
{

// Input
ref_in_k = ref;
//Filter	
ref_f = exp((double) pole_ref_filter * Ts ) *ref_angle_k_1 + (1-exp((double) pole_ref_filter * Ts ))*ref_in_k_1;

	//First derivatieve
	xd= (ref_f - ref_angle_k_1)/Ts;
	
	//Second derivative
	xdd= (xd - xd_k_1)/Ts;	
	
	//time shift -convolution
	ref_angle_k_1 = ref_f;	
	ref_in_k_1 = ref_in_k;
	
	xd_k_1 = xd;
	xdd_k_1 = xdd;
	
}



/*******************************************************************************
* Function Name  : Sliding mode controller Yuri Shtessel-SMC book 2015  pp159
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float esti_error=0,estimated_high=0,estimated_high_k_1,z1_k=0,z1_k_1=0,dz1=0;
float velocity_estimated=0,velocity=0;
float lam1=25,lam2=15;

void SMC_Differenciator()
{



    esti_error=  estimated_high - dist_f;
	
	 
    dz1=  -1.0 * lam2 * sgn( esti_error);
	  z1_k = z1_k_1  + Ts * dz1; //Integration
    z1_k_1 = z1_k;
    
	
	
    velocity_estimated = -1.0 * lam1  * sqrt(ABS(esti_error)) * sgn(esti_error) + z1_k;
	
	  // Integration
	   estimated_high = estimated_high_k_1 + Ts * velocity_estimated ;
     estimated_high_k_1 = estimated_high;
	
	    velocity=velocity_estimated;
}


/*******************************************************************************
* Function Name  : Signum function
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float sgn(float sigma)
{

     if(sigma>0)
		 {
		    return 1.0;
		 }else if(sigma<0)
     {
		    return -1.0;
		 }else
		 {
		   return 0.0;
		 }
		 
}


/*******************************************************************************
* Function Name  : ABS function
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float ABS(float sigma)
{

     if(sigma>0)
		 {
		    return sigma;
		 }else if(sigma<0)
     {
		    return -1.0 * sigma;
		 }else
       return 0;
}
