#include<stm32f10x_gpio.h>
#include<stm32f10x_rcc.h>
GPIO_InitTypeDef GPIO_InitStruct;
int led=0;

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);





    while(1)
    {
    	GPIO_WriteBit(GPIOA,GPIO_Pin_4,led);
    	for (int i=0; i<900000;i++);
    	led=~led;
    }
}
