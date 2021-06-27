/*
*******************************************************************************
  Copyright (C), 2012-2014, Embest Tech. Co., Ltd.
  FileName     : stm32f4_discovery_debug.c      
  Version      : 1.0.0     
  Date         : 2012/05/29
  Description  : 
  Function List: 
  History      :
  <author>     : lichy       
  <time>       : 2012/05/29
  <version >   : 1.0.0 	
  <desc>       : build this moudle			 
*******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery_debug.h"

#ifdef	BASED_ON_MAKEFILE
	typedef enum
	{
	  COM1 = 0,
	  COM2 = 1,
	  COM3 = 2,
	  COM4 = 3,
	  COM5 = 4,
	  COM6 = 5,
	} COM_TypeDef;

	/** @addtogroup STM32F4_DISCOVERY_LOW_LEVEL_USART
	  * @{
	  */
	#define COMn                             2
	/**
	 * @brief Definition for COM port3, connected to USART1
	 */
	#define EVAL_COM3                        USART1
	#define EVAL_COM3_CLK                    RCC_APB2Periph_USART1
	#define EVAL_COM3_TX_PIN                 GPIO_Pin_9
	#define EVAL_COM3_TX_GPIO_PORT           GPIOA
	#define EVAL_COM3_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
	#define EVAL_COM3_TX_SOURCE              GPIO_PinSource9
	#define EVAL_COM3_TX_AF                  GPIO_AF_USART1
	#define EVAL_COM3_RX_PIN                 GPIO_Pin_10
	#define EVAL_COM3_RX_GPIO_PORT           GPIOA
	#define EVAL_COM3_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
	#define EVAL_COM3_RX_SOURCE              GPIO_PinSource10
	#define EVAL_COM3_RX_AF                  GPIO_AF_USART1
	#define EVAL_COM3_IRQn                   USART1_IRQn

	/**
	 * @brief Definition for COM port2, connected to USART3
	 */
	#define EVAL_COM2                        USART3
	#define EVAL_COM2_CLK                    RCC_APB1Periph_USART3
	#define EVAL_COM2_TX_PIN                 GPIO_Pin_10
	#define EVAL_COM2_TX_GPIO_PORT           GPIOC
	#define EVAL_COM2_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
	#define EVAL_COM2_TX_SOURCE              GPIO_PinSource10
	#define EVAL_COM2_TX_AF                  GPIO_AF_USART3
	#define EVAL_COM2_RX_PIN                 GPIO_Pin_11
	#define EVAL_COM2_RX_GPIO_PORT           GPIOC
	#define EVAL_COM2_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
	#define EVAL_COM2_RX_SOURCE              GPIO_PinSource11
	#define EVAL_COM2_RX_AF                  GPIO_AF_USART3
	#define EVAL_COM2_IRQn                   USART3_IRQn

	/**
	 * @brief Definition for COM port1, connected to USART4
	 */
	#define EVAL_COM1                        USART6
	#define EVAL_COM1_CLK                    RCC_APB2Periph_USART6
	#define EVAL_COM1_TX_PIN                 GPIO_Pin_6
	#define EVAL_COM1_TX_GPIO_PORT           GPIOC
	#define EVAL_COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
	#define EVAL_COM1_TX_SOURCE              GPIO_PinSource6
	#define EVAL_COM1_TX_AF                  GPIO_AF_USART6
	#define EVAL_COM1_RX_PIN                 GPIO_Pin_7
	#define EVAL_COM1_RX_GPIO_PORT           GPIOC
	#define EVAL_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
	#define EVAL_COM1_RX_SOURCE              GPIO_PinSource7
	#define EVAL_COM1_RX_AF                  GPIO_AF_USART6
	#define EVAL_COM1_IRQn                   USART6_IRQn
	/**
	  * @}
	  */
#endif

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup stm32f4_discovery_debug
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE unsigned char __io_putchar(unsigned char ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Display Init (LCD or/and USART)
  * @param  None
  * @retval None
  */
void STM32f4_Discovery_Debug_Init(void) {
#ifdef DEBUG_ENABLE
	USART_InitTypeDef USART_InitStructure;
	/* USARTx configured as follow:
	 - BaudRate = 115200 baud
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (RTS and CTS signals)
	 - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
#if DEBUG_ENABLE == 1
	STM_EVAL_COMInit(COM1, &USART_InitStructure);
#elif DEBUG_ENABLE == 2
	STM_EVAL_COMInit(COM2, &USART_InitStructure);
#elif DEBUG_ENABLE == 3
	STM_EVAL_COMInit(COM3, &USART_InitStructure);
#endif
#else
	USART_InitTypeDef USART_InitStructure;
	/* USARTx configured as follow:
	 - BaudRate = 115200 baud
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (RTS and CTS signals)
	 - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	STM_EVAL_COMInit(COM1, &USART_InitStructure);
#endif
	DEBUG("\n\r Debug Module Init \n\r");
}

/**
  * @brief  Read character but no wait if no key
  * @param  None
  * @retval None
  */
/*  */
int Get_Peek_Key(void) {
#ifdef DEBUG_ENABLE
#if DEBUG_ENABLE == 1
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE ) == RESET) {}
	return (USART_ReceiveData(EVAL_COM1 ));
#elif DEBUG_ENABLE == 2
	while (USART_GetFlagStatus(EVAL_COM2, USART_FLAG_RXNE) == RESET) {}
	return (USART_ReceiveData(EVAL_COM2));
#elif DEBUG_ENABLE == 3
	while (USART_GetFlagStatus(EVAL_COM3, USART_FLAG_RXNE) == RESET) {}
	return (USART_ReceiveData(EVAL_COM3));
#endif
#else
	return 0;
#endif


}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
unsigned char __io_putchar(unsigned char ch) {
#ifdef DEBUG_ENABLE
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC ) == RESET) {}

	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
#if DEBUG_ENABLE == 1
	USART_SendData(EVAL_COM1, ch);
#elif DEBUG_ENABLE == 2
	USART_SendData(EVAL_COM2, ch);
#elif DEBUG_ENABLE == 3
	USART_SendData(EVAL_COM3, ch);
#endif
#endif
	return ch;
}
