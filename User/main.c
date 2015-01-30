/**
  ******************************************************************************
  * @file    STM32F2xx_IAP/src/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    02-May-2011
  * @brief   Main program body
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

/** 
 * @file     main.c
 * @brief    STM32F2xx-IAP-UART
 * @details  STM32F2xx in-application programming using the USART (AN3374)
 * @author   华兄
 * @email    591881218@qq.com
 * @date     2015
 * @version  vX.XX
 * @par Copyright (c):  
 *           华兄电子
 * @par History:          
 *   version: author, date, desc\n 
 */  

/** @addtogroup STM32F2xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "flash_if.h"
#include "stm32_eval.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(uint32_t BaudRate);
static uint32_t IAP_FlagCheck(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int  main(void)
{
  /* Unlock the Flash Program Erase controller */
  FLASH_If_Init();

  /* Initialize Leds mounted on STM32_PDA_EVAL board */
  STM_EVAL_LEDInit(LED_PWR);
  STM_EVAL_LEDInit(LED_UART);

  /* Turn on PWR's LED */
  STM_EVAL_LEDOn(LED_PWR);

  /* Turn off UART's LED */
  STM_EVAL_LEDOff(LED_UART); 

  /* 检测应用程序参数保存区升级标志 */
  if (0 == IAP_FlagCheck())
  { 
    /* Execute the IAP driver in order to reprogram the Flash */
    IAP_Init(UART_BAUD_RATE); /* Default BaudRate: 115200bps */
    /* Display main menu */
    Main_Menu();
  }
  /* Keep the user application running */
  else
  {
    FLASH_If_JumpToApplication();
  }

  while (1)
  {}
}

/**
  * @brief  Initialize the IAP: Configure USART.
  * @param  None
  * @retval None
  */
static void IAP_Init(uint32_t BaudRate)
{
  USART_InitTypeDef USART_InitStructure;

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
}

// ---------------------------------------------------------

unsigned int CalcCRC(unsigned char *buf, unsigned int crc)
{
    unsigned char i, chk;

    
    crc = crc ^ *buf;
    
    for(i = 0; i < 8; i++)
    {        
        chk = crc & 1;
        
        crc = crc >> 1;
        
        crc = crc & 0x7fff;
        
        if(1 == chk)
        {
            crc = crc ^ 0xa001;
        }
        
        crc = crc & 0xffff;
    }
    
    return (crc);
}

unsigned int Get_Checksum(unsigned char *buf, unsigned short len)
{
    unsigned char *ptr;
    unsigned char high, low; 
    unsigned int i, crc;  


    ptr = buf;
    crc = 0xffff; 
    
    for(i = 0; i < len; i++) 
    { 
        crc = CalcCRC(ptr, crc); 
        
        ptr++; 
    } 
    
    high = crc % 256; 
    low = crc / 256; 
    crc = (high << 8) | low; 
    
    return (crc); 
}

// ---------------------------------------------------------

static uint32_t IAP_FlagCheck(void)
{ 
  uint16_t len;
  uint32_t *p_prm;
  uint32_t crc, addr, data, n = APPLICATION_PRM_COUNT;

    
  while(n)
  {
    n--;

    p_prm = (uint32_t *)(APPLICATION_PRM_ADDRESS + n * APPLICATION_PRM_SIZE);

    if(APPLICATION_PRM_TAG == p_prm[0])
    {
      crc = Get_Checksum((uint8_t *)&p_prm[3], (uint16_t)(APPLICATION_PRM_SIZE - 12));

      if(crc == p_prm[2])
      {
        addr = APPLICATION_PRM_ADDRESS + n * APPLICATION_PRM_SIZE + IAP_FLAG_OFFSET;
        data = IAP_FLAG_FINISH;
        len = 1;
        
        if(IAP_FLAG_REQUEST == *(uint32_t *)addr)
        {       
          /* 清除应用程序参数保存区升级标志 */
          FLASH_If_Write((uint32_t *)&addr, (uint32_t *)&data, len);

          return (0);
        }
        else
        {
          break;
        }
      }
    }
  }

  return (1);
}

// ---------------------------------------------------------

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
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
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
