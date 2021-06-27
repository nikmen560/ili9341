/**
  ******************************************************************************
  * @file    DCMI/Camera/dcmi_OV7670.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   This file includes the driver for OV7670 Camera module mounted on
  *          STM324xG-EVAL board RevB.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "camera_api.h"
#include "dcmi_ov7670.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DCMI_Camera
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  TIMEOUT  2

#define  CAMERA_PWR_EN_PORT		GPIOA
#define  CAMERA_PWR_EN_PIN		GPIO_Pin_2

/* Bits definitions ----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Writes a byte at a specific Camera register
  * @param  I2Cx: where x can be 1, 2 or 3 to select the camera I2C peripheral
  * @param  Device: Camera write address.
  * @param  Addr: Camera register address.
  * @param  Data: data to be written to the specific register
  * @retval 0x00 if write operation is OK.
  *         0xFF if timeout condition occured (device not connected or bus error).
  */
uint8_t Camera_I2C_SingleRandomWrite ( I2C_TypeDef* I2Cx, uint8_t Device, uint16_t Addr, uint8_t Data ) {
	uint32_t timeout = DCMI_TIMEOUT_MAX;

	/* Generate the Start Condition */
	I2C_GenerateSTART ( I2Cx, ENABLE );

	/* Test on I2C1 EV5 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_MODE_SELECT )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send DCMI selected device slave Address for write */
	I2C_Send7bitAddress ( I2Cx, Device, I2C_Direction_Transmitter );

	/* Test on I2Cx EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send I2Cx location address LSB */
	I2C_SendData ( I2Cx, (uint8_t) (Addr) );

	/* Test on I2Cx EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send Data */
	I2C_SendData ( I2Cx, Data );

	/* Test on I2Cx EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send I2Cx STOP Condition */
	I2C_GenerateSTOP ( I2Cx, ENABLE );

	/* If operation is OK, return 0 */
	return 0;
}

/**
  * @brief  Reads a byte from a specific Camera register
  * @param  I2Cx: where x can be 1, 2 or 3 to select the camera I2C peripheral
  * @param  Device: Camera write address
  * @param  Addr: Camera register address
  * @retval data read from the specific register or 0xFF if timeout condition
  *         occured.
  */
uint8_t Camera_I2C_SingleRandomRead ( I2C_TypeDef* I2Cx, uint8_t Device, uint16_t Addr ) {
	uint32_t timeout = DCMI_TIMEOUT_MAX;
	uint8_t Data = 0;

	/* Generate the Start Condition */
	I2C_GenerateSTART ( I2Cx, ENABLE );

	/* Test on I2Cx EV5 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_MODE_SELECT )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send DCMI selected device slave Address for write */
	I2C_Send7bitAddress ( I2Cx, Device, I2C_Direction_Transmitter );

	/* Test on I2Cx EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send I2Cx location address LSB */
	I2C_SendData ( I2Cx, (uint8_t) (Addr) );

	/* Test on I2Cx EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Clear AF flag if arised */
	I2Cx->SR1 |= (uint16_t) 0x0400;

	/* Generate the Stop Condition */
	I2C_GenerateSTOP ( I2Cx, ENABLE );

	/* Generate the Start Condition */
	I2C_GenerateSTART ( I2Cx, ENABLE );

	/* Test on I2Cx EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_MODE_SELECT )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Send DCMI selected device slave Address for write */
	I2C_Send7bitAddress ( I2Cx, Device, I2C_Direction_Receiver );

	/* Test on I2Cx EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Prepare an NACK for the next data received */
	I2C_AcknowledgeConfig ( I2Cx, DISABLE );

	/* Test on I2Cx EV7 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while (!I2C_CheckEvent ( I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED )) {
		/* If the timeout delay is exceeded, exit with error code */
		if ((timeout--) == 0)
			return 0xFF;
	}

	/* Prepare Stop after receiving data */
	I2C_GenerateSTOP ( I2Cx, ENABLE );

	/* Receive the Data */
	Data = I2C_ReceiveData ( I2Cx );

	/* return the read data */
	return Data;
}

/**
  * @brief  Set the QVGA size(240*320).
  * @param  None
  * @retval None
  */
void Camera_OV7670_QVGASizeSetup ( I2C_TypeDef* I2Cx ) {
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM7, COM7_RESET ); /* reset to default values */
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_TSLB, 0x0C );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM15, COM15_RGB565 | COM15_R00FF );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM7, COM7_FMT_QVGA | COM7_RGB );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HREF, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HSTART, 0x16 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HSTOP, 0x04 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_VSTART, 0x02 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_VSTOP, 0x7a );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_VREF, 0x03 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM14, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SCL_XSC, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SCL_YSC, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SCALING_DCWCTR, 0x11 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SCALING_PC, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SCALING_PCLK_DELAY, 0x02 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_CLKRC, 0x01 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SLOP, 0x20 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM1, 0x1c );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM2, 0x28 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM3, 0x3c );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM4, 0x55 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM5, 0x68 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM6, 0x76 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM7, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM8, 0x88 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM9, 0x8f );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM10, 0x96 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM11, 0xa3 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM12, 0xaf );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM13, 0xc4 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM14, 0xd7 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAM15, 0xe8 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AWB | COM8_AEC );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GAIN, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AECH, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM4, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM9, COM9_AGC_8X );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_BD50MAX, 0x05 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_BD60MAX, 0x07 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AEW, 0x75 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AEB, 0x63 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_VPT, 0xA5 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC1, 0x78 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC2, 0x68 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGA1, 0x03 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC3, 0xdf );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC4, 0xdf );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC5, 0xf0 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC6, 0x90 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_HAECC7, 0x94 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AWB | COM8_AEC );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM5, 0x61 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM6, 0x82 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG16, 0x02 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MVFP, MVFP_MIRROR | 0x07 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_ADCCTR1, 0x02 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_ADCCTR2, 0x91 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG29, 0x07 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_CHLF, 0x0b );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG35, 0x0b );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_ADC, 0x1d );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_ACOM, 0x71 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_OFON, 0x2a );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM12, 0x78 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG4D, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG4E, 0x20 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GFIX, 0x0c );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_DBLV, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG74, 0x19 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG8D, 0x4f );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG8E, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG8F, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG90, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG91, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_DM_LNL, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG96, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG9A, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGB0, 0x84 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_ABLC1, 0x0c );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGB2, 0x0e );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_THL_DLT, 0x82 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGB8, 0x0a );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBC1, 0x14 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBC2, 0xf0 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBC3, 0x34 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBC4, 0x58 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBC5, 0x28 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBC6, 0x3a );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG59, 0x88 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG5A, 0x88 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG5B, 0x44 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG5C, 0x67 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG5D, 0x49 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG5E, 0x0e );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_LCC3, 0x04 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_LCC4, 0x20 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_LCC5, 0x05 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_LCC6, 0x04 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_LCC7, 0x08 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBCTR3, 0x0a );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBCTR2, 0x55 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBCTR1, 0x11 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AWBCTR0, 0x9f );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_GGAIN, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_BLUE, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_RED, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AWB | COM8_AEC );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM10, COM10_PCLK_REV | COM10_HREF_REV );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MTX1, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MTX2, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MTX3, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MTX4, 0x22 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MTX5, 0x5e );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_MTX6, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_CMATRIX_SIGN, 0x9e );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM16, 0x08 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_EDGE, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG75, 0x05 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG76, 0xe1 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_DNSTH, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG77, 0x01 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM13, COM13_GAMMA | COM13_UVSAT );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG4B, 0x09 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_SATCTR, 0x60 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM16, 0x38 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_CONTRAST, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_ARBLM, 0x11 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM11, 0x02 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_NT_CTRL, 0x89 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG96, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG97, 0x30 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG98, 0x20 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG99, 0x30 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG9A, 0x84 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG9B, 0x29 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG9C, 0x03 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_BD50ST, 0x4c );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_BD60ST, 0x3f );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG78, 0x04 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x01 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0xf0 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x0f );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x10 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x7e );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x0a );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x0b );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x01 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x0c );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x0f );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x0d );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x20 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x09 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x80 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x02 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0xc0 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x03 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x05 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REGC8, 0x30 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_REG79, 0x26 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM2, COM2_ODCAP_4x );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_BRIGHT, 0x00 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_CONTRAST, 0x40 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_COM11, 0x42 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AECHH, 0x08 );
	Camera_I2C_SingleRandomWrite ( I2Cx, OV7670_DEVICE_WRITE_ADDRESS, OV7670_AECH, 0x80 );
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
