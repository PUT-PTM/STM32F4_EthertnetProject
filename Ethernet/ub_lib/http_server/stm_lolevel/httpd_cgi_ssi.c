/**
  ******************************************************************************
  * @file    httpd_cg_ssi.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Webserver SSI and CGI handlers
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


/*-----------------------------------------------------------------------*/
/* Angepasst von UB                                                      */
/* V:1.0 / 10.05.2013                                                    */
/*-----------------------------------------------------------------------*/


//--------------------------------------------------------------
// CGI-Funktionen
// USE_CGI_GPIO = um GPIO-Pins per HTML-Seite zu schalten
// USE_CGI_ADC  = um einen ADC auzulesen
//--------------------------------------------------------------
#define  USE_CGI_GPIO
#define  USE_CGI_ADC

//--------------------------------------------------------------
// Dieses File wertet CGI-Kommandos vom Browser auf
//
// Falls "USE_CGI_GPIO" = enable
//    werden 4 Pins aus GPIO-Ausgang definiert (PD12, PD13, PD14, PD15)
//    diese Ausgänge können per CGI-Kommando gesetzt werden
//
// Falls "USE_CGI_ADC" = enable
//    wird 1 Pin aus ADC-Eingang definiert (PA3)
//    dieser Eingang wird zyklisch gewandelt
//    und kann per CGI-Kommando ausgelesen werden
//    (CooCox-Lib : ADC, wird benötigt)
//--------------------------------------------------------------


/* Includes ------------------------------------------------------------------*/
#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "stm32_ub_http_server.h"

#ifdef USE_CGI_GPIO
  #include "stm32f4xx_gpio.h"
  #include "stm32f4xx_rcc.h"
#endif

#ifdef USE_CGI_ADC
  #include "stm32f4xx_adc.h"
#endif

#include <string.h>
#include <stdlib.h>

tSSIHandler ADC_Page_SSI_Handler;
uint32_t ADC_not_configured=1;
uint32_t GPIO_not_configured=1;

/* we will use character "t" as tag for CGI */
char const* TAGCHAR="t";
char const** TAGS=&TAGCHAR;

/* CGI handler for LED control */ 
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

/* Html request for "/leds.cgi" will start LEDS_CGI_Handler */
const tCGI LEDS_CGI={"/leds.cgi", LEDS_CGI_Handler};

/* Cgi call table, only one CGI used */
tCGI CGI_TAB[1];


/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */
#ifdef USE_CGI_ADC
static void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  // Clock Enable vom Pin
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Clock enable vom ADC
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  // Config des Pins als Analog-Eingang
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // ADC-Config
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 
  ADC_CommonInit(&ADC_CommonInitStructure); 

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC3, &ADC_InitStructure);

  // Messkanal einrichten
  ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 1, ADC_SampleTime_56Cycles);

  // ADC-Enable
  ADC_Cmd(ADC3, ENABLE);

  // Messung starten 
  ADC_SoftwareStartConv(ADC3);
}
#endif // USE_CGI_ADC

/**
  * @brief  ADC_Handler : SSI handler for ADC page 
  */
u16_t ADC_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
  /* We have only one SSI handler iIndex = 0 */
  if (iIndex ==0)
  {  
    #ifdef USE_CGI_ADC
    char Digit1=0, Digit2=0, Digit3=0, Digit4=0; 
    uint32_t ADCVal = 0;        

     // falls ADC noch nicht initialisiert
     if (ADC_not_configured ==1)       
     {
        // ADC initialisieren
        ADC_Configuration();
        ADC_not_configured=0;
     }
     
     // Messwert auslesen
     ADCVal = ADC_GetConversionValue(ADC3);
     
     // ADC-Wert in Volt umrechnen
     ADCVal = (uint32_t)(ADCVal * 0.8);  
     
     // die Zahl in 4 Ziffern umrechnen     
     Digit1= ADCVal/1000;
     Digit2= (ADCVal-(Digit1*1000))/100 ;
     Digit3= (ADCVal-((Digit1*1000)+(Digit2*100)))/10;
     Digit4= ADCVal -((Digit1*1000)+(Digit2*100)+ (Digit3*10));
        
     // Ziffern in Character umwandeln
     *pcInsert       = (char)(Digit1+0x30);
     *(pcInsert + 1) = (char)(Digit2+0x30);
     *(pcInsert + 2) = (char)(Digit3+0x30);
     *(pcInsert + 3) = (char)(Digit4+0x30);
    
    /* 4 characters need to be inserted in html*/
    #endif // USE_CGI_ADC
    return 4;
  }
  return 0;
}


#ifdef USE_CGI_GPIO
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  // Clock Enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  // Config als Digital-Ausgang
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}
#endif // USE_CGI_GPIO


/**
  * @brief  CGI handler for LEDs control 
  */
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  
  /* tylko jeden SSI handler iIndex = 0 */
  if (iIndex==0)
  {
    #ifdef USE_CGI_GPIO
    uint32_t i=0;

   // gdy nie jest zainicjowana konfiguracja GPIO
   if (GPIO_not_configured ==1)       
   {
     // GPIO inicjalizacja
     GPIO_Configuration();
     GPIO_not_configured=0;
   }

    // All GPIOs
    GPIOD->BSRRH = GPIO_Pin_12;
    GPIOD->BSRRH = GPIO_Pin_13;
    GPIOD->BSRRH = GPIO_Pin_14;
    GPIOD->BSRRH = GPIO_Pin_15;
    
    /* Check cgi parameter : example GET /leds.cgi?led=2&led=4 */
    for (i=0; i<iNumParams; i++)
    {
      /* GET "led" */
      if (strcmp(pcParam[i] , "led")==0)   
      {
        /* switch led1 ON if 1 */
        if(strcmp(pcValue[i], "1") ==0) {
          GPIOD->BSRRL = GPIO_Pin_12;
        }
          
        /* switch led2 ON if 2 */
        else if(strcmp(pcValue[i], "2") ==0) {
          GPIOD->BSRRL = GPIO_Pin_13;
        }
        
        /* switch led3 ON if 3 */
        else if(strcmp(pcValue[i], "3") ==0) {
          GPIOD->BSRRL = GPIO_Pin_14;
        }
        
        /* switch led4 ON if 4 */
        else if(strcmp(pcValue[i], "4") ==0) {
          GPIOD->BSRRL = GPIO_Pin_15;
        }
      }
    }

    #endif // USE_CGI_GPIO
  }
  /* adres po wykonaniu cgi*/
  return "/diody.html";
}

/**
 * Initialize SSI handlers
 */
void httpd_ssi_init(void)
{  
  /* configure SSI handlers (ADC page SSI) */
  http_set_ssi_handler(ADC_Handler, (char const **)TAGS, 1);
}

/**
 * Initialize CGI handlers
 */
void httpd_cgi_init(void)
{ 
  /* configure CGI handlers (LEDs control CGI) */
  CGI_TAB[0] = LEDS_CGI;
  http_set_cgi_handlers(CGI_TAB, 1);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
