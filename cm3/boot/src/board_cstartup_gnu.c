/**
 ******************************************************************************
 * @file      board_cstartup_gnu.c
 * @author    Coocox
 * @version   V1.0
 * @date      12/23/2009
 * @brief     AT91SAM3Ux Devices Startup code (for AT91 Library)
 *            This module performs:
 *                - Set the initial SP
 *                - Set the vector table entries with the exceptions ISR address
 *                - Initialize data and bss
 *                - Setup the microcontroller system.
 *                - Call the application's entry point.
 *            After Reset the Cortex-M3 processor is in Thread mode,
 *            priority is Privileged, and the Stack is set to Main.
 *******************************************************************************
 */
#include "exceptions.h"


/*----------Stack Configuration-----------------------------------------------*/  
#define STACK_SIZE       0x00000100      /*!< Stack size (in Words)           */
__attribute__ ((section(".co_stack")))
unsigned long pulStack[STACK_SIZE];     


/*----------Symbols defined in linker script----------------------------------*/  
extern unsigned long _sidata;    /*!< Start address for the initialization 
                                      values of the .data section.            */
extern unsigned long _sdata;     /*!< Start address for the .data section     */    
extern unsigned long _edata;     /*!< End address for the .data section       */    
extern unsigned long _sbss;      /*!< Start address for the .bss section      */
extern unsigned long _ebss;      /*!< End address for the .bss section        */      
extern void _eram;               /*!< End address for ram                     */


/*----------Function prototypes-----------------------------------------------*/  
extern int main(void);           /*!< The entry point for the application.    */
extern void LowLevelInit(void);  /*!< Setup the microcontroller system.       */
__attribute__ ((used))
void Reset_Handler(void);        /*!< Default reset handler                   */


/**
  *@brief The minimal vector table for a Cortex M3.  Note that the proper constructs
  *       must be placed on this to ensure that it ends up at physical address
  *       0x00000000.  
  */
__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
  /*----------Core Exceptions-------------------------------------------------*/        
  (void *)&pulStack[STACK_SIZE-1],     /*!< The initial stack pointer         */
  Reset_Handler,               /*!< Reset Handler                             */
  NMI_Handler,                 /*!< NMI Handler                               */
  HardFault_Handler,           /*!< Hard Fault Handler                        */
  MemManage_Handler,           /*!< MPU Fault Handler                         */
  BusFault_Handler,            /*!< Bus Fault Handler                         */
  UsageFault_Handler,          /*!< Usage Fault Handler                       */
  0,0,0,0,                     /*!< Reserved                                  */
  SVC_Handler,                 /*!< SVCall Handler                            */
  DebugMon_Handler,            /*!< Debug Monitor Handler                     */
  0,                           /*!< Reserved                                  */
  PendSV_Handler,              /*!< PendSV Handler                            */
  SysTick_Handler,             /*!< SysTick Handler                           */

  /*----------External Exceptions---------------------------------------------*/
  SUPC_IrqHandler,             /*!<  0 SUPPLY CONTROLLER                      */
  RSTC_IrqHandler,             /*!<  1 RESET CONTROLLER                       */
  RTC_IrqHandler,              /*!<  2 REAL TIME CLOCK                        */
  RTT_IrqHandler,              /*!<  3 REAL TIME TIMER                        */
  WDT_IrqHandler,              /*!<  4 WATCHDOG TIMER                         */
  PMC_IrqHandler,              /*!<  5 PMC                                    */
  EFC0_IrqHandler,             /*!<  6 EFC0                                   */
  EFC1_IrqHandler,             /*!<  7 EFC1                                   */
  DBGU_IrqHandler,             /*!<  8 DBGU                                   */
  HSMC4_IrqHandler,            /*!<  9 HSMC4                                  */
  PIOA_IrqHandler,             /*!< 10 Parallel IO Controller A               */
  PIOB_IrqHandler,             /*!< 11 Parallel IO Controller B               */
  PIOC_IrqHandler,             /*!< 12 Parallel IO Controller C               */
  USART0_IrqHandler,           /*!< 13 USART 0                                */
  USART1_IrqHandler,           /*!< 14 USART 1                                */
  USART2_IrqHandler,           /*!< 15 USART 2                                */
  USART3_IrqHandler,           /*!< 16 USART 3                                */
  MCI0_IrqHandler,             /*!< 17 Multimedia Card Interface              */
  TWI0_IrqHandler,             /*!< 18 TWI 0                                  */
  TWI1_IrqHandler,             /*!< 19 TWI 1                                  */
  SPI0_IrqHandler,             /*!< 20 Serial Peripheral Interface            */
  SSC0_IrqHandler,             /*!< 21 Serial Synchronous Controller 0        */
  TC0_IrqHandler,              /*!< 22 Timer Counter 0                        */
  TC1_IrqHandler,              /*!< 23 Timer Counter 1                        */
  TC2_IrqHandler,              /*!< 24 Timer Counter 2                        */
  PWM_IrqHandler,              /*!< 25 Pulse Width Modulation Controller      */
  ADCC0_IrqHandler,            /*!< 26 ADC controller0                        */
  ADCC1_IrqHandler,            /*!< 27 ADC controller1                        */
  HDMA_IrqHandler,             /*!< 28 HDMA                                   */
  UDPD_IrqHandler,             /*!< 29 USB Device High Speed UDP_HS           */
  IrqHandlerNotUsed            /*!< 30 not used                               */
};


/**
  * @brief  This is the code that gets called when the processor first
  *         starts execution following a reset event. Only the absolutely
  *         necessary set is performed, after which the application
  *         supplied main() routine is called. 
  * @param  None
  * @retval None
  */
void Reset_Handler(void)
{
  /* Initialize data and bss */
  unsigned long *pulSrc, *pulDest;

  /* Copy the data segment initializers from flash to SRAM */
  pulSrc = &_sidata;

  for(pulDest = &_sdata; pulDest < &_edata; )
  {
    *(pulDest++) = *(pulSrc++);
  }
  
  /* Zero fill the bss segment.  This is done with inline assembly since this
     will clear the value of pulDest if it is not kept in a register. */
  __asm("  ldr     r0, =_sbss\n"
        "  ldr     r1, =_ebss\n"
        "  mov     r2, #0\n"
        "  .thumb_func\n"
        "zero_loop:\n"
        "    cmp     r0, r1\n"
        "    it      lt\n"
        "    strlt   r2, [r0], #4\n"
        "    blt     zero_loop");
  
  /* Setup the microcontroller system */
  LowLevelInit();
  
  /* Call the application's entry point.*/
  main();
}

/*********************** (C) COPYRIGHT 2009 Coocox ************END OF FILE*****/