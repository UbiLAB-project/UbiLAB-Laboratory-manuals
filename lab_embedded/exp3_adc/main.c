#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_adc.h>

GPIO_InitTypeDef GPIO_ADC_InitStruct;
ADC_InitTypeDef ADC_InitStruct;
uint16_t value=0;
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_ADC_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_ADC_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_ADC_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_ADC_InitStruct);					// init GPIOB
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


    while(1)
    {
    	uint16_t value=0;
    	value=ADC_GetConversionValue(ADC1);
    	for(int i=0;i<500000;i++);
    }
}
