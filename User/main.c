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
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int  main(void)
{
  /* 初始化电源 */
  STM_EVAL_PWRInit();

  /* 使能电源 */
  STM_EVAL_PWROn();

  /* Unlock the Flash Program Erase controller */
  FLASH_If_Init();

  /* 检测应用程序参数保存区升级标志 */
  if (0 == IAP_FlagCheck())
  {
    /* Initialize Leds mounted on STM32_PDA_EVAL board */
    STM_EVAL_LEDInit(LED_PWR);
    STM_EVAL_LEDInit(LED_UART);

    /* Turn on PWR's LED */
    STM_EVAL_LEDOn(LED_PWR);

    /* Turn off UART's LED */
    STM_EVAL_LEDOff(LED_UART); 
    
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
