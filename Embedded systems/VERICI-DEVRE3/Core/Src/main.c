/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RFM300_Driver.h"
#include "rf_conf.h"
#include <string.h>
#include <stdio.h>
#include "sht3x.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  //  https://github.com/Sensirion/embedded-sht
 HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Basladi\r\n", 9, 100);
  sensirion_i2c_init();

  /*while (sht3x_probe(SHT3X_I2C_ADDR_DFLT) != STATUS_OK) {
    	printf("SHT sensor probing failed\n");
  }*/ // burayı aç
  printf("SHT sensor probing successful\n");

  #define TX_NUM  24
  byte tx_buf[32] = {'H', 'o', 'p', 'e', 'R', 'F', ' ', 'R', 'F', 'M', ' ', 'C', 'O', 'B', 'R', 'F', 'M', '3', '0', '0', 'A'};
  char buffer[23];
  char buffer2[100];
  //char buffer[100];
  char bufferkontrol[100];
  char sayac[100];
  char buffer_temperature[100];


  FixedPktLength    = 0;
  PayloadLength     = 23;
  vInit();
  vCfgBank(CMTBank, 12);
  vCfgBank(SystemBank, 12);
  vCfgBank(FrequencyBank, 8);
  vCfgBank(DataRateBank, 24);
  vCfgBank(BasebandBank, 29);
  vCfgBank(TXBank, 11);
  vEnableAntSwitch(0);
  vGpioFuncCfg(GPIO1_DCLK + GPIO2_DCLK + GPIO3_INT2); //GPIO Maping



  vIntSrcCfg(INT_FIFO_NMTY_TX, INT_TX_DONE);    //IRQ maping
  vIntSrcEnable(TX_DONE_EN);
  vClearFIFO();
  bGoSleep();

  HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Tx...\r\n", 7, 100);
int kontrol=1;
int c=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  int32_t temperature, humidity;
	  int8_t ret = sht3x_measure_blocking_read(SHT3X_I2C_ADDR_DFLT,&temperature, &humidity);

	  if (ret == STATUS_OK) {
		  sprintf(buffer, "%d", temperature/10); //100e bölünmesi demek ,'den sonraki 2 sayının silinmesi demek örneğin sonuç 255 yani 25.5 derece eğer ,'den sorna 2 hanenin gözükmesini istersek 10 diyeceğiz yani sonu. 25.57
	      HAL_UART_Transmit(&hlpuart1, "t: ", 3, 1000);
	      HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, strlen(buffer), 1000);
	      HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
	      //  printf("measured temperature: %0.2f degreeCelsius","measured humidity: %0.2f percentRH\n",temperature / 1000.0f, humidity / 1000.0f);






	      sprintf(buffer2, "%d", humidity/10);
	      HAL_UART_Transmit(&hlpuart1, "n: ", 3, 1000);
	      HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer2, strlen(buffer2), 1000);
	      HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);


	      strncat(buffer, buffer2, 5); //sondaki sayı eklenecek karakter sayısı
	      strncat(buffer,"A0000000002B", 12);
	      HAL_UART_Transmit(&hlpuart1, "buffer toplam: ", 15, 1000);
	      HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, strlen(buffer), 1000);
	      HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);

	      //if((bufferkontrol[0]!=buffer[0])||(bufferkontrol[1]!=buffer[1])||(bufferkontrol[2]!=buffer[2])||(bufferkontrol[3]!=buffer[3])){
	    	  bSendMessage(buffer,  strlen(buffer));
	    	  while (GPO3_L());
	    	  bIntSrcFlagClr();
	    	  vClearFIFO();
	    	  bGoSleep();
	    	  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"gonderildi\r\n",12, 100);
	    	  HAL_Delay(2000);


	      //}
	     // else HAL_UART_Transmit( &hlpuart1, (uint8_t *)"degısmedi\r\n",11, 100);

	      //strcpy(bufferkontrol,buffer);


	  }else
	  {
		  c++;
		 		  	    	sprintf(sayac, "%d", c);
		 		  	    	HAL_UART_Transmit(&hlpuart1, (uint8_t *)sayac, strlen(sayac), 1000);
		 		  	    	HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  int temperature1=-1111;
		  int humidity1=1111;

		  //HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  //HAL_UART_Transmit(&hlpuart1, "error",  5, 1000);



		  if(temperature1<-1000){


			  temperature1=temperature1*-1;
			  sprintf(buffer, "%d", temperature1);

			  HAL_UART_Transmit(&hlpuart1, "-10dan kucuk", 12, 1000);
			  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);


			  HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 5, 1000);
			  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);


			  temperature1=temperature1*-1;
		  }
		  else if(temperature1<0){

		  			temperature1=temperature1*-1;
		  			sprintf(buffer, "%d", temperature1);

		  			HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 5, 1000);
		  			HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);

		  			HAL_UART_Transmit(&hlpuart1, "0'la -10 arasında",18, 1000);
		  			HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  			temperature1=temperature1*-1;

		  		  }

		  else if(0==temperature1){

		  }else if(temperature1<1000){

			  HAL_UART_Transmit(&hlpuart1, "0 ile 10 arasinda", 17, 1000);
			  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
			  sprintf(buffer, "%d", temperature1);

			  		  			HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 15, 1000);
			  		  			HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);

		  }else if(temperature1>=1000){

			  HAL_UART_Transmit(&hlpuart1, "10'dan buyuk", 12, 1000);
			  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);

			  sprintf(buffer, "%d", temperature1); //100e bölünmesi demek ,'den sonraki 2 sayının silinmesi demek örneğin sonuç 255 yani 25.5 derece eğer ,'den sorna 2 hanenin gözükmesini istersek 10 diyeceğiz yani sonu. 25.57
			  HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, strlen(buffer), 1000);
			  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  }

		  	      //  printf("measured temperature: %0.2f degreeCelsius","measured humidity: %0.2f percentRH\n",temperature / 1000.0f, humidity / 1000.0f);


		  		  sprintf(buffer2, "%d", humidity1);

		  		  strncat(buffer, buffer2, 10); //sondaki sayı eklenecek karakter sayısı
		  		  HAL_UART_Transmit(&hlpuart1, "buffer: ", 9, 1000);
		  		  HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 4, 1000);
		  		  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  		  strncat(buffer, "7", 1); //pil durumu 1-6 arası kadameli olacak
		  		  HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 10, 1000);
		  		  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);



		  		if(temperature1<-1000){
		  			strncat(buffer, "-", 1); //+ - durumu
		  			HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 22, 1000);
		  			HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);


		  			HAL_UART_Transmit(&hlpuart1, "-10dan kucuk", 12, 1000);
		  			HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  		}else if(temperature1<0){
		  			strncat(buffer, "e-", 2); //+ - durumu
		  			HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 5, 1000);
		  			HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  		}
		  				  else if(0==temperature1){


		  				  }else if(temperature1<1000){
		  					strncat(buffer, "e+", 2);
		  					  HAL_UART_Transmit(&hlpuart1, "0 ile 10 arasinda", 17, 1000);
		  					  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);

		  				  }else if(temperature1>=1000){

		  					  HAL_UART_Transmit(&hlpuart1, "10'dan buyuk", 12, 1000);
		  					  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  					  strncat(buffer, "+", 1); //+ - durumu
		  					  HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 22, 1000);
		  					  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  				  }




		  	    strncat(buffer,"A0000000003B", 12);
		  	    HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 22, 1000);
		  	   	HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  	    //    strncat(buffer,"Asill000000B", 12);
		  	   // strncat(buffer,"Asil2000000B", 12);
		  	  //  strncat(buffer,"Asil2000000B", 12);
		  	  //   strncat(buffer,"Asil3000000B", 12);
		  	  //  strncat(buffer,"Asil4000000B", 12);
		  	  //  strncat(buffer,"Asil5000000B", 12);
		  	   // strncat(buffer,"Asil6000000B", 12);
		  	      HAL_UART_Transmit(&hlpuart1, "buffer toplam: ", 15, 1000);
		  	      HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 22, 1000);
		  	      HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);

		  	      //if((bufferkontrol[0]!=buffer[0])||(bufferkontrol[1]!=buffer[1])||(bufferkontrol[2]!=buffer[2])||(bufferkontrol[3]!=buffer[3])){
		  	    	  bSendMessage(buffer,  strlen(buffer));
		  	    	  while (GPO3_L());
		  	    	  bIntSrcFlagClr();
		  	    	  vClearFIFO();
		  	    	  bGoSleep();
		  	    	  HAL_UART_Transmit( &hlpuart1, (uint8_t *)"gonderildi\r\n",12, 100);
		  	    	HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n",2, 100);
		  	    	  HAL_Delay(4000);



		  	      //}
		  	     // else HAL_UART_Transmit( &hlpuart1, (uint8_t *)"degısmedi\r\n",11, 100);

		  	      //strcpy(buffer,bufferkontrol);
	  }






  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000708;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CSB_Pin|FCSB_Pin|SCLK_Pin|SDIO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CSB_Pin FCSB_Pin SCLK_Pin SDIO_Pin */
  GPIO_InitStruct.Pin = CSB_Pin|FCSB_Pin|SCLK_Pin|SDIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUZZER_Pin GPO3_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin|GPO3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
