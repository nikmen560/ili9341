/**
  ******************************************************************************
  * @file    DCMI/Camera/dcmi_OV7670.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Header for dcmi_OV7670.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DCMI_OV7670_H
#define __DCMI_OV7670_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"




/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Use this define to set the maximum delay timeout for the I2C DCMI_OV7670_SingleRandomWrite()
   and DCMI_OV7670_SingleRandomRead() operations. Exeeding this timeout delay,
   the read/write functions will be aborted and return error code (0xFF).
   The period of the delay will depend on the system operating frequency. The following
   value has been set for system running at 168 MHz. */
#define DCMI_TIMEOUT_MAX               10000

#define OV7670_DEVICE_WRITE_ADDRESS    0x42
#define OV7670_DEVICE_READ_ADDRESS     0x43

/* OV7670 Registers definition when DSP bank selected (0xFF = 0x00) */

/* OV7670 Registers definition when sensor bank selected (0xFF = 0x01) */
	/* Gain lower 8 bits (rest in vref) */
#define OV7670_GAIN			0x00
#define OV7670_BLUE			0x01    /* blue gain */
#define OV7670_RED			0x02    /* red gain */
#define OV7670_VREF			0x03    /* Pieces of GAIN, VSTART, VSTOP */
#define OV7670_COM1			0x04    /* Control 1 */
#define  COM1_CCIR656		 0x40    /* CCIR656 enable */
#define  COM1_HREF_SKIP_0	 0x00
#define  COM1_HREF_SKIP_1	 0x04
#define  COM1_HREF_SKIP_3	 0x08
#define OV7670_BAVE			0x05    /* U/B Average level */
#define OV7670_GbAVE		0x06    /* Y/Gb Average level */
#define OV7670_AECHH		0x07    /* AEC MS 5 bits */
#define OV7670_RAVE			0x08    /* V/R Average level */
#define OV7670_COM2			0x09    /* Control 2 */
#define  COM2_SSLEEP		 0x10    /* Soft sleep mode */
#define  COM2_ODCAP_1x		 0x00
#define  COM2_ODCAP_2x		 0x01
#define  COM2_ODCAP_3x		 0x02
#define  COM2_ODCAP_4x		 0x03
#define OV7670_PID			0x0A    /* Product ID MSB */
#define OV7670_VER			0x0B    /* Product ID LSB */
#define OV7670_COM3			0x0C    /* Control 3 */
#define	 COLOR_BAR_OUTPUT	 0x80
#define  COM3_SWAP			 0x40    /* Byte swap */
#define  COM3_SCALEEN		 0x08    /* Enable scaling */
#define  COM3_DCWEN			 0x04    /* Enable downsamp/crop/window */
#define	 COM3_SINGLE_FRAME	 0x01
#define	 COM3_RGB565_FORMAT	 0x00
#define OV7670_COM4			0x0D    /* Control 4 */
#define OV7670_COM5			0x0E    /* All "reserved" */
#define	 COM5_SLAM_MODE_ENABLE     0x40
#define	 COM5_EXPOSURE_NORMAL_MODE 0x01
#define OV7670_COM6			0x0F    /* Control 6 */
#define OV7670_AECH			0x10    /* More bits of AEC value */
#define OV7670_CLKRC		0x11    /* Clock control */
#define  CLK_EXT			 0x40    /* Use external clock directly */
#define  CLK_SCALE			 0x3F    /* Mask for internal clock scale */
#define OV7670_COM7			0x12    /* Control 7 */
#define  COM7_RESET			 0x80    /* Register reset */
#define  COM7_FMT_MASK		 0x38
#define  COM7_FMT_VGA		 0x00
#define  COM7_FMT_CIF		 0x20    /* CIF format */
#define  COM7_FMT_QVGA		 0x10    /* QVGA format */
#define  COM7_FMT_QCIF		 0x08    /* QCIF format */
#define  COM7_RGB			 0x04    /* bits 0 and 2 - RGB format */
#define  COM7_YUV			 0x00    /* YUV */
#define  COM7_BAYER			 0x01    /* Bayer format */
#define  COM7_PBAYER		 0x05    /* "Processed bayer" */
#define OV7670_COM8			0x13    /* Control 8 */
#define  COM8_FASTAEC		 0x80    /* Enable fast AGC/AEC */
#define  COM8_AECSTEP		 0x40    /* Unlimited AEC step size */
#define  COM8_BFILT			 0x20    /* Band filter enable */
#define  COM8_AGC			 0x04    /* Auto gain enable */
#define  COM8_AWB			 0x02    /* White balance enable */
#define  COM8_AEC			 0x01    /* Auto exposure enable */
#define OV7670_COM9			0x14    /* Control 9  - gain ceiling */
#define	 COM9_AGC_2X		 0x00
#define	 COM9_AGC_4X		 0x10	/* Automatic Gain Ceiling 4X */
#define	 COM9_AGC_8X		 0x20
#define	 COM9_AGC_16X		 0x30
#define	 COM9_AGC_32X		 0x40
#define	 COM9_AGC_64X		 0x50
#define	 COM9_AGC_128X		 0x60
#define	 COM9_DROP_VSYNC	 0x04
#define	 COM9_DROP_HREF		 0x02
#define OV7670_COM10		0x15    /* Control 10 */
#define	 COM10_REMAP_SLHS	 0x80
#define  COM10_HSYNC		 0x40    /* HSYNC instead of HREF */
#define  COM10_PCLK_HB		 0x20    /* Suppress PCLK on horiz blank */
#define  COM10_PCLK_REV		 0x10
#define  COM10_HREF_REV		 0x08    /* Reverse HREF */
#define  COM10_VS_LEAD		 0x04    /* VSYNC on clock leading edge */
#define  COM10_VS_NEG		 0x02    /* VSYNC negative */
#define  COM10_HS_NEG		 0x01    /* HSYNC negative */
#define OV7670_REG16		0x16
#define OV7670_HSTART		0x17    /* Horiz start high bits */
#define OV7670_HSTOP		0x18    /* Horiz stop high bits */
#define OV7670_VSTART		0x19    /* Vert start high bits */
#define OV7670_VSTOP		0x1A    /* Vert stop high bits */
#define OV7670_PSHFT		0x1B    /* Pixel delay after HREF */
#define OV7670_MIDH			0x1C    /* Manuf. ID high */
#define OV7670_MIDL			0x1D    /* Manuf. ID low */
#define OV7670_MVFP			0x1E    /* Mirror / vflip */
#define  MVFP_MIRROR		0x20    /* Mirror image */
#define  MVFP_FLIP			0x10    /* Vertical flip */
#define OV7670_LAEC			0x1F
#define OV7670_ADCCTR0		0x20
#define OV7670_ADCCTR1		0x21
#define OV7670_ADCCTR2		0x22
#define OV7670_ADCCTR3		0x23
#define OV7670_AEW			0x24    /* AGC upper limit */
#define OV7670_AEB			0x25    /* AGC lower limit */
#define OV7670_VPT			0x26    /* AGC/AEC fast mode op region */
#define OV7670_BBIAS		0x27
#define OV7670_GbBIAS		0x28
#define OV7670_REG29		0x29
#define OV7670_EXHCH		0x2A
#define OV7670_EXHCL		0x2B
#define OV7670_RBIAS		0x2C
#define OV7670_ADVFL		0x2D
#define OV7670_ADVFH		0x2E
#define OV7670_YAVE			0x2F
#define OV7670_HSYST		0x30    /* HSYNC rising edge delay */
#define OV7670_HSYEN		0x31    /* HSYNC falling edge delay */
#define OV7670_HREF			0x32    /* HREF pieces */
#define OV7670_CHLF			0x33
#define OV7670_ARBLM		0x34
#define OV7670_REG35		0x35
#define OV7670_REG36		0x36
#define OV7670_ADC			0x37
#define OV7670_ACOM			0x38
#define OV7670_OFON			0x39
#define OV7670_TSLB			0x3A    /* lots of stuff */
#define	 PCLK_DELAY_0		 0x00
#define	 PCLK_DELAY_2		 0x40
#define	 PCLK_DELAY_4		 0x80
#define	 PCLK_DELAY_6		 0xC0
#define	 OUTPUT_BITWISE_REV	 0x20
#define	 UV_NORMAL			 0x00
#define	 UV_FIXED			 0x10
#define	 YUV_SEQ_YUYV		 0x00
#define	 YUV_SEQ_YVYU		 0x02
#define	 YUV_SEQ_VYUY		 0x04	/* UYVY or VYUY - see com13 */
#define	 YUV_SEQ_UYVY		 0x06
#define	 BANDING_FREQ_50	 0x02
#define OV7670_COM11		0x3B    /* Control 11 */
#define  COM11_NIGHT		 0x80    /* NIght mode enable */
#define  COM11_NMFR			 0x60    /* Two bit NM frame rate */
#define  COM11_HZAUTO		 0x10    /* Auto detect 50/60 Hz */
#define  COM11_50HZ			 0x08    /* Manual 50Hz select */
#define  COM11_EXP			 0x02
#define OV7670_COM12		0x3C    /* Control 12 */
#define  COM12_HREF			 0x80    /* HREF always */
#define OV7670_COM13		0x3D    /* Control 13 */
#define  COM13_GAMMA		 0x80    /* Gamma enable */
#define  COM13_UVSAT		 0x40    /* UV saturation auto adjustment */
#define  COM13_UVSWAP		 0x01    /* V before U - w/TSLB */
#define OV7670_COM14		0x3E    /* Control 14 */
#define  COM14_DCWEN		 0x10    /* DCW/PCLK-scale enable */
#define OV7670_EDGE			0x3F    /* Edge enhancement factor */
#define OV7670_COM15		0x40    /* Control 15 */
#define  COM15_R10F0		 0x00    /* Data range 10 to F0 */
#define  COM15_R01FE		 0x80    /*            01 to FE */
#define  COM15_R00FF		 0xC0    /*            00 to FF */
#define  COM15_RGB565		 0x10    /* RGB565 output */
#define  COM15_RGB555		 0x30    /* RGB555 output */
#define OV7670_COM16		0x41    /* Control 16 */
#define  COM16_AWBGAIN		 0x08    /* AWB gain enable */
#define OV7670_COM17		0x42    /* Control 17 */
#define  COM17_AECWIN		 0xC0    /* AEC window - must match COM4 */
#define  COM17_CBAR			 0x08    /* DSP Color bar */
#define OV7670_AWBC1		0x43
#define OV7670_AWBC2		0x44
#define OV7670_AWBC3		0x45
#define OV7670_AWBC4		0x46
#define OV7670_AWBC5		0x47
#define OV7670_AWBC6		0x48
#define OV7670_REG49		0x49
#define OV7670_REG4A		0x4A
#define OV7670_REG4B		0x4B
#define OV7670_DNSTH		0x4C
#define OV7670_REG4D		0x4D
#define OV7670_REG4E		0x4E
#define OV7670_MTX1			0x4F
#define OV7670_MTX2			0x50
#define OV7670_MTX3			0x51
#define OV7670_MTX4			0x52
#define OV7670_MTX5			0x53
#define OV7670_MTX6			0x54
#define  CMATRIX_LEN		 6
#define OV7670_BRIGHT		0x55    /* Brightness */
#define OV7670_CONTRAST		0x56    /* Contrast control */
#define OV7670_CONTRASCENTER 0x57
#define OV7670_CMATRIX_SIGN	0x58
#define OV7670_REG59		0x59
#define OV7670_REG5A		0x5A
#define OV7670_REG5B		0x5B
#define OV7670_REG5C		0x5C
#define OV7670_REG5D		0x5D
#define OV7670_REG5E		0x5E
#define OV7670_REG5F		0x5F
#define OV7670_REG60		0x60
#define OV7670_REG61		0x61
#define OV7670_LCC1			0x62
#define OV7670_LCC2			0x63
#define OV7670_LCC3			0x64
#define OV7670_LCC4			0x65
#define OV7670_LCC5			0x66
#define OV7670_MANU			0x67
#define OV7670_MANV			0x68
#define OV7670_GFIX			0x69    /* Fix gain control */
#define OV7670_GGAIN		0x6A
#define OV7670_DBLV			0x6B
#define OV7670_AWBCTR3		0x6C
#define OV7670_AWBCTR2		0x6D
#define OV7670_AWBCTR1		0x6E
#define OV7670_AWBCTR0		0x6F
#define OV7670_SCL_XSC		0x70    /* Horizontal scale factor */
#define OV7670_SCL_YSC		0x71    /* Vertical scale factor */
#define OV7670_SCALING_DCWCTR 0x72
#define OV7670_SCALING_PC	0x73
#define OV7670_REG74		0x74
#define OV7670_REG75		0x75
#define OV7670_REG76		0x76    /* OV's name */
#define  R76_BLKPCOR		 0x80    /* Black pixel correction enable */
#define  R76_WHTPCOR		 0x40    /* White pixel correction enable */
#define OV7670_REG77		0x77
#define OV7670_REG78		0x78
#define OV7670_REG79		0x79
#define OV7670_SLOP			0x7A
#define OV7670_GAM1			0x7B
#define OV7670_GAM2			0x7C
#define OV7670_GAM3			0x7D
#define OV7670_GAM4			0x7E
#define OV7670_GAM5			0x7F
#define OV7670_GAM6			0x80
#define OV7670_GAM7			0x81
#define OV7670_GAM8			0x82
#define OV7670_GAM9			0x83
#define OV7670_GAM10		0x84
#define OV7670_GAM11		0x85
#define OV7670_GAM12		0x86
#define OV7670_GAM13		0x87
#define OV7670_GAM14		0x88
#define OV7670_GAM15		0x89
#define OV7670_REG8A		0x8A
#define OV7670_REG8B		0x8B
#define OV7670_RGB444		0x8C    /* RGB 444 control */
#define  R444_ENABLE		 0x02    /* Turn on RGB444, overrides 5x5 */
#define  R444_RGBX			 0x01    /* Empty nibble at end */
#define OV7670_REG8D		0x8D
#define OV7670_REG8E		0x8E
#define OV7670_REG8F		0x8F
#define OV7670_REG90		0x90
#define OV7670_REG91		0x91
#define OV7670_DM_LNL		0x92
#define OV7670_DM_LNH		0x93
#define OV7670_LCC6			0x94
#define OV7670_LCC7			0x95
#define OV7670_REG96		0x96
#define OV7670_REG97		0x97
#define OV7670_REG98		0x98
#define OV7670_REG99		0x99
#define OV7670_REG9A		0x9A
#define OV7670_REG9B		0x9B
#define OV7670_REG9C		0x9C
#define OV7670_BD50ST		0x9D
#define OV7670_BD60ST		0x9E
#define OV7670_HAECC1		0x9F    /* Hist AEC/AGC control 1 */
#define OV7670_HAECC2		0xA0    /* Hist AEC/AGC control 2 */
#define OV7670_REGA1		0xA1
#define OV7670_SCALING_PCLK_DELAY 0xA2
#define OV7670_REGA3		0xA3
#define OV7670_NT_CTRL		0xA4
#define OV7670_BD50MAX		0xA5    /* 50hz banding step limit */
#define OV7670_HAECC3		0xA6    /* Hist AEC/AGC control 3 */
#define OV7670_HAECC4		0xA7    /* Hist AEC/AGC control 4 */
#define OV7670_HAECC5		0xA8    /* Hist AEC/AGC control 5 */
#define OV7670_HAECC6		0xA9    /* Hist AEC/AGC control 6 */
#define OV7670_HAECC7		0xAA   /* Hist AEC/AGC control 7 */
#define OV7670_BD60MAX		0xAB    /* 60hz banding step limit */
#define OV7670_STR_OPT		0xAC
#define OV7670_STR_R		0xAD
#define OV7670_STR_G		0xAE
#define OV7670_STR_B		0xAF
#define OV7670_REGB0		0xB0
#define OV7670_ABLC1		0xB1
#define OV7670_REGB2		0xB2
#define OV7670_THL_DLT		0xB3
#define OV7670_REGB4		0xB4
#define OV7670_THL_DLT_AREA	0xB5
#define OV7670_REGB6		0xB6
#define OV7670_REGB7		0xB7
#define OV7670_REGB8		0xB8
#define OV7670_REGBC		0xBC
#define OV7670_REGBD		0xBD
#define OV7670_AD_CHB		0xBE
#define OV7670_AD_CHR		0xBF
#define OV7670_AD_CHGb		0xC0
#define OV7670_AD_CHGr		0xC1
#define OV7670_REGC2		0xC2
#define OV7670_REGC3		0xC3
#define OV7670_REGC4		0xC4
#define OV7670_REGC5		0xC5
#define OV7670_REGC6		0xC6
#define OV7670_REGC7		0xC7
#define OV7670_REGC8		0xC8
#define OV7670_SATCTR		0xC9

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint8_t Camera_I2C_SingleRandomWrite ( I2C_TypeDef* I2Cx, uint8_t Device, uint16_t Addr, uint8_t Data );
uint8_t Camera_I2C_SingleRandomRead ( I2C_TypeDef* I2Cx, uint8_t Device, uint16_t Addr );
void Camera_OV7670_QVGASizeSetup ( I2C_TypeDef* I2Cx );

#endif /* __DCMI_OV7670_H */
