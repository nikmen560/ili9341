/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Network connection configuration
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

/* Includes ------------------------------------------------------------------*/
#include "mem.h"
#include "memp.h"
#include "dhcp.h"
#include "ethernetif.h"
#include "ethernet.h"
#include "netconf.h"
#include "tcpip.h"
#include "tcp.h"
#include "udp.h"
#include "etharp.h"
#include <stdio.h>
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	DHCP_START = 0,
	DHCP_WAIT_ADDRESS,
	DHCP_ADDRESS_ASSIGNED,
	DHCP_TIMEOUT
} DHCP_State_TypeDef;

/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif xnetif; /* network interface structure */
uint32_t TCPTimer = 0;
uint32_t ARPTimer = 0;

#ifdef USE_DHCP
	uint32_t DHCPfineTimer = 0;
	uint32_t DHCPcoarseTimer = 0;
	DHCP_State_TypeDef DHCP_state = DHCP_START;
#endif

/* Private functions ---------------------------------------------------------*/
#ifdef USE_DHCP
	void LwIP_DHCP_Process_Handle(void);
#endif

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init ( void ) {
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
#ifndef USE_DHCP
	uint8_t iptab[4];
	uint8_t iptxt[20];
	uint8_t msktxt[20];
	uint8_t porttxt[20];
#endif
#ifdef WEBSERVER
	/* Initializes the dynamic memory heap defined by MEM_SIZE.*/
	mem_init();

	/* Initializes the memory pools defined by MEMP_NUM_x.*/
	memp_init();
#else
	/* Create tcp_ip stack thread */
	tcpip_init ( NULL, NULL );
#endif

	/* IP address setting & display on STM32_evalboard LCD*/
#ifdef USE_DHCP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else
	IP4_ADDR ( &ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3 );
	IP4_ADDR ( &netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3 );
	IP4_ADDR ( &gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3 );

#ifdef USE_LCD_FOR_IP
#include "fonts.h"

	iptab[0] = IP_ADDR3;
	iptab[1] = IP_ADDR2;
	iptab[2] = IP_ADDR1;
	iptab[3] = IP_ADDR0;

	sprintf ( (char*) iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0] );

	LCD_SetTextColor ( LCD_COLOR_BLUE2 );
	LCD_DisplayStringLine ( LINE(5), (uint8_t*) "??????????? IP-?????" );
	LCD_DisplayStringLine ( LINE(6), iptxt );

	sprintf ( (char*) msktxt, "  %d.%d.%d.%d", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3 );

	LCD_SetTextColor ( LCD_COLOR_MAGENTA );
	LCD_DisplayStringLine ( LINE(7), (uint8_t*) "   ????? ???????    " );
	LCD_DisplayStringLine ( LINE(8), msktxt );

	LCD_SetTextColor ( LCD_COLOR_BLUE );
#ifdef TCP_SERVER
	sprintf ( (char*)porttxt,  "???? ???????????.%d  ", LISTEN_PORT );
#else
	sprintf ( (char*) porttxt, "???? ??????????  %d  ", DESTINATION_PORT );
#endif
	LCD_DisplayStringLine ( LINE(9), porttxt );
#endif
#endif

	/* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
	 struct ip_addr *netmask, struct ip_addr *gw,
	 void *state, err_t (* init)(struct netif *netif),
	 err_t (* input)(struct pbuf *p, struct netif *netif))

	 Adds your network interface to the netif_list. Allocate a struct
	 netif and pass a pointer to this structure as the first argument.
	 Give pointers to cleared ip_addr structures when using DHCP,
	 or fill them with sane numbers otherwise. The state pointer may be NULL.

	 The init function pointer must point to a initialization function for
	 your ethernet netif interface. The following code illustrates it's use.*/
#ifdef WEBSERVER
	netif_add ( &xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input );
#else
	netif_add ( &xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input );
#endif

	/*  Registers the default network interface. */
	netif_set_default ( &xnetif );

	/*  When the netif is fully configured this function must be called.*/
	netif_set_up ( &xnetif );
}

/**
  * @brief  Called when a frame is received
  * @param  None
  * @retval None
  */
void LwIP_Pkt_Handle(void) {
	/* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
	ethernetif_input ( &xnetif );
}

/**
  * @brief  LwIP periodic tasks
  * @param  localtime the current LocalTime value
  * @retval None
  */
void LwIP_Periodic_Handle(__IO uint32_t localtime) {

#if LWIP_TCP
	/* TCP periodic process every 250 ms */
	if (localtime - TCPTimer >= TCP_TMR_INTERVAL) {
		TCPTimer = localtime;
		tcp_tmr();
	}
#endif

	/* ARP periodic process every 5s */
	if ((localtime - ARPTimer) >= ARP_TMR_INTERVAL) {
		ARPTimer = localtime;
		etharp_tmr();
	}

#ifdef USE_DHCP
	/* Fine DHCP periodic process every 500ms */
	if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS) {
		DHCPfineTimer = localtime;
		dhcp_fine_tmr();
		if ( (DHCP_state != DHCP_ADDRESS_ASSIGNED)&&(DHCP_state != DHCP_TIMEOUT) ) {
			/* process DHCP state machine */
			LwIP_DHCP_Process_Handle();
		}
	}

	/* DHCP Coarse periodic process every 60s */
	if ( localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS ) {
		DHCPcoarseTimer = localtime;
		dhcp_coarse_tmr();
	}
#endif
}

#ifdef USE_DHCP
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void LwIP_DHCP_task ( void * pvParameters ) {
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	uint32_t IPaddress;
	uint8_t iptab[4];
	uint8_t iptxt[20];
	uint8_t DHCP_state;
	DHCP_state = DHCP_START;

	for (;;) {
		switch (DHCP_state) {
			case DHCP_START:
			{
				dhcp_start(&xnetif);
				IPaddress = 0;
				DHCP_state = DHCP_WAIT_ADDRESS;
#ifdef USE_LCD
				LCD_DisplayStringLine ( Line4, (uint8_t*)"     Looking for    " );
				LCD_DisplayStringLine ( Line5, (uint8_t*)"     DHCP server    " );
				LCD_DisplayStringLine ( Line6, (uint8_t*)"     please wait... " );
#endif
			}
			break;

			case DHCP_WAIT_ADDRESS:
			{
				/* Read the new IP address */
				IPaddress = xnetif.ip_addr.addr;

				if ( IPaddress! = 0 )
				{
					DHCP_state = DHCP_ADDRESS_ASSIGNED;

					/* Stop DHCP */
					dhcp_stop ( &xnetif );

#ifdef USE_LCD      
					iptab[0] = (uint8_t)(IPaddress >> 24);
					iptab[1] = (uint8_t)(IPaddress >> 16);
					iptab[2] = (uint8_t)(IPaddress >> 8);
					iptab[3] = (uint8_t)(IPaddress);

					sprintf ( (char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0] );

					LCD_ClearLine ( Line4 );
					LCD_ClearLine ( Line5 );
					LCD_ClearLine ( Line6 );
					/* Display the IP address */
					LCD_DisplayStringLine ( Line7, (uint8_t*)"IP address assigned " );
					LCD_DisplayStringLine ( Line8, (uint8_t*)"  by a DHCP server  " );
					LCD_DisplayStringLine ( Line9, iptxt );
#endif  
					vTaskDelete ( NULL );
				} else {
					/* DHCP timeout */
					if ( xnetif.dhcp->tries > MAX_DHCP_TRIES ) {
						DHCP_state = DHCP_TIMEOUT;

						/* Stop DHCP */
						dhcp_stop ( &xnetif );

						/* Static address used */
						IP4_ADDR ( &ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
						IP4_ADDR ( &netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3 );
						IP4_ADDR ( &gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3 );
						netif_set_addr ( &xnetif, &ipaddr , &netmask, &gw );

#ifdef USE_LCD   
						LCD_DisplayStringLine ( Line7, (uint8_t*)"    DHCP timeout    " );

						iptab[0] = IP_ADDR3;
						iptab[1] = IP_ADDR2;
						iptab[2] = IP_ADDR1;
						iptab[3] = IP_ADDR0;

						sprintf ( (char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0] );

						LCD_ClearLine ( Line4 );
						LCD_ClearLine ( Line5 );
						LCD_ClearLine ( Line6 );
						LCD_DisplayStringLine ( Line8, (uint8_t*)"  Static IP address   " );
						LCD_DisplayStringLine ( Line9, iptxt );
#endif
						vTaskDelete ( NULL );
					}
				}
			}
			break;

			default: break;
		}
		/* wait 250 ms */
		vTaskDelay ( 250 );
	}
}
#endif  /* USE_DHCP */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
