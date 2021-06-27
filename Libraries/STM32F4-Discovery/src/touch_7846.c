#include "touch_7846.h"
#include <stdio.h>

Pen_Holder Pen_Point;


unsigned char flag=0;

														
unsigned char SPI_WriteByte ( u8 num ) {
	unsigned char Data = 0;
	while (SPI_I2S_GetFlagStatus ( SPI2, SPI_I2S_FLAG_TXE ) == RESET) {
	}
	SPI_I2S_SendData ( SPI2, num );
	while (SPI_I2S_GetFlagStatus ( SPI2, SPI_I2S_FLAG_RXNE ) == RESET) {
	}
	Data = SPI_I2S_ReceiveData ( SPI2 );
	return Data;
} 	

void SpiDelay ( unsigned int DelayCnt ) {
	unsigned int i;
	for (i = 0; i < DelayCnt; i++) {
	}
}

u16 TPReadX ( void ) {
	u16 x = 0;
	T_CS();
	SpiDelay(10);
	SPI_WriteByte ( 0x90 );
	SpiDelay(10);
	x = SPI_WriteByte ( 0xFF );
	x <<= 8;
	x += SPI_WriteByte ( 0x0 );
	T_DCS();
	x = x >> 4;
	x = x & 0xFFF;
	return (x);
}


u16 TPReadY ( void ) {
	u16 y = 0;
	T_CS();
	SpiDelay(10);
	SPI_WriteByte ( 0xd0 );
	SpiDelay(10);
	y = SPI_WriteByte ( 0x0 );
	y <<= 8;
	y += SPI_WriteByte ( 0x0 );
	T_DCS();
	y = y >> 4;
	y = y & 0xFFF; //fff
	return (y);
}

		   
u8 read_once ( void ) {
	Pen_Point.X = TPReadX();
	Pen_Point.Y = TPReadY();
	return 1;
}	 

 


void touch_init ( void ) {
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE );

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init ( GPIOB, &GPIO_InitStruct );

	GPIO_PinAFConfig ( GPIOB, GPIO_PinSource13, GPIO_AF_SPI2 ); //sclk	 13
	GPIO_PinAFConfig ( GPIOB, GPIO_PinSource14, GPIO_AF_SPI2 ); //miso	 14
	GPIO_PinAFConfig ( GPIOB, GPIO_PinSource15, GPIO_AF_SPI2 ); //mosi	 15

	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_SPI2, ENABLE );

	SPI_I2S_DeInit ( SPI2 );
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //SPI_CPOL_Low 	 SPI_CPOL_High
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //SPI_NSS_Hard	 //SPI_NSS_Soft
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init ( SPI2, &SPI_InitStructure );
	SPI_Cmd ( SPI2, ENABLE );
	//CS
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = TOUCH_CS_PIN;
	GPIO_Init ( TOUCH_CS_PORT, &GPIO_InitStruct );
	T_DCS();
	//Interrupt request
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init ( GPIOC, &GPIO_InitStruct );

	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_SYSCFG, ENABLE );

	SYSCFG_EXTILineConfig ( EXTI_PortSourceGPIOC, EXTI_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init ( &EXTI_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init ( &NVIC_InitStructure );
}

  	   
u8 Read_Ads7846(void) {
	u8 t, t1, count = 0;
	u16 databuffer[2][10];
	u16 temp = 0;

	if (GPIO_ReadInputDataBit ( GPIOC, GPIO_Pin_4 ) == 0) {
		read_once();
		while (count < 10) {
			{
				if (read_once()) {
					databuffer[0][count] = Pen_Point.X;
					databuffer[1][count] = Pen_Point.Y;
					count++;
				}
			}
		}

		if (count == 10) {
			do {
				t1 = 0;
				for (t = 0; t < count - 1; t++) {
					if (databuffer[0][t] > databuffer[0][t + 1]) {
						temp = databuffer[0][t + 1];
						databuffer[0][t + 1] = databuffer[0][t];
						databuffer[0][t] = temp;
						t1 = 1;
					}
				}
			} while (t1);
			do {
				t1 = 0;
				for (t = 0; t < count - 1; t++) {
					if (databuffer[1][t] > databuffer[1][t + 1]) {
						temp = databuffer[1][t + 1];
						databuffer[1][t + 1] = databuffer[1][t];
						databuffer[1][t] = temp;
						t1 = 1;
					}
				}
			} while (t1);

			Pen_Point.X = 2047
					- ((databuffer[0][3] + databuffer[0][4] + databuffer[0][5])
							/ 3);
			Pen_Point.Y = ((databuffer[1][3] + databuffer[1][4]
					+ databuffer[1][5]) / 3);
			flag = 1;
			return 1;
		}
		flag = 0;
	}
	return 0;

}

void EXTI9_5_IRQHandler ( void ) {
	if (EXTI_GetITStatus ( EXTI_Line5 ) != RESET) {
		EXTI_ClearITPendingBit ( EXTI_Line5 );
		// Read_Ads7846();
	}
}

void Convert_Pos(void) {
	Read_Ads7846();
	Pen_Point.X0 = (int) ((Pen_Point.Y - 103) / 7.7);
	Pen_Point.Y0 = (int) ((Pen_Point.X - 104) / 5.56);
	if (Pen_Point.X0 > 240) {
		Pen_Point.X0 = 240;
	}
	if (Pen_Point.Y0 > 320) {
		Pen_Point.X0 = 320;
	}

} 
