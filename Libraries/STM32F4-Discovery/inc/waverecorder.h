/**
  ******************************************************************************
  * @file    Audio_playback_and_record/inc/waverecorder.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    28-October-2011
  * @brief   Header for waverecorder.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2S_AUDIO_H
#define __I2S_AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Exported types ------------------------------------------------------------*/
/* Exported Defines ----------------------------------------------------------*/
/* SPI Configuration defines */
#define SPI_SCK_PIN                       GPIO_Pin_10
#define SPI_SCK_GPIO_PORT                 GPIOB
#define SPI_SCK_GPIO_CLK                  RCC_AHB1Periph_GPIOB
#define SPI_SCK_SOURCE                    GPIO_PinSource10
#define SPI_SCK_AF                        GPIO_AF_SPI2

#define SPI_MOSI_PIN                      GPIO_Pin_3
#define SPI_MOSI_GPIO_PORT                GPIOC
#define SPI_MOSI_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define SPI_MOSI_SOURCE                   GPIO_PinSource3
#define SPI_MOSI_AF                       GPIO_AF_SPI2

#define AUDIO_REC_SPI_IRQHANDLER          SPI2_IRQHandler


/* Audio recording frequency in Hz */
#define REC_FREQ                          8000  

/* PDM buffer input size */
#define INTERNAL_BUFF_SIZE      64

/* PCM buffer output size */
#define PCM_OUT_SIZE            16


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void AUDIO_REC_SPI_IRQHANDLER(void);
uint32_t WaveRecorderStop(void);
uint32_t WavaRecorderHeaderInit(uint8_t* pHeadBuf);
void Delay(__IO uint32_t nTime);
void WaveRecorderUpdate(void);
void simple_rec_start(void);

#endif /* __WAVE_RECORDER_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
