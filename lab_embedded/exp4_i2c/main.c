#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_tim.h>

#define SHT20_I2C_ADDR                        0x80
#define SHT20_NOHOLD_TEMP_REG_ADDR            0xF3
#define SHT20_NOHOLD_HUMDTY_REG_ADDR          0xF5
#define SHT20_HOLD_TEMP_REG_ADDR            0xF3
#define SHT20_HOLD_HUMDTY_REG_ADDR          0xF5
#define ERROR_I2C_TIMEOUT                     998
#define ERROR_BAD_CRC                         999
#define SHIFTED_DIVISOR                       0x988000
I2C_InitTypeDef I2C_InitStruct;
GPIO_InitTypeDef GPIO_I2C_InitStruct;

int temp;
int humidity;
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
			ms--; 
		}
	TIM2 -> CR1 &= ~(0x1); 
	RCC -> APB1ENR &= ~(0x1 );
}
void init_I2C1(void){

	// enable APB1 peripheral clock for I2C1
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
			// enable clock for SCL and SDA pins
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

			/* setup SCL and SDA pins
			 * You can connect I2C1 to two different
			 * pairs of pins:
			 * 1. SCL on PB6 and SDA on PB7
			 * 2. SCL on PB8 and SDA on PB9
			 */
			GPIO_I2C_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

				// we are going to use PB6 and PB7
			GPIO_I2C_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;			// set pins to alternate function
			GPIO_I2C_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// set GPIO speed
				//GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
				//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// enable pull up resistors
			GPIO_Init(GPIOB, &GPIO_I2C_InitStruct);					// init GPIOB
			GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

			// Connect I2C1 pins to AF

			 I2C_InitStruct.I2C_ClockSpeed=100000;
			 I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;
			 I2C_InitStruct.I2C_DutyCycle=I2C_DutyCycle_2;
			 I2C_InitStruct.I2C_Ack=I2C_Ack_Enable;
			 I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;  //size of the address
			 I2C_InitStruct.I2C_OwnAddress1=0x00;  //address of the Microcontroller

			 I2C_Init( I2C1,&I2C_InitStruct);   //init I2C
			 I2C_Cmd(I2C1, ENABLE); 		//Enables or disables the specified I2C peripheral.

}
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
		// Send I2C1 START condition
			while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BUSY));
		 	I2C_GenerateSTART(I2C1, ENABLE);
		 	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		 	// Send slave Address for write
		 	I2C_Send7bitAddress(I2Cx, address, direction);
		 	if(direction == I2C_Direction_Transmitter){
		 			 		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		 			 	}
		 			 	else if(direction == I2C_Direction_Receiver){
		 			 		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		 			 	}

}
void I2C_restart(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
		//
		 	I2C_GenerateSTART(I2C1, ENABLE);
		 	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		 	// Send slave Address for write
		 	I2C_Send7bitAddress(I2Cx, address, direction);
		 	if(direction == I2C_Direction_Transmitter){
		 			 		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		 			 	}
		 			 	else if(direction == I2C_Direction_Receiver){
		 			 		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		 			 	}

}

void I2C_write(I2C_TypeDef* I2Cx, uint8_t data)
		 	{

		 		I2C_SendData(I2C1, data);
		 		// wait for I2C1 EV8_2 --> byte has been transmitted
		 		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		 	}

		 	/* This function reads one byte from the slave device
		 	 * and acknowledges the byte (requests another byte)
		 	 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
		 		// enable acknowledge of recieved data
		 		I2C_AcknowledgeConfig(I2C1, ENABLE);
		 		// wait until one byte has been received
		 		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );
		 		// read data from I2C data register and return data byte
		 		uint8_t data = I2C_ReceiveData(I2C1);
		 		return data;
		 	}

		 	/* This function reads one byte from the slave device
		 	 * and doesn't acknowledge the recieved data
		 	 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
		 		// disabe acknowledge of received data
		 		// nack also generates stop condition after last byte received
		 		// see reference manual for more info
		 		I2C_AcknowledgeConfig(I2Cx, DISABLE);

		 		// wait until one byte has been received
		 		while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
		 		// read data from I2C data register and return data byte
		 		uint8_t data = I2C_ReceiveData(I2Cx);
		 		return data;
		 	}

void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2C1, ENABLE);
}
uint16_t SHT20_readValue(I2C_TypeDef* I2Cx, uint8_t address,uint8_t reg){
	uint8_t lsb,msb;
	delay_ms(80);
	I2C_start(I2C1,address,I2C_Direction_Transmitter);
	delay_ms(80);
	I2C_write(I2C1,reg);
	delay_ms(80);
	I2C_restart(I2C1,address,I2C_Direction_Receiver);
	delay_ms(80);
	msb=I2C_read_ack(I2C1);
	delay_ms(80);
	lsb=I2C_read_ack(I2C1);
	delay_ms(80);

	u8 checksum=I2C_read_ack(I2Cx);
	delay_ms(80);
	I2C_stop(I2Cx);
	delay_ms(80);

	uint16_t raw_value=((uint16_t) msb<<8)|(uint16_t)(lsb);
//	if (SHT20_checkCRC((u16)raw_value, (u8)checksum) != 0)
//	        return (ERROR_BAD_CRC);
	return (raw_value & 0xfffc);


}
float SHT20_readHumidity(I2C_TypeDef* I2Cx, uint8_t address){
	float tempRH;
	float rh;
	uint16_t rawHumidity=SHT20_readValue(I2Cx,address,SHT20_HOLD_HUMDTY_REG_ADDR);

//    if (rawHumidity == ERROR_I2C_TIMEOUT || rawHumidity == ERROR_BAD_CRC)
//        return (rawHumidity);
    tempRH=rawHumidity*125.0/65535.0;
    tempRH=tempRH-6.0;
    return tempRH;

}

float SHT20_readTemp(I2C_TypeDef* I2Cx, uint8_t address){
	float temp;
	float rh;
	uint16_t rawTemp=SHT20_readValue(I2Cx,address,SHT20_NOHOLD_TEMP_REG_ADDR);
//
//    if (rawTemp == ERROR_I2C_TIMEOUT || rawTemp == ERROR_BAD_CRC)
//        return (rawTemp);
    temp=rawTemp*(175.72/65536.0);
    temp=temp-46.85;
    return temp;
}

int main(void)
{
	init_I2C1();

    while(1)
    {

    	temp=SHT20_readTemp(I2C1,SHT20_I2C_ADDR);
    	delay_ms(1000);
    	humidity=SHT20_readHumidity(I2C1,SHT20_I2C_ADDR);

    }
}
