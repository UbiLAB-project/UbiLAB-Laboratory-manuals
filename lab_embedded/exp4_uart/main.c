#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_adc.h>
GPIO_InitTypeDef GPIO_InitStruct_USART;
USART_InitTypeDef USART_InitSTruct;

GPIO_InitTypeDef GPIO_ADC_InitStruct;
ADC_InitTypeDef ADC_InitStruct;
static uint8_t value=55;
void adc(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_ADC_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_ADC_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_ADC_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_ADC_InitStruct);					
	//GPIO_PinRemapConfig(GPIO_Remap_ADC1_ETRGREG, ENABLE);
	ADC_DeInit(ADC1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);

	ADC_InitStruct.ADC_Mode==ADC_Mode_Independent;
	ADC_InitStruct.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;
	ADC_InitStruct.ADC_NbrOfChannel=1;
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_Init(ADC1,&ADC_InitStruct);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_1Cycles5);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration( ADC1);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



void init_usart1(GPIO_InitTypeDef GPIO_InitStruct_USART,USART_InitTypeDef USART_InitSTruct){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct_USART.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct_USART.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStruct_USART.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct_USART);
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);

	USART_InitSTruct.USART_BaudRate=9600;
	USART_InitSTruct.USART_WordLength=USART_WordLength_8b;
	USART_InitSTruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitSTruct.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	USART_InitSTruct.USART_StopBits=USART_StopBits_1;
	USART_InitSTruct.USART_Parity=USART_Parity_No;

	USART_Init(USART1,&USART_InitSTruct);
	USART_Cmd(USART1,ENABLE);

}
void init_usart2(GPIO_InitTypeDef GPIO_InitStruct_USART2,USART_InitTypeDef USART_InitSTruct2){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct_USART2.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct_USART2.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStruct_USART2.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct_USART2);
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);

	USART_InitSTruct2.USART_BaudRate=115200;
	USART_InitSTruct2.USART_WordLength=USART_WordLength_8b;
	USART_InitSTruct2.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitSTruct2.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_InitSTruct2.USART_StopBits=USART_StopBits_1;
	USART_InitSTruct2.USART_Parity=USART_Parity_No;
	USART_Init(USART2,&USART_InitSTruct2);
	USART_Cmd(USART2,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	//NVIC_EnableIRQ(USART2_IRQn);

}


void send_characters(uint8_t *array,USART_TypeDef* USARTx){
	int i=0;
	int flag=0;
	while(array[i]!=0x00){
		while((!(USARTx->SR&0x00000040)));

		u8 ch=array[i];

		USART_SendData(USARTx,(uint16_t)ch);
		i++;

	} flag=1;
}
void send_number(uint8_t num,USART_TypeDef* USARTx){
	u8 rem;

		while((!(USARTx->SR&0x00000040)));
		rem=num%10;
		num=num/10;
		USART_SendData(USARTx,(uint16_t)(num+'0'));
		while((!(USARTx->SR&0x00000040)));
		USART_SendData(USARTx,(uint16_t)(rem+'0'));




}
char *buf;
int FLAG_MESSAGE_COMPLETED;
//void USART1_IRQHandler(void)
//{
//
//    /* RXNE handler */
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//    {
//
//    	*buf=(char)USART_ReceiveData(USART2);
//    	if((*buf=='\n') || (*buf=='\r') ){
//    		FLAG_MESSAGE_COMPLETED=1;
//
//    	}
//    	buf++;
//
//
//        /* If received 't', toggle LED and transmit 'T' */
//
//
//
//        }
//  }


int main(void)
{

	init_usart2(GPIO_InitStruct_USART, USART_InitSTruct);
	adc();

    while(1)
    {

    	send_characters("The moisture is ",USART2);
    	value=ADC_GetConversionValue(ADC1);
    	send_number(value,USART2);
    	send_characters(" Please, water  the plant\r\n!!!",USART2);
    	for(int i=0;i<500000;i++);

    }
}
