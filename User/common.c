/**
  ******************************************************************************
  * @file    STM32F2xx_IAP/src/common.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    02-May-2011
  * @brief   This file provides all the common functions.
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

/** @addtogroup STM32F2xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "flash_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t IAP_FlagAddr = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The integer
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t * num)
{
  uint8_t inputstr[16];

  while (1)
  {
    GetInputString(inputstr);
    if (inputstr[0] == '\0') continue;
    if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
    {
      SerialPutString("User Cancelled\r\n");
      return 0;
    }

    if (Str2Int(inputstr, num) == 0)
    {
      SerialPutString("Error, Input again: \r\n");
    }
    else
    {
      return 1;
    }
  }
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t SerialKeyPressed(uint8_t *key)
{

  if ( USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET)
  {
    /* Turn on UART's LED */
    STM_EVAL_LEDOn(LED_UART);
    
    *key = (uint8_t)EVAL_COM1->DR;

    /* Turn off UART's LED */
    STM_EVAL_LEDOff(LED_UART);
    
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t GetKey(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (SerialKeyPressed((uint8_t*)&key)) break;
  }
  return key;

}

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void SerialPutChar(uint8_t c)
{
  /* Turn on UART's LED */
  STM_EVAL_LEDOn(LED_UART);
    
  USART_SendData(EVAL_COM1, c);
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE) == RESET)
  {
  }

  /* Turn off UART's LED */
  STM_EVAL_LEDOff(LED_UART);  
}

/**
  * @brief  Print a string on the HyperTerminal
  * @param  s: The string to be printed
  * @retval None
  */
void Serial_PutString(uint8_t *s)
{
  while (*s != '\0')
  {
    SerialPutChar(*s);
    s++;
  }
}

/**
  * @brief  Get Input string from the HyperTerminal
  * @param  buffP: The input string
  * @retval None
  */
void GetInputString (uint8_t * buffP)
{
  uint32_t bytes_read = 0;
  uint8_t c = 0;
  do
  {
    c = GetKey();
    if (c == '\r')
      break;
    if (c == '\b') /* Backspace */
    {
      if (bytes_read > 0)
      {
        SerialPutString("\b \b");
        bytes_read --;
      }
      continue;
    }
    if (bytes_read >= CMD_STRING_SIZE )
    {
      SerialPutString("Command string size overflow\r\n");
      bytes_read = 0;
      continue;
    }
    if (c >= 0x20 && c <= 0x7E)
    {
      buffP[bytes_read++] = c;
      SerialPutChar(c);
    }
  }
  while (1);
  SerialPutString(("\n\r"));
  buffP[bytes_read] = '\0';
}

/**
  * @brief  STM32 soft reset
  * @param  None
  * @retval None
  */
void STM32_SoftReset(void)
{
  __set_FAULTMASK(SET);
    
  NVIC_SystemReset();
}

/**
  * @brief  Initialize the IAP: Configure USART
  * @param  None
  * @retval None
  */
void IAP_Init(uint32_t BaudRate)
{
  USART_InitTypeDef USART_InitStructure;

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  /* USART configured as follow:
        - BaudRate = BaudRate  
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

uint32_t IAP_FlagCheck(void)
{ 
  uint32_t *p_prm;
  uint32_t crc, addr, n = APPLICATION_PRM_COUNT;

    
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
        
        if(IAP_FLAG_REQUEST == *(uint32_t *)addr)
        {       
          IAP_FlagAddr = addr;
          
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

uint32_t IAP_FlagClear(void)
{
  uint16_t len;
  uint32_t addr, data;


  if (0 == IAP_FlagAddr)
  {
    return (1);
  }

  addr = IAP_FlagAddr;
  data = IAP_FLAG_FINISH;
  len = 1;

  return (FLASH_If_Write((uint32_t *)&addr, (uint32_t *)&data, len));
}

// ---------------------------------------------------------

/**
  * @}
  */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
