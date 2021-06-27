/*****************************************************************************
 * @file    spi_flash.c
 * @date    03/28/2013
 * @brief   This file provides a set of functions needed to manage:
 *          - the communication between SPI peripheral and SPI M25P64 FLASH;
 *          - the IAP operation.
 *
 *          http://www.coocox.org/repo/e504b02d-30b2-4a91-be3f-3e095a9d3a32/src/TMPM34x_lib/src/tmpm341_ssp.c.htm
 *          http://tuxotronic.org/wiki/snippets/lpc2368-ssp-init-as-spi
 *          http://we.easyelectronics.ru/LPC/lpcxpresso-urok-8-spi-podklyuchaem-displey-ot-nokia-3310.html#comments
 *          https://github.com/linux4sam/at91bootstrap/tree/master/driver
 * (!!)     http://hardware-ntp.googlecode.com/svn-../trunk/pcb-1/
 *  		http://read.pudn.com/downloads163/sourcecode/embed/741535/it_amrith/App_Pros1/AT25DF321/AT25DF321.c__.htm
 *****************************************************************************/

/* LPC1768 - flash memory is placed address range of 0x00000000 - 0x0007FFFF
--
sector#  start_address   last_address   sector_size
      0     0x00000000  -  0x00000FFF            4K
      1     0x00001000  -  0x00001FFF            4K
      2     0x00002000  -  0x00002FFF            4K
      3     0x00003000  -  0x00003FFF            4K
      4     0x00004000  -  0x00004FFF            4K
      5     0x00005000  -  0x00005FFF            4K
      6     0x00006000  -  0x00006FFF            4K
      7     0x00007000  -  0x00007FFF            4K
      8     0x00008000  -  0x00008FFF            4K
      9     0x00009000  -  0x00009FFF            4K
     10     0x0000A000  -  0x0000AFFF            4K
     11     0x0000B000  -  0x0000BFFF            4K
     12     0x0000C000  -  0x0000CFFF            4K
     13     0x0000D000  -  0x0000DFFF            4K
     14     0x0000E000  -  0x0000EFFF            4K
     15     0x0000F000  -  0x0000FFFF            4K
     16     0x00010000  -  0x00017FFF           32K
     17     0x00018000  -  0x0001FFFF           32K
     18     0x00020000  -  0x00027FFF           32K
     19     0x00028000  -  0x0002FFFF           32K
     20     0x00030000  -  0x00037FFF           32K
     21     0x00038000  -  0x0003FFFF           32K
     22     0x00040000  -  0x00047FFF           32K
     23     0x00048000  -  0x0004FFFF           32K
     24     0x00050000  -  0x00057FFF           32K
     25     0x00058000  -  0x0005FFFF           32K
     26     0x00060000  -  0x00067FFF           32K
     27     0x00068000  -  0x0006FFFF           32K
     28     0x00070000  -  0x00077FFF           32K
     29     0x00078000  -  0x0007FFFF           32K
 */

/* Includes *******************************************************************/
//#include "lpc17xx_spi.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_gpio.h"
#include "spi_flash.h"
#include "app.h"

/* Private define *************************************************************/
								/* SPI Flash supported commands */
#define CMD_READ0	0x03		/* Read from Memory instruction (no dummy bytes) */
#define CMD_READ1	0x0B		/* Read from Memory instruction (+1 dummy byte) */
#define CMD_READ2	0x1B		/* Read from Memory instruction (+2 dummy bytes) */
#define CMD_READ	CMD_READ1
#define CMD_WRITE	0x02		/* Write to Memory instruction (1..256 bytes) */
#define CMD_WREN	0x06		/* Write Enable instruction */
#define CMD_WRDIS	0x04		/* Write Disable instruction */
#define CMD_RDSR	0x05		/* Read Status Register instruction  */
#define CMD_WRSR1	0x01		/* Write Status Register 1 instruction (here is "Write In Progress" flag) */
#define CMD_WRSR2	0x31		/* Write Status Register 2 instruction */
#define CMD_RDID	0x9F		/* Read identification */
#define CMD_CHER	0x60		/* Chip Erase instruction */
#define CMD_ER64k	0xD8		/* 64K Block Erase instruction */
#define CMD_ER32k	0x52		/* 32K Block Erase instruction */
#define CMD_ER4k	0x20		/*  4K Block Erase instruction */
#define CMD_BULKER	0xC7		/* Erase All sectors instruction */
#define CMD_SDPD	0xB9		/* Set Deep Power Down mode */
#define CMD_RDPD	0xAB		/* Resume from Deep Power Down mode */
#define CMD_RESET	0xF0		/* Flash Reset */
#define DUMMY_BYTE	0xFF

/* External variables ******************************************************--*/
extern U8 ErrorStatus;

/* Private macro ************************************************************-*/
U16 u16TmpSPI;
#define dTmp u16TmpSPI
#define AT25DF_SPI_ENABLE()				\
			cleanRxFIFO();				\
			SPI_FLASH_CS_PORT->FIOCLR = (1<<SPI_FLASH_PIN_CS); \
			SSP_WAIT_UNTIL_BUSY()
#define AT25DF_SPI_DISABLE()			\
			SSP_WAIT_UNTIL_BUSY();		\
			SPI_FLASH_CS_PORT->FIOSET = (1<<SPI_FLASH_PIN_CS)

#define AT25DF_WR_BYTE(b)			while (!(SSP_PORT->SR&0x0001)); SSP_PORT->DR = (U8)(b)
#define AT25DF_RD_BYTE(b)			while (!(SSP_PORT->SR&0x0004)); b = SSP_PORT->DR

#define SSP_WAIT_UNTIL_BUSY()		while (SSP_PORT->SR&0x0010)		// SSP status: (1<<4)Busy; (1<<2)RX FIFO not empty

#define SSP_CHECK_OVERRUN			(SSP_PORT->RIS & 0x01)

#define WAIT_TICK()					dTmp = SSP_PORT->SR

/* Private variables *********************************************************/

/* External variables ********************************************************/

/* Private function prototypes ***********************************************/

/* Private functions *********************************************************/

static void cleanRxFIFO(void)
{
	U8 Dummy;
	SSP_PORT->ICR = 0x0003;				// (2)RTIC | (1)RORIC
	while (SSP_PORT->SR&0x0004) Dummy = SSP_PORT->DR;
	Dummy = Dummy;						// just to avoid compilation warning
}

static U8 AT25DF_TXRX(U8 data)
{
	U8 ret;
	AT25DF_WR_BYTE(data);
	AT25DF_RD_BYTE(ret);
	return ret;
}

static inline void writeEnable(void)
{
	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_WREN);				// write enable command
	AT25DF_SPI_DISABLE();
}

static inline void writeDisable(void)
{
	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_WRDIS);				// write disable command
	AT25DF_SPI_DISABLE();
}

/*******************************************************************************
 * @brief  Writes up to 256 bytes into FLASH with a single WRITE cycle
 *         (Page WRITE sequence)
 * @param  addr:  destination Flash address
 *         buf:   pointer that contains the data to be written into FLASH
 *         len:   the number of bytes to write into FLASH (equal or less "AT25DF_PAGE_SIZE")
 * @retval None
 *******************************************************************************/
static void at25df_pageProgram(U32 addr, const U8 *buf, U16 len)
{
	U8 *ptr=(U8*)buf, i;

	WAIT_TICK();
	writeEnable ();
	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_WR_BYTE(CMD_WRITE);
	AT25DF_WR_BYTE(addr>>16);			// Address MSB
	AT25DF_WR_BYTE(addr>>8);
	AT25DF_WR_BYTE(addr);				// Address LSB
	while( len ) {
		AT25DF_WR_BYTE(*ptr);
		ptr++;
		len--;
	}
	AT25DF_SPI_DISABLE();

	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	i = AT25DF_TXRX(CMD_RDSR);
	while (1) {
		i = AT25DF_TXRX(DUMMY_BYTE);
		if( !(i&1) ) break;
		WAIT_TICK();
	}
	i = AT25DF_TXRX(DUMMY_BYTE);
	AT25DF_SPI_DISABLE();
	ErrorStatus &= ~SPI_ERROR_WRITE;
	if( i&0x20 ) {					// EPE (Erase or program error detected)
		ErrorStatus |= SPI_ERROR_WRITE;
	}

	writeDisable();
}

/*****************************************************************************
 * @brief  Initializes the peripherals used by the SPI FLASH driver.
 * @param  None
 * @retval None
 *****************************************************************************/
void SPI_FLASH_Init(void)
{
	// SSP1 (SPI) init sequence
	LPC_SC->PCONP |= (1<<21);		// PCSSP0 - power on
	LPC_PINCON->PINSEL3 &= ~((3<<8) | (3<<12) | (3<<14) | (3<<16));		// SCK0 | CS | MISO0 | MOSI0
	LPC_PINCON->PINSEL3 |= ((3<<8) | (0<<12) | (3<<14) | (3<<16));		// configure SPI0 pin outs
	SPI_FLASH_CS_PORT->FIODIR |= (1<<SPI_FLASH_PIN_CS);		// set CS pin as output
	SPI_FLASH_CS_PORT->FIOSET = (1<<SPI_FLASH_PIN_CS);		// deselect chip

	SSP_PORT->CR1 = 0x0;			// disable SSP, must be off during changes
	SSP_PORT->IMSC = 0;				// INTs disabled
	SSP_PORT->DMACR = 0;			// no DMA
	SSP_PORT->ICR = 3;				// Clear INTs: (1)RORIC; (2)RTIC
	cleanRxFIFO ();

	SSP_CFG_Type sspCfg;
	sspCfg.CPHA = 0;				// (0)ok
	sspCfg.CPOL = 0;				// (0)ok
	sspCfg.ClockRate = 1000000;
	sspCfg.Databit = 7;				// 8-bit
	sspCfg.FrameFormat = 0;			// spi
	sspCfg.Mode = 0;				// master
	SSP_Init(SSP_PORT,&sspCfg);

	SSP_PORT->CR1 |= 0x0002;		// Enable SSP
}

/*****************************************************************************
 * @brief  enable writing and clean protection SPI FLASH
 * @param  None
 * @retval None
 *****************************************************************************/
void SPI_FLASH_Unprotect (void)
{
	U8 i;

	writeEnable();

	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_WRSR1);			// status register, byte 1
	AT25DF_TXRX(0x00);				// global unprotect
	AT25DF_SPI_DISABLE();

	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_RDSR);			// status register
	i = AT25DF_TXRX(DUMMY_BYTE);	// check lock bits are now clear
	AT25DF_SPI_DISABLE();
	ErrorStatus &= ~SPI_ERROR_UNPROTECT;
	if( i&0x0C ) {					// unable to unprotect chip, write still protected
		ErrorStatus |= SPI_ERROR_UNPROTECT;
	}
}

/*****************************************************************************
 * @brief  Erases the specified FLASH 4K sector.
 * @param  SectorAddr: address of the sector to erase.
 * @retval None
 *****************************************************************************/
void SPI_FLASH_SectorErase4k (U32 addr)
{
	U16 i;

	writeEnable();

	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_ER4k);			// block erase command
	AT25DF_TXRX(addr>>16);			// Address MSB
	AT25DF_TXRX(addr>>8);
	AT25DF_TXRX(addr);				// Address LSB
	AT25DF_SPI_DISABLE();

	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_RDSR);			// check erase status
	while (1) {
		i = AT25DF_TXRX(0);			// read status
		if (!(i&1)) {
			break;
		}
		WAIT_TICK();
	}
	i = AT25DF_TXRX(DUMMY_BYTE);
	AT25DF_SPI_DISABLE();
	ErrorStatus &= ~SPI_ERROR_ERASE4K;
	if (i == 0x20) {				// failed to erase block
		ErrorStatus |= SPI_ERROR_ERASE4K;
	}
}

/*****************************************************************************
 * @brief  Writes block of data to the FLASH. In this function, the number of
 *   WRITE cycles are reduced, using Page WRITE sequence.
 * @param  pBuffer: pointer to the buffer  containing the data to be written
 *   to the FLASH.
 * @param  WriteAddr: FLASH's internal address to write to.
 * @param  NumByteToWrite: number of bytes to write to the FLASH.
 * @retval None
 *****************************************************************************/
U8 SPI_FLASH_BufferWrite (const U8* buf, U32 addr, U16 len)
{
	U8 status;
	U8 *bufCopy = (U8*)buf;

	if( len == 0 ) return 0;
	if (len > AT25DF_PAGE_SIZE) len = AT25DF_PAGE_SIZE;

//	CoEnterMutexSection (mut_flash);

	AT25DF_SPI_ENABLE();
	status = AT25DF_TXRX(CMD_RDSR);	// read status register command
	AT25DF_SPI_DISABLE();
	ErrorStatus &= ~SPI_ERROR_WRITEMODE;
	if( !(status&0x02) ) {			// Device is write enabled
		ErrorStatus |= SPI_ERROR_WRITEMODE;
		return SPI_ERROR_WRITEMODE;
	}

	addr--;
	// if buffer tail goes over page boundaries, split to two writes
	// each page is 256 byte long
	U16 pageOffset = (U16)(addr&AT25DF_PAGE_MASK);
	if (pageOffset + len > AT25DF_PAGE_SIZE) {
		U16 safeLen = AT25DF_PAGE_SIZE - pageOffset;
		at25df_pageProgram (addr, bufCopy, safeLen);
		addr = (addr&(~AT25DF_PAGE_MASK)) + AT25DF_PAGE_SIZE;		// write remaining bytes in the beginning of the next page
		len -= safeLen;
		bufCopy += safeLen;
	}
	at25df_pageProgram (addr, bufCopy, len);
//	CoLeaveMutexSection (mut_flash);

	return (ErrorStatus & SPI_ERROR_WRITE);
}

/*****************************************************************************
 * @brief  Reads block of data from the FLASH.
 * @param  buf:  pointer to the buffer that receives the data read from the FLASH.
 *         addr: source address of the Flash.
 *         num:  number of bytes to read from the FLASH.
 * @retval None
 *****************************************************************************/
void SPI_FLASH_BufferRead (const U8* buf, U32 addr, U16 len)
{
	U8 *ptr=(U8*)buf;
//	CoEnterMutexSection (mut_flash);
	//AT25DF_WAIT_UNTIL_DONE();

	AT25DF_SPI_ENABLE();
	AT25DF_WR_BYTE(CMD_READ);
	AT25DF_TXRX(addr>>16);					// Address MSB
	AT25DF_TXRX(addr>>8);
	AT25DF_TXRX(addr);						// Address LSB
#if (CMD_READ == CMD_READ1 || CMD_READ == CMD_READ2)
	AT25DF_TXRX(DUMMY_BYTE);				// Don't care byte according to 7.1 pg 8/
  #if (CMD_READ == CMD_READ2)
	AT25DF_TXRX(DUMMY_BYTE);				// Don't care byte according to 7.1 pg 8/
  #endif
#endif
	while (len--) *ptr++ = AT25DF_TXRX(DUMMY_BYTE);
	AT25DF_SPI_DISABLE();
//	CoLeaveMutexSection (mut_flash);
}

/*****************************************************************************
 * @brief  Reads FLASH identification.
 * @param  None
 * @retval FLASH identification
 *****************************************************************************/
U32 SPI_FLASH_ReadID (void)
{
	U32 Temp0, Temp1, Temp2, Temp3;

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_RDID);
	Temp0 = AT25DF_TXRX(DUMMY_BYTE);
	Temp1 = AT25DF_TXRX(DUMMY_BYTE);
	Temp2 = AT25DF_TXRX(DUMMY_BYTE);
	Temp3 = AT25DF_TXRX(DUMMY_BYTE);
	AT25DF_SPI_DISABLE();

	return (Temp0 << 24) | (Temp1 << 16) | (Temp2 << 8) | Temp3;
}

void SPI_FLASH_Reset (void)
{
	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_WRSR2);					// status register, byte 2
	AT25DF_TXRX(0x10);						// enable software reset
	AT25DF_SPI_DISABLE();

	WAIT_TICK();

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_RESET);					// software reset
	AT25DF_TXRX(0xD0);						// Confirmation byte
	AT25DF_SPI_DISABLE();
}

U32 SPI_FLASH_ReadStatus (void)
{
	U32 status1, status2, status3;

	AT25DF_SPI_ENABLE();
	AT25DF_TXRX(CMD_RDSR);
	status1 = AT25DF_TXRX(DUMMY_BYTE);		// status register byte 1
	status2 = AT25DF_TXRX(DUMMY_BYTE);		// status register byte 2
	status3 = AT25DF_TXRX(DUMMY_BYTE);		// status register byte 1
	AT25DF_SPI_DISABLE();
	return (status1<<16) | (status2<<8) | status3;
}

/*****************************************************************************
 * End Of File
 *****************************************************************************/
