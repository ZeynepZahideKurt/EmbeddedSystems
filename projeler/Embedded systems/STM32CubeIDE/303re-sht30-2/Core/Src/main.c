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

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
#define SHT30_I2C 						    I2C1

#define SHT30_I2C_ADDR                     (0x44)

#define SHT30_I2C_SCL_PIN                  (GPIO_Pin_6)           /* PB.06 */
#define SHT30_I2C_SCL_GPIO_PORT            (GPIOB)

#define SHT30_I2C_SDA_PIN                  (GPIO_Pin_7)           /* PB.07 */
#define SHT30_I2C_SDA_GPIO_PORT            (GPIOB)

#define I2C_SPEED               100000U

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  SHT3X_GetTempAndHumi(&temperature, &humidity, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	          printf("temperature=%.1f℃\thumidity=%.1f%%RH\terror=%d\r\n",(float)temperature,(float)humidity,(u8)error);

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
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
  hi2c1.Init.Timing = 0x2000090E;
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
etError SHT3X_GetTempAndHumi(ft* temperature, ft* humidity,etRepeatability repeatability, etMode mode, u8t timeout)
{
    etError error;

    switch(mode)
    {
        case MODE_CLKSTRETCH: // get temperature with clock stretching mode
            error = SHT3X_GetTempAndHumiClkStretch(temperature, humidity,repeatability, timeout);
            break;
        case MODE_POLLING:	// get temperature with polling mode
            error = SHT3X_GetTempAndHumiPolling(temperature, humidity,repeatability, timeout);
            break;
        default:
            error = PARM_ERROR;
            break;
    }

    return error;
}



etError SHT3X_GetTempAndHumiClkStretch(ft* temperature, ft* humidity,etRepeatability repeatability, u8t timeout)
{
    etError error;	// error code
    u16t	rawValueTemp; // temperature raw value from sensor
    u16t	rawValueHumi; // humidity raw value from sensor

    error = SHT3X_StartWriteAccess();

    // if no error ...
    if(error == NO_ERROR)
    {
        // start measurement in clock stretching mode
        // use depending on the required repeatability, the corresponding command


        switch(repeatability)
        {
            case REPEATAB_LOW:
                error = SHT3X_WriteCommand(CMD_MEAS_CLOCKSTR_L);
                break;
            case REPEATAB_MEDIUM:
                error = SHT3X_WriteCommand(CMD_MEAS_CLOCKSTR_M);
                break;
            case REPEATAB_HIGH:
                error = SHT3X_WriteCommand(CMD_MEAS_CLOCKSTR_H);
                break;
            default:
                error = PARM_ERROR;
                break;
        }
    }

    // if no error, start read access
    if(error == NO_ERROR) error = SHT3X_StartReadAccess();
    // if no error, read temperature raw values
    if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, timeout);
    // if no error, read humidity raw values
    if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);
    //SHT3X_StopAccess();
    // if no error, calculate temperature in °C and humidity in %RH
    if(error == NO_ERROR)
    {
        *temperature = SHT3X_CalcTemperature(rawValueTemp);
        *humidity = SHT3X_CalcHumidity(rawValueHumi);
    }
    else
	{
        SHT3X_StopAccess();
	}

    return error;
}


static etError SHT3X_StartWriteAccess(void)
{
    etError error = NO_ERROR; // error code

	/*!< While the bus is busy */
	SHT30_Timeout = SHT30_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(SHT30_I2C, I2C_FLAG_BUSY))
    {
	    if((SHT30_Timeout--) == 0)
		{
			 error = TIMEOUT_ERROR;
			 return error;
		}
	}

    I2C_AcknowledgeConfig(SHT30_I2C, ENABLE);

    //I2c_StartCondition();
    // write a start condition
    I2C_GenerateSTART(SHT30_I2C, ENABLE);

	 /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
	 /**
		* @brief  Communication start
		*
		* After sending the START condition (I2C_GenerateSTART() function) the master
		* has to wait for this event. It means that the Start condition has been correctly
		* released on the I2C bus (the bus is free, no other devices is communicating).
		*
		*/
	SHT30_Timeout = SHT30_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(SHT30_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
	    if((SHT30_Timeout--) == 0)
		{
			 error = TIMEOUT_EV5_ERROR;
			 return error;
		}
	}

    // write the sensor I2C address with the write flag
    //error = I2c_WriteByte(_i2cAddress << 1);
    //发送地址后，等待从机确认 EV6
	I2C_Send7bitAddress(SHT30_I2C, (SHT30_I2C_ADDR << 1), I2C_Direction_Transmitter);

	/*!< Test on EV6 and clear it */
	SHT30_Timeout = SHT30_LONG_TIMEOUT;
    while(!I2C_CheckEvent(SHT30_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	    if((SHT30_Timeout--) == 0)
		{
			error = TIMEOUT_EV6_ERROR;
			return error;
		}
	}

    return error;
}


static etError SHT3X_WriteCommand(etCommands command)
{
    etError error = NO_ERROR; // error code

    // write the upper 8 bits of the command to the sensor
    //error	= I2c_WriteByte(command >> 8);
	I2C_SendData(SHT30_I2C,command >> 8);
    /* --EV8 */
	SHT30_Timeout = SHT30_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(SHT30_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
         if((SHT30_Timeout--) == 0)
		 {
		      error = TIMEOUT_EV8_ERROR;
			  return error;
		}
	}


    // write the lower 8 bits of the command to the sensor
    //error |= I2c_WriteByte(command & 0x00FF);
	I2C_SendData(SHT30_I2C,command & 0x00FF);
	SHT30_Timeout = SHT30_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(SHT30_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))//EV8_2
	{
	    if((SHT30_Timeout--) == 0)
		{
			error = TIMEOUT_EV8_ERROR;
		    return error;
		}
	}

    I2C_GenerateSTOP(SHT30_I2C, ENABLE);//主发送器通信完成

    return error;
}

static etError SHT3X_StartReadAccess(void)
{
    etError error; // error code

    // write a start condition
    //I2c_StartCondition();


   /*!< While the bus is busy */
   SHT30_Timeout = SHT30_LONG_TIMEOUT;
   while(I2C_GetFlagStatus(SHT30_I2C, I2C_FLAG_BUSY))
   {
	   if((SHT30_Timeout--) == 0)
		{
			 error = TIMEOUT_ERROR;
			 return error;
		}
   }


    // write a start condition
    //I2c_StartCondition();
	//EV5:After sending the START condition (I2C_GenerateSTART() function) the master
    //has to wait for this event.
    I2C_GenerateSTART(SHT30_I2C, ENABLE);

	 /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
	SHT30_Timeout = SHT30_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(SHT30_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
	    if((SHT30_Timeout--) == 0)
			{
				 error = TIMEOUT_EV5_ERROR;
			   return error;
			}
	  }

      // write the sensor I2C address with the write flag
      //error = I2c_WriteByte(_i2cAddress << 1);
	  //发送地址后，等待从机确认 EV6
	  I2C_Send7bitAddress(SHT30_I2C, (SHT30_I2C_ADDR << 1), I2C_Direction_Receiver);

		/*!< Test on EV6 and clear it */
	  SHT30_Timeout = SHT30_FLAG_TIMEOUT;
	  while(!I2C_CheckEvent(SHT30_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	  {
		  if((SHT30_Timeout--) == 0)
		  {
			   error = TIMEOUT_EV6_ERROR;
			   return error;
		  }
	 }

    // write the sensor I2C address with the read flag
    //error = I2c_WriteByte(_i2cAddress << 1 | 0x01);

    return error;
}


static etError SHT3X_Read2BytesAndCrc(u16t* data, etI2cAck finaleAckNack,u8t timeout)
{


    etError error;	// error code
    u8t	bytes[2]; // read data array
    u8t	checksum; // checksum byte

    // read two data bytes and one checksum byte
    error = I2c_ReadByte(&bytes[0], ACK, timeout);
    if(error == NO_ERROR) error = I2c_ReadByte(&bytes[1], ACK, 0);
    if(error == NO_ERROR) error = I2c_ReadByte(&checksum, finaleAckNack, 0);

    // verify checksum
    if(error == NO_ERROR) error = SHT3X_CheckCrc(bytes, 2, checksum);

    // combine the two bytes to a 16-bit value
    *data = (bytes[0] << 8) | bytes[1];

    return error;
}



etError I2c_ReadByte(u8t *rxByte, etI2cAck ack, u8t timeout)
{
    etError error = NO_ERROR;

	__set_PRIMASK(1);//关总中断

    if(ack == ACK)
	{
		I2C_AcknowledgeConfig(SHT30_I2C, ENABLE);
	}
	else
	{
	    //关闭通信:
	    /* 主设备在从 从设备 接收到最后一个字节后发送一个NACK。接收到NACK后，从设备
		 * 释放对SCL和SDA线的控制;主设备就可以发送一个停止/重起始条件.
		 * 1:为了在收到最后一个字节后产生NACK脉冲，在读倒数第二个字节之后必须清除ACK位.
		 * 2:为了产生一个停止/重起始条件,软件必须在读第二个数据字节之后设置STOP/START位.
		  */
	   I2C_AcknowledgeConfig(SHT30_I2C, DISABLE);
       (void)SHT30_I2C->SR2;
       I2C_GenerateSTOP(SHT30_I2C, ENABLE);
	}

	//从机端接收到主机地址后，开始向主机发送数据
	/*!< Test on EV7 */
	SHT30_Timeout = SHT30_LONG_TIMEOUT;
	while(!I2C_CheckEvent(SHT30_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
		if((SHT30_Timeout--) == 0)
		{
			error = TIMEOUT_EV7_ERROR;
			return error;
		}
    }
    *rxByte  = I2C_ReceiveData(SHT30_I2C);

	 if(ack == NACK)
	 {
		/* Wait to make sure that STOP control bit has been cleared */
		//STOP位：监测到停止条件时，由硬件清除。
		SHT30_Timeout = SHT30_FLAG_TIMEOUT;
		while(SHT30_I2C->CR1 & I2C_CR1_STOP)
		{
			if((SHT30_Timeout--) == 0)
			{
				error = TIMEOUT_ERROR;
				return error;
			}
		}
	}
	__set_PRIMASK(0);//开总中断
    return error;		//return with no error
}


static etError SHT3X_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum)
{
    u8t crc;	// calculated checksum

    // calculates 8-Bit checksum
    crc = SHT3X_CalcCrc(data, nbrOfBytes);

    // verify checksum
    if(crc != checksum) return CHECKSUM_ERROR;
    else	return NO_ERROR;
}


static void SHT3X_StopAccess(void)
{
	 I2C_DeInit(SHT30_I2C);
	 SHT30_Init(SHT30_I2C_ADDR);
}

void SHT30_Init(uint8_t i2cAddress)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	/*配置时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SHT30_I2C_SCL_PIN|SHT30_I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(SHT30_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*!< SHT30_I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_DeInit(I2C1);

    /*!< I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = i2cAddress;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C1, ENABLE);
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
