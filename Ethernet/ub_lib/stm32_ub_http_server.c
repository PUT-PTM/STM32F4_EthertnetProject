//--------------------------------------------------------------
// File     : stm32_ub_http_server.c
// Datum    : 10.05.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO, SYSCFG, EXTI, MISC
// Funktion : Ethernet HTTP-Server (per PHY : DP83848C)
//            IP-Stack = lwIP (V:1.3.2)
//
// Hinweis  : benutzt wird die RMII-Schnittstelle
//
//            PA1  = RMII_Ref_Clk       PC1 = ETH_MDC
//            PA2  = ETH_MDIO           PC4 = RMII_RXD0
//            PA7  = RMII_CRS_DV        PC5 = RMII_RXD1
//            PB11 = RMII_TX_EN
//            PB12 = RMII_TXD0          NRST = Reset
//            PB13 = RMII_TXD1
//            PB14 = RMII_INT
//
// Webseite : Die HTML-Webseiten wurde mit dem Programm
//            "makefsdata.exe" in ein C-File umgewandelt
//            und im Projekt als "fsdata.c" im Flash gespeichert
//            => diese File darf NICHT mitcompiliert werden !!
//
// CGI      : die CGI-Funktionen werden im File
//            "httpd_cgi_ssi.c" freigegeben und programmiert
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_http_server.h"


//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
volatile uint32_t LocalTime = 0;



//--------------------------------------------------------------
// Init vom HTTP Server
// Return wert :
//      HTTP_SERVER_OK => Server wurde gestartet
//   != HTTP_SERVER_OK => Fehler, Server nicht gestartet
//--------------------------------------------------------------
HTTP_SERVER_STATUS_t UB_HTTP_Server_Init(void)
{
  HTTP_SERVER_STATUS_t ret_wert=HTTP_SERVER_OK;
  uint32_t check=0;

  // Ethernet init
  check=ETH_BSP_Config();
  if(check==1) ret_wert=HTTP_SERVER_ETH_MACDMA_ERR;
  if(check==2) ret_wert=HTTP_SERVER_ETH_PHYINT_ERR;

  // Init vom LwIP-Stack
  LwIP_Init();

  // HTTP init
  httpd_init();

  return(ret_wert);
}

//--------------------------------------------------------------
// HTTP Server
// diese Funktion muss zyklisch aufgerufen werden
//--------------------------------------------------------------
void UB_HTTP_Server_Do(void)
{
  // test ob ein Packet angekommen ist
  if (ETH_CheckFrameReceived())
  {
    // empfangenes Packet auswerten
    LwIP_Pkt_Handle();
  }
  // LwIP zyklisch aufrufen
  LwIP_Periodic_Handle(LocalTime);
}


//--------------------------------------------------------------
// Systick (wird alle 10ms aufgerufen)
//--------------------------------------------------------------
void SysTick_Handler(void)
{
  // incrementiert den Counter
  LocalTime += 10;  // +10 weil Timerintervall = 10ms
}


//--------------------------------------------------------------
// ISR von Ext-Interrupt (PB14)
//--------------------------------------------------------------
void EXTI15_10_IRQHandler(void)
{
  // check welcher Ext-Interrupt (10 bis 15) aufgetreten ist
  if(EXTI_GetITStatus(ETH_LINK_EXTI_LINE) != RESET)
  {
    // wenn es der Interrupt vom LINK-Status war

    // Interrupt Handler starten
    Eth_Link_ITHandler(DP83848_PHY_ADDRESS);
    // ISR-Flag löschen
    EXTI_ClearITPendingBit(ETH_LINK_EXTI_LINE);
  }
}

