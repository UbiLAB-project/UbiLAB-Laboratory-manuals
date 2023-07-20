/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "TCLab.h"

float temp1,temp2;         //Temperature value T1 and T2
extern float temp1_ref;    //Temperature reference for Heater Q1
extern float temp2_ref;    //Temperature reference for Heater Q2
extern float Kp1; Ti1; Td1;
extern float Kp2; Ti2; Td2;
extern float open_loop_enable1;
extern float open_loop_enable2;
extern float u_k1; u_k2;
extern float k_pz1, a_pz1, b_pz1;
extern float k_pz2, a_pz2, b_pz2;
extern float reg1;
extern float reg2;
// PI regulator
float Ts=1.0; //Sampling time
float e_k=0,e_k1_1=0;
float I_k=0,I_k2_1=0;
float u_k1_1=0.f, u_k2_1=0.f;

float P_1=0.0f; I1=0.0f; D1=0.0f, out;
float P_2=0.0f; I2=0.0f; D2=0.0f;

float error1_k=0.0f; error1_k_1;
float error2_k=0.0f; error2_k_1;






//Regulator po polinomski sintezi
/*
float Ts=1.0;
float y_f[2] = {0.0, 0.0};
//float temp1_k_1 = 0;
float ref[2] = {25.0, 25.0};
float e_r[2] = {0.0, 0.0};
float y_r[2] = {0.0, 0.0};
float referenca = 30.0;
*/
void Controller_real_time()
{
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
   //Read temperature
	temp1=readTemperature(pinT1);
	temp2=readTemperature(pinT2);

	sendFloatResponse1(temp1_ref, temp1,  u_k1, temp2_ref, temp2,  u_k2);


	//PID regulator1

	if(open_loop_enable1==0)
	{

		if(reg1==1)
		{
		error1_k= temp1_ref - temp1;

		//P-proportional part
		  P_1 = Kp1 * error1_k;

		// I-integral part
		  I1 =  I1 + 1/(Ti1) *error1_k * Ts;

	   // D-derivative part
		  D1 =  Td1/Ts * (error1_k - error1_k_1);
		  error1_k_1=error1_k;

		 //Output
		u_k1 = P_1  + I1 + D1;
		}
		else if(reg1==2)
		{
		    //Prehitevalni kompenzator
			error1_k=temp1_ref - temp1;
			u_k1=a_pz1 * u_k1_1 + k_pz1*(error1_k - b_pz1*error1_k_1);
			u_k1_1=u_k1;
			error1_k_1=error1_k;
		}
	}

	if(u_k1>255)
		u_k1=255;
	else if(u_k1<0)
		u_k1=0;

	if(open_loop_enable2==0)
	{

		if(reg2==1)
			{
			error2_k= temp2_ref - temp2;

			//P-proportional part
			  P_2 = Kp2 * error2_k;

			// I-integral part
			  I2 =  I2 + 1/(Ti2) *error2_k * Ts;

		   // D-derivative part
			  D2 =  Td2/Ts * (error2_k - error2_k_1);
			  error2_k_1=error2_k;

			 //Output
			u_k2 = P_2  + I2 + D2;
			}
		else if(reg2==2)
		{
			//Prehitevalni kompenzator
			error2_k=temp2_ref - temp2;
			u_k2=a_pz2 * u_k2_1 + k_pz2*(error2_k - b_pz2*error2_k_1);
			u_k2_1=u_k2;
			error2_k_1=error2_k;
		}
	}

	if(u_k2>255)
		u_k2=255;
	else if(u_k2<0)
		u_k2=0;

/*
	//Regulator po polinomski sintezi
	ref[1]=referenca;

	y_f[1]=(2.52759719*y_f[0]+1.97353326*ref[1]-1.957*ref[0])/2.54397496;
	y_f[0]=y_f[1];
	ref[0]=ref[1];
	//temp1_k_1=temp1;

	e_r[1]=y_f[1]-temp1;

	y_r[1]=y_r[0]+2.54397496*e_r[1]-2.52759719*e_r[0];
	y_r[0]=y_r[1];
	e_r[0]=e_r[1];

	if(y_r[1]>255)
	{
		y_r[1]=255;
	}
	else if(y_r[1]<0)
	{
		y_r[1]=0;
	}
*/
   //Set output values
	setHeater1(u_k1); //Span [0-255]   y_r[1]
	setHeater2(u_k2); //Span [0-255]

}















































































































/*
float Kp=3.7,Ki=0.021,Kd=0.7;
     e_k=temp1_ref - temp1;

		 //P-part
		 	 P= Kp*e_k;

		 //I-Part
			 I_k=I_k_1 + Ki * Ts * e_k;
			 I_k_1=I_k;
			 I=I_k;

         //D-Part
			 D=Kd * (e_k - e_k_1)/Ts;
             e_k_1=e_k;

		 out=P+I+D;

*/
