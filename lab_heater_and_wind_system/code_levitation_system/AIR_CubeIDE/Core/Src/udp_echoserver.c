/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Server/Src/udp_echoserver.c
  * @author  MCD Application Team
  * @brief   UDP echo server
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/err.h"
#include <string.h>
#include <stdio.h>
#include "udp_echoserver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    17   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    7   /* define the UDP remote connection port */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void PWM_from_UDP(char *p_buff,int len);

/* Private functions ---------------------------------------------------------*/
extern float dist_f;
extern UART_HandleTypeDef huart3;
struct udp_pcb *broadcast_pcb; //Device structure
struct udp_pcb *upcb;          //Device structure
u8_t   data_x[100];
 
#define out_buf_size 50
int out_buf_i = 0;
int buf[out_buf_size];
uint8_t toggle_receive_flag=0;

extern ip4_addr_t ipaddr;
extern ip4_addr_t netmask;
extern ip4_addr_t gw;
extern ip4_addr_t forward_ip; //Destination address
extern int controller_on;
extern uint16_t PWM_FAN;
/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{

	
   err_t err;

	
   /* Create a new UDP control block  */
   upcb = udp_new();
   broadcast_pcb = udp_new(); // Send data without response
	
	
	
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
       err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
		   //err = udp_bind(upcb, &ipaddr, UDP_SERVER_PORT); //MY
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
      }
      else
      {
        udp_remove(upcb);
      }
   }
	 
	 
	 
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */


//struct pbuf *c;
char buff_andy[10];
char *p_buff;
int len_buff_andy=0,num_UDP=0;

void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

  /* Connect to the remote client */
  udp_connect(upcb, addr, UDP_CLIENT_PORT);
  
	forward_ip=*addr; // In case of DHCP (save the IP ADDRES from client device)
	
	
	//My code
 
	  
	  memcpy(buff_andy, p->payload, p->len); //Copy received buffer to buff_andy 
	  len_buff_andy=strlen(buff_andy);
	  
	  if(memcmp("D",buff_andy,1)==0) //Request to send Tof Distance
		{
		   sprintf(buff_andy, "%.2f",dist_f); 
			
	  }else if(memcmp("P",buff_andy,1)==0) //Request to send Tof Distance		
  	{
	    //GET PWM value
			p_buff=buff_andy; //Set pointer to the buffer
			PWM_from_UDP(p_buff,len_buff_andy);
			
			toggle_receive_flag = !toggle_receive_flag; //alternate value between 0 and 1
			
	  }
		 else if(memcmp("S",buff_andy,1)==0) //Start controller	
    {
		 //START
			controller_on=1;
		
		}
		else if(memcmp("E",buff_andy,1)==0) //stop controller	
		{
		
		 //Stop Controller
			
			controller_on=0;
			TIM2->CCR1=3000; //STOP PWM
		}
		else
		{
		  sprintf(buff_andy, "NaN");  //Send back 'NaN'
		}
	  
	  //num_UDP=atoi(buff_andy);
		//num_UDP=num_UDP+1;
		//sprintf(buff_andy,"%d",num_UDP);
    //sprintf(buff_andy,"%d",num_UDP);
    
	  //sprintf(buff_andy, "%.2f",dist_f);
    
		len_buff_andy=strlen(buff_andy);
	  p->len= len_buff_andy;
    p->tot_len= len_buff_andy; 
    memcpy(p->payload, buff_andy, len_buff_andy); //Copy buff_andy to lwIP stuck buffer
	 
	 
	 
	 
  /* Tell the client that we have accepted it */
  udp_send(upcb, p);
  //udp_send(upcb, p);
	
  /* free the UDP connection, so we can accept new clients */
  udp_disconnect(upcb);
	
  /* Free the p buffer */
  pbuf_free(p);
		
		//Clear Buffer
	  for(int i=0;i<len_buff_andy;i++)
		{ 
		   buff_andy[i]=' ';
		}	
   
}

/*******************************************************************************
* Function Name  : UDP SEND function 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

//void udp_send_custum() {
//
//  struct pbuf *pp;
//
//
//	//Data to send
//  sprintf((char*)data_x, "D%.2f",dist_f);
//
//  /* allocate pbuf from pool*/
//  pp = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data_x), PBUF_POOL);
//
//  if (pp != NULL)
//  {
//    /* copy data to pbuf */
//    pbuf_take(pp, (char*)data_x, strlen((char*)data_x));
//
//    /* send udp data */
//    udp_sendto(broadcast_pcb, pp, &forward_ip, UDP_CLIENT_PORT); //TARGET IP and client PORT
//
//		/* free pbuf */
//    pbuf_free(pp);
//
//		/*Send on serial*/
//	 HAL_UART_Transmit_IT(&huart3,data_x,strlen((char*)data_x)); //Vector data originate from TIM5 interrupt
//
//  }
//}



/*******************************************************************************
* Function Name  : get data from UDP received buffer
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
char ascii_pwm_buff[5];
uint8_t ascii_pwm_cnt=0;

void PWM_from_UDP(char *p_buff,int len)
{
	// Find ASCII values for numbers
    for(int i=0;i<len;i++)
	  {
		      
		   if(p_buff[i]>47 && p_buff[i]<59 )
			 {
			 
			   ascii_pwm_buff[ascii_pwm_cnt]=p_buff[i];
				 ascii_pwm_cnt++;
				 
			 }
		
		
		}

 //Convert ASCII numbert to int		
     if( ascii_pwm_cnt!=0) 
	   {
			 PWM_FAN=atoi(ascii_pwm_buff);
		     
			    //Limiter
			     if  (PWM_FAN<3000)
					 {
					   PWM_FAN=3000;
					 
					 }
					 else if(PWM_FAN>4300)
					 {
					   PWM_FAN=4300;
					 }						
		 
		 }
		
	//Clear buffer	 
		for(int i=0;i<ascii_pwm_cnt;i++)
		{
		   ascii_pwm_buff[i]=0;
		}
		
	ascii_pwm_cnt=0;	
		
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
