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
#include <stm32l0xx_hal.h>
#include "type.h"
#include "spi.h"
#include "CMT2300drive.h"
#include "stdio.h"
#include "TxConfig.h"
#include "RxConfig.h"

static unsigned char statetx = true;  //  false为RX  true为TX
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
IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define LEN 21

unsigned char str[LEN] = {"feng rf test!!!!feng!"};
unsigned char getstr[LEN+1];
cmt2300aEasy radio;
void CLK_Init(void);
  void setup_Tx(void);
  void setup_Rx(void);
  void CMT2119B_TxInit();
  void CMT2219B_RxInit();
  void loop_Tx(void);
  void loop_Rx(void);
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
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  /*P3M1&=~(1<1);
  	P3M0|=(1<<1);
  	P3M1&=~(1<5);
  	P3M0|=(1<<5);
  	UART_INIT();
  	EA = 1;*/
  	printf("start0!\r\n");

    if(false == statetx )
  	{
  		CMT2219B_RxInit();
  		setup_Rx();
  		while(1)
  		{
  				loop_Rx();
  		}
  	}
    else
  	{
  		CMT2119B_TxInit();
  		setup_Tx();
  		while (1)
  		{
  			 loop_Tx();
  		}
  	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, csb_Pin|fcsb_Pin|buzzer_Pin|SCLK_Pin
                          |SDIO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : csb_Pin fcsb_Pin buzzer_Pin SCLK_Pin
                           SDIO_Pin */
  GPIO_InitStruct.Pin = csb_Pin|fcsb_Pin|buzzer_Pin|SCLK_Pin
                          |SDIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void CMT2119B_TxInit()
{
	/**********基础设置初始化一次即可*******/
	radio.FixedPktLength    = false;
	radio.PayloadLength     = LEN;
	cmt2300aEasy_vInit();

	cmt2300aEasy_vCfgBank(CMTBank, 12);
	cmt2300aEasy_vCfgBank(SystemBank, 12);
	cmt2300aEasy_vCfgBank(FrequencyBank, 8);
	cmt2300aEasy_vCfgBank(DataRateBank, 24);
	cmt2300aEasy_vCfgBank(BasebandBank, 29);
	cmt2300aEasy_vCfgBank(TXBank, 11);

	cmt2300aEasy_vEnablePLLcheck();
	cmt2300aEasy_bGoSleep();  				//让配置生效
	/**************************************/
}

void CMT2219B_RxInit()
{
	/**********基础设置初始化一次即可*******/
	radio.FixedPktLength    = false;
	radio.PayloadLength     = LEN;
	cmt2300aEasy_vInit();

		cmt2300aEasy_vCfgBank(CMT_Bank, sizeof(CMT_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(System_Bank, sizeof(System_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Frequency_Bank, sizeof(Frequency_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Data_Rate_Bank, sizeof(Data_Rate_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Baseband_Bank, sizeof(Baseband_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(Reserve_Bank, sizeof(Reserve_Bank)/sizeof(unsigned short int));
		cmt2300aEasy_vCfgBank(LBD_Bank, sizeof(LBD_Bank)/sizeof(unsigned short int));

	cmt2300aEasy_vEnablePLLcheck();
	cmt2300aEasy_bGoSleep();  				//让配置生效
	/**************************************/
}

void setup_Tx(void)
{

	if(cmt2300aEasy_bGoStandby())//;   //进入配置模式
	{
		printf("bGoStandby ok!\r\n");
	}
	cmt2300aEasy_vEnableAntSwitch(0);  //设置天线切换_IO口切换
	cmt2300aEasy_vGpioFuncCfg(GPIO1_INT2+GPIO2_INT2+GPIO3_INT2); //IO口的映射
	cmt2300aEasy_vIntSrcCfg(INT_FIFO_NMTY_TX, INT_TX_DONE);    //IO口中断的映射
	cmt2300aEasy_vIntSrcEnable(TX_DONE_EN);           //中断使能

	cmt2300aEasy_vClearFIFO();  //清除FIFO
	if(cmt2300aEasy_bGoSleep())//;    //进入睡眠，让配置生效
	{
		printf("bGoSleep ok!\r\n");;
	}

}

void setup_Rx(void)
{
	cmt2300aEasy_bGoStandby();   //进入配置模式
	cmt2300aEasy_vEnableAntSwitch(0); //为 1 时 GPIO1 和 GPIO2 不可用
	//cmt2300aEasy_vGpioFuncCfg(GPIO1_INT1+GPIO2_Dout+GPIO3_DCLK);  //IO口的功能映射

	//cmt2300aEasy_vIntSrcCfg(INT_RSSI_VALID, INT_CRC_PASS);   //GPO3映射成CRC_pass中断，此处如果要用该中断，RFPDK需要配置CRC
	cmt2300aEasy_vIntSrcCfg(INT_PKT_DONE, INT_PKT_DONE);  //GPO3映射成PKT_DONE中断 //IO口中断的映射
	cmt2300aEasy_vIntSrcEnable(PKT_DONE_EN /*+ CRC_PASS_EN*/);          //中断使能

	cmt2300aEasy_vClearFIFO();
	cmt2300aEasy_bGoSleep();           //让配置生效
	cmt2300aEasy_bGoRx();              //for Rx
}


void loop_Tx()
{
	printf("send!\r\n");
	cmt2300aEasy_bSendMessage(str, LEN);
  	//while(GPO3_L());   // 判断GPIO中断 为低等 为高运行下面代码
	cmt2300aEasy_bIntSrcFlagClr();
	cmt2300aEasy_vClearFIFO();
	HAL_Delay(20);
}

unsigned char tmp;
void loop_Rx()
{

	//if(GPO1_H())
	//{
		cmt2300aEasy_bGoStandby();
		tmp = cmt2300aEasy_bGetMessage(getstr);
		printf("recv=%s\r\n",getstr);
		cmt2300aEasy_bIntSrcFlagClr();
		cmt2300aEasy_vClearFIFO();
		cmt2300aEasy_bGoRx();
	//}else
	//{
		printf("nothing!\r\n");
		HAL_Delay(1000);

	//}
}

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
