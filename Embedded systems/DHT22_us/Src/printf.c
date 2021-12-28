#include <stm32l4xx_hal.h>
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stddef.h"

extern UART_HandleTypeDef huart2;

int HAL_Printf(const char *fmt, ...)
{
  int m;
  char str[512] = {0};
  va_list va;
  va_start(va,fmt);
  m = vsnprintf(str, sizeof(str)-1, fmt, va);
  va_end(va);
  str[m+1] = '\0';
  HAL_UART_Transmit(&huart2,(uint8_t*)str,strlen(str),0xFFFF);
  return m;
}
