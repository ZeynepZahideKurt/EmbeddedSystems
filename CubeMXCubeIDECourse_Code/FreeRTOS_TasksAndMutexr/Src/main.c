/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"


UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId Thread1Handle;
osThreadId Thread2Handle;

osThreadId Thread3Handle;

osMutexId uart_mutexHandle;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

void StartDefaultTask(void const * argument);
void Thhread1Func(void const * argument);
void Thread2Func(void const * argument);
void Thread3Func(void const * argument);


typedef  uint32_t  TaskProfiler;

TaskProfiler Thread_1_Profiler,Thread_2_Profiler,Thread_3_Profiler,Default_Thread_Profiler;


int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  osMutexDef(uart_mutex);
  uart_mutexHandle = osMutexCreate(osMutex(uart_mutex));

  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Thread1 */
  osThreadDef(Thread1, Thhread1Func, osPriorityNormal, 0, 128);
  Thread1Handle = osThreadCreate(osThread(Thread1), NULL);

  /* definition and creation of Thread3 */
  osThreadDef(Thread3, Thread3Func, osPriorityNormal, 0, 128);
  Thread3Handle = osThreadCreate(osThread(Thread3), NULL);

  /* definition and creation of Thread2 */
  osThreadDef(Thread2, Thread2Func, osPriorityNormal, 0, 128);
  Thread2Handle = osThreadCreate(osThread(Thread2), NULL);

  osKernelStart();

  while (1)
  {

  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }


}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  //Default_Thread_Profiler++;
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

void Thread3Func(void const * argument)
{

	uint8_t message[24] = "Greetings from Thread3\r\n";
    uint32_t notifyValue;
	while(1){
		//Thread_3_Profiler++;

		 xTaskNotifyWait(pdFALSE,0xFF,&notifyValue,portMAX_DELAY);

		 if((notifyValue & 0x01) != 0x00){

			  xSemaphoreTake(uart_mutexHandle,portMAX_DELAY);

			  HAL_UART_Transmit(&huart2,message,24,10);

			  xSemaphoreGive(uart_mutexHandle);

		 }



	}

}

void Thhread1Func(void const * argument)
{
  /* USER CODE BEGIN Thhread1Func */
  /* Infinite loop */
  uint8_t message[24] = "Greetings from Thread1\r\n";
  for(;;)
  {
	  //Thread_1_Profiler++;
	 // HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);

	  xSemaphoreTake(uart_mutexHandle,portMAX_DELAY);

	  HAL_UART_Transmit(&huart2,message,24,10);

	  xSemaphoreGive(uart_mutexHandle);

      osDelay(10);
  }
  /* USER CODE END Thhread1Func */
}


/* USER CODE END Header_Thread2Func */
void Thread2Func(void const * argument)
{
  /* USER CODE BEGIN Thread2Func */
  /* Infinite loop */
	  uint8_t message[24] = "Greetings from Thread2\r\n";

  for(;;)
  {
	  //Thread_2_Profiler++;
	  if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)== GPIO_PIN_RESET){

		  xTaskNotify(Thread3Handle,0x01,eSetBits);

		  xSemaphoreTake(uart_mutexHandle,portMAX_DELAY);

		  HAL_UART_Transmit(&huart2,message,24,10);

		  xSemaphoreGive(uart_mutexHandle);

	  }


      osDelay(10);
  }
  /* USER CODE END Thread2Func */
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
