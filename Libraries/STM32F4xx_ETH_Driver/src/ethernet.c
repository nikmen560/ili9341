/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include "sys.h"
#include "api.h"
#include "inet.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include "stm32f4_discovery.h"
#include "netconf.h"
#include "ethernet.h"
#include "tcpip.h"
#include "sockets.h"
#include "netdb.h"

#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*--------------- Tasks Priority -------------*/
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 2 )      
#define LED_TASK_PRIO    ( tskIDLE_PRIORITY + 1 )
#define APP_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )
#define EMPTYMESSAGE   "                    "

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern struct netif xnetif;
__IO uint32_t test;
char EthBuf[4096];
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
void ToggleLed4(void * pvParameters)
{
  while (1)
  {   
    test = xnetif.ip_addr.addr;
    /*check if IP address assigned*/
    if (test !=0) {
      for( ;; ) {
        /* toggle LED4 each 250ms */
        STM_EVAL_LEDToggle(LED4);
        vTaskDelay(250);
      }
    }
  }
}

static void app_thread(void *arg) {

	struct netconn *conn, *newconn;
	err_t err;

	LWIP_UNUSED_ARG(arg);

	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	if (conn != NULL ) {
#ifdef TCP_SERVER
		/* Bind connection to well known port number 7. */
		err = netconn_bind(conn, NULL, LISTEN_PORT);

		if (err == ERR_OK) {
			/* Tell connection to go into listening mode. */
			netconn_listen(conn);

			while (1) {
				/* Grab new connection. */
				newconn = netconn_accept(conn);

				/* Process the new connection. */
				if (newconn) {
					struct netbuf *buf;
					void *data;
					u16_t len;

					while ((buf = netconn_recv(newconn)) != NULL ) {
						do {
							netbuf_data(buf, &data, &len);
							memcpy(EthBuf, data, len);
							EthBuf[len] = '\0';
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_DisplayStringLine(Line4, (uint8_t*) EMPTYMESSAGE);
							LCD_DisplayStringLine(Line4, (uint8_t*) EthBuf);

						} while (netbuf_next(buf) >= 0);

						netbuf_delete(buf);
					}

					/* Close connection and discard connection identifier. */
					netconn_close(newconn);
					netconn_delete(newconn);
				}
			}
		} else {
			printf(" can not bind TCP netconn");
			while(1);
		}
#else
		struct in_addr address;
		struct ip_addr ipaddr;
		inet_aton(DESTINATION_IP, &address);
		ipaddr.addr = address.s_addr;
		/* Bind connection to well known port number 7. */
		err = netconn_connect(conn, &ipaddr, DESTINATION_PORT);

		if (err == ERR_OK) {

			char str[] = "Message";
			netconn_write(conn, str, sizeof(str), NETCONN_COPY);

			while (1) {

					struct netbuf *buf;
					void *data;
					u16_t len;

					while ((buf = netconn_recv(conn)) != NULL ) {
						do {
							netbuf_data(buf, &data, &len);
							memcpy(EthBuf, data, len);
							EthBuf[len] = '\0';
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_DisplayStringLine(Line4, (uint8_t*) EMPTYMESSAGE);
							LCD_DisplayStringLine(Line4, (uint8_t*) EthBuf);

						} while (netbuf_next(buf) >= 0);

						netbuf_delete(buf);
					}
				}
		} else {
			printf(" can not bind TCP netconn");
			while(1);
		}
#endif
	} else {
		printf("can not create TCP netconn");
		while(1);
	}
}

void AppInit(void) {
	sys_thread_new("my_app", app_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, APP_THREAD_PRIO);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void TCP_UDP_echoInit(void) {

	/* configure ethernet (GPIOs, clocks, MAC, DMA) */
	ETH_BSP_Config();

	/* Initilaize the LwIP stack */
	LwIP_Init();

	AppInit();

#ifdef USE_DHCP
	/* Start DHCPClient */
	xTaskCreate(LwIP_DHCP_task, (int8_t*)"DHCPClient", configMINIMAL_STACK_SIZE * 2, NULL,DHCP_TASK_PRIO, NULL);
#endif

	/* Start toogleLed4 task : Toggle LED4  every 250ms */
	xTaskCreate(ToggleLed4, (int8_t*)"LED4", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);

}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
