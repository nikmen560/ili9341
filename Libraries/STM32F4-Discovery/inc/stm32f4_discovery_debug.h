/*
*******************************************************************************
  Copyright (C), 2012-2014, Embest Tech. Co., Ltd.
  FileName     : stm32f4_discovery_debug.h      
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_DEBUG_H
#define __STM32F4_DISCOVERY_DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif
                                              
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Defines -------------------------------------------------------------------*/
#ifdef DEBUG_ENABLE
#define DEBUG(format, args...) printf(format, ##args)
#else
#define DEBUG(format, args...)
#endif /* USE_DEBUG */
/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32F4_DISCOVERY
  * @{
  */
/** @defgroup STM32F4_DISCOVERY_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM32f4_Discovery_Debug_Init(void);
int Get_Peek_Key(void);
unsigned char __io_putchar(unsigned char ch);
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_DEBUG_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
