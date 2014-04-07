//--------------------------------------------------------------
// File     : main.c
// Datum    : 12.05.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Funktion : Demo der HTTP-Server Library
// Hinweis  : Diese zwei Files muessen auf 8MHz stehen
//              "cmsis_boot/stm32f4xx.h"
//              "cmsis_boot/system_stm32f4xx.c"
//--------------------------------------------------------------

#include "main.h"
#include "stm32_ub_http_server.h"
#include "stm32_ub_led.h"

int main(void)
{
  HTTP_SERVER_STATUS_t check;

  SystemInit(); // Quarz Einstellungen aktivieren

  // LEDs initialisieren
  UB_Led_Init();

  // HTTP-Server initialisieren
  check=UB_HTTP_Server_Init();
  if(check==HTTP_SERVER_OK) {
    // wenn Server ok
    UB_Led_On(LED_GREEN);
  }
  else {
    // bei einem Fehler
    UB_Led_On(LED_RED);
  }

  while(1)
  {
	// HTTP-Server zyklisch aufrufen
	UB_HTTP_Server_Do();
  }
}
