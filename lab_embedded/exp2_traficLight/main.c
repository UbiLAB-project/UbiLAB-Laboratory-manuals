#include<stm32f10x_gpio.h>
#include<stm32f10x_rcc.h>
GPIO_InitTypeDef GPIO_InitStruct;
int red_led=0;
int green_led=0;
int yellow_led=0;
void delay_ms(int ms)
{
	RCC -> APB1ENR |= 0x1;
	TIM2 -> PSC = 72-1; 
	TIM2 -> ARR = 1000-1; 
	TIM2 -> CNT = 0;
	TIM2 -> EGR |= 0x1; 
	TIM2 -> SR &= ~(0x1); 
	TIM2 -> CR1 |= 0x1; 
	while(ms > 0) 
	{
		while((TIM2 -> SR & 0x1) == 0); 
			TIM2 -> SR &= ~(0x1); 
			ms--; 
		}
	TIM2 -> CR1 &= ~(0x1); 
	RCC -> APB1ENR &= ~(0x1 );
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

    while(1)
    {
    	green_led=0;
    	red_led=1;
    	GPIO_WriteBit(GPIOA,GPIO_Pin_4,red_led);
    	GPIO_WriteBit(GPIOA,GPIO_Pin_6,yellow_led);
    	GPIO_WriteBit(GPIOA,GPIO_Pin_7,green_led);
    	delay_ms(5000);
    	red_led=0;
    	yellow_led=1;
    	GPIO_WriteBit(GPIOA,GPIO_Pin_4,red_led);
    	GPIO_WriteBit(GPIOA,GPIO_Pin_6,yellow_led);
    	delay_ms(5000);
    	green_led=1;
    	yellow_led=0;
    	GPIO_WriteBit(GPIOA,GPIO_Pin_6,yellow_led);
    	GPIO_WriteBit(GPIOA,GPIO_Pin_7,green_led);
    	delay_ms(5000);

    }
}
