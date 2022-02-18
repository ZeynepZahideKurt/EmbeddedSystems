/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 @file           : main.c
 @brief          : Main program body
 ******************************************************************************
 @attention

 <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 All rights reserved.</center></h2>

 This software component is licensed by ST under BSD 3-Clause license,
 the "License"; You may not use this file except in compliance with the
 License. You may obtain a copy of the License at:
 opensource.org/licenses/BSD-3-Clause

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
#define printf(...)  HAL_UART_Transmit((UART_HandleTypeDef *)&hlpuart1, (uint8_t *)u_buf,\
		                                sprintf((char*)u_buf,__VA_ARGS__), 0xFFFF);
uint8_t u_buf[256];
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
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC_Init(void);
static void MX_RTC_Init(void);
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
/*Bu kodda alıcıya 25 bit gönderiyoruz. Ama sayarken ilk biti 0. diye sayıyoruz
 * örneğin: 247244023294+A0000000001B
 * ilk 4 biti sıcaklık yani, şu anda +24.72 derece
 * sonraki 4 biti nem yani şu andan %44.02'lik bir nem var
 * sonraki 4 biti vericiye gelen voltaj mili volt cinsinden 3294 milivolt
 * sonraki kod gelen sıcaklık + mı - mi
 * sonraki A ile başlayıp B ile biten kod arasındaki numara seri numarası integer cinsinden
 *
 * örneğin: 94744023294e-A0000000001B
 * 11. bitinde e olması demek sıcaklığın tek haneli olması demek yani sadece 3bit dikkate alınır
 * ilk 3 biti sıcaklık yani, -9.47 derece
 * sonraki 4 biti nem yani şu andan %44.02'lik bir nem var
 * sonraki 4 biti vericiye gelen voltaj mili volt cinsinden 3294 milivolt
 * sonraki kod gelen sıcaklık + mı - mi
 * sonraki A ile başlayıp B ile biten kod arasındaki numara seri numarası integer cinsinden
 */
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
  MX_ADC_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	//  https://github.com/Sensirion/embedded-sht sht30 kodları kaynak
	sensirion_i2c_init();
	printf("Basladi\r\n");
	/*
	 * Orijinal kodda var ama biz sensör hatalıysa hata mesajı gönderiyoruz
	 * while (sht3x_probe(SHT3X_I2C_ADDR_DFLT) != STATUS_OK) {
	 * printf("SHT sensor probing failed\n"); }
	 */
	printf("SHT sensor probing successful\n");
    #define VREFINT_CAL_ADDR			0x1FF80078 //STM32L010C6Tx datasheet'inden bakıldı. Bu koda ve aşağıdaki Vrefint için yazıldı
    #define VREFINT_CAL 				((uint16_t*) VREFINT_CAL_ADDR)

    #define TX_NUM  25
	byte tx_buf[25] = { 'H', 'A', 'T', 'A', '1', '1', '1', '1', '1', '1', 'A', '0', '0', '0', '0', '0', '0', '0', '0', '0', '5', 'B' };
	// Sensör bozulursa veya çalışmazsa veya yanlış veri gelirse alıcıya hatalı mesajını gönderir (tx_buf[32]).
	char buffer[25];
	char buffer2[50];
	char buffer3[100];

	FixedPktLength = 0;
	PayloadLength = 23;
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

	//int c = 0; sayaç kullanmak istersen yorumu aç
	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED); // vrefint'ten ölçülecek voltajın düzgün ölçülmesi için yapılan kalibrasyon ayarı

	int32_t temperature, humidity;
	int8_t ret = sht3x_measure_blocking_read(SHT3X_I2C_ADDR_DFLT, &temperature,&humidity);
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) { //RTC ile saniye sayar o sürede stanby moduna alır. bekleme bittikten sonra bu fonksiyonun içindekileri yapar ve tekrar uyku moduna geçer.

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(3000); //yukarıdaki ve hal delay satırı kod yazarken aşamayı görmek için yazılmıştır sonradan silebilirsin

		//ADC'deki vrefint enable ettikten sonra aşağıdaki başlayan ve biten kodlarda cihaza gelen voltu ölçer
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc, 100);
		uint16_t VREFINTADC = HAL_ADC_GetValue(&hadc);
		HAL_ADC_Stop(&hadc);
		float VDD = 3000.0 * (*VREFINT_CAL / (float) VREFINTADC);
		//buraya kadar cihaza gelen vrefint'i ölçtü.

		temperature = temperature / 10;
		humidity = humidity / 10;
		if (ret == STATUS_OK) {
			/*Vericinin kaç kere gönderdiğinin alıcının kaç kere aldığını kontrol eden sayaç
			 * c++;
			 * printf("Sayaç: %d\r\n",c);
			 */

//Sıcaklık verisindne gelen veriler eğer -40 ile 100 derece arasında doğru kabul edilir ve alıcıya gönderilmek üzere aşağıdaki fonksiyona girer.
			if (-4000 <= temperature && temperature <= 9999 && 0 <= humidity&& humidity <= 9999) {

				if (temperature < -1000) {
					//printf("-10dan kucuk\r\n");
					// -1 ile çarptım çünkü -'li kısmı olmadan ilk 4 bitini göndermek istiyorum.
					temperature = temperature * -1;
					sprintf(buffer, "%d", temperature);
					//Tekrardan -1 ile çarptım çünkü temperature - mi + mı diye kontrol ederken geçek sıcaklığa ihtiyacım var
					temperature = temperature * -1;
				} else if (temperature < 0) {
					// -1 ile çarptım çünkü -'li kısmı olmadan ilk 4 bitini göndermek istiyorum.
					temperature = temperature * -1;
					sprintf(buffer, "%d", temperature);
					//printf("0'la -10 arasında\r\n");
					//Tekrardan -1 ile çarptım çünkü temperature - mi + mı diye kontrol ederken geçek sıcaklığa ihtiyacım var
					temperature = temperature * -1;

				} else if (temperature < 1000) {

					//printf("0 ile 10 arasinda\r\n");
					sprintf(buffer, "%d", temperature);

				} else if (temperature >= 1000) {
					sprintf(buffer, "%d", temperature); //Temperature/100 demedik çünkü ilk 4 bitini de alıcıya göndermek istiyoruz.100e bölünmesi demek ,'den sonraki 2 sayının silinmesi demek örneğin sonuç 255 yani 25.5 derece eğer ,'den sorna 2 hanenin gözükmesini istersek 10 diyeceğiz yani sonu. 25.57
					//printf("10'dan buyuk\r\n");

				}

				//  printf("measured temperature: %0.2f degreeCelsius","measured humidity: %0.2f percentRH\n",temperature / 1000.0f, humidity / 1000.0f);

				sprintf(buffer2, "%d", humidity);

				strncat(buffer, buffer2, 4); //sondaki sayı eklenecek karakter sayısı
				//printf("buffer: %s\r\n",buffer);

				/*   if(VDD<=3600 && VDD>3240){
				 pil=5;
				 HAL_UART_Transmit(&hlpuart1, "pil=5", 12, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 strncat(buffer, "5", 1); //pil durumu 1-5 arası kadameli olacak
				 HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 10, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 }else if(VDD<=3240 && VDD>2880){
				 pil=4;
				 HAL_UART_Transmit(&hlpuart1, "pil=4", 12, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 strncat(buffer, "4", 1); //pil durumu 1-5 arası kadameli olacak
				 HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 10, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 }else if(VDD<=2880 && VDD>2520){
				 pil=3;
				 HAL_UART_Transmit(&hlpuart1, "pil=3", 12, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 strncat(buffer, "3", 1); //pil durumu 1-5 arası kadameli olacak
				 HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 10, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 }else if(VDD<=2520 && VDD>2160){
				 pil=2;
				 HAL_UART_Transmit(&hlpuart1, "pil=2", 12, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 strncat(buffer, "2", 1); //pil durumu 1-5 arası kadameli olacak
				 HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 10, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 }else if(VDD<2160){
				 pil=1;
				 HAL_UART_Transmit(&hlpuart1, "pil=1", 12, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 strncat(buffer, "1", 1); //pil durumu 1-5 arası kadameli olacak
				 HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 10, 1000);
				 HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				 }*/

				//Buffer'a pil durumunu ekliyoruz
				int vddint = (int) VDD;
				sprintf(buffer3, "%d", vddint);
				strncat(buffer, buffer3, 4); //sondaki sayı eklenecek karakter sayısı


				//Buffer'a sıcaklığın - mi + mı durumunu ekliyoruz
				if (temperature < -1000) {
					strncat(buffer, "-", 1); //+ - durumu
					//printf("-10dan kucuk");

				} else if (temperature < 0) {
					strncat(buffer, "e-", 2); //+ - durumu
					//printf("0 ile -10 arasinda");

				} else if (temperature < 1000) {
					strncat(buffer, "e+", 2);
					//printf("0 ile 10 arasinda");

				} else if (temperature >= 1000) {
					strncat(buffer, "+", 1); //+ - durumu
					//printf("10'dan buyuk \r\n");
					//printf("buffer: %s\r\n",buffer);
				}
				//strncat(buffer, "s", 2);

				//Buffer'a seri numarasını ekliyoruz
				strncat(buffer, "A0000000002B", 12); //seri numarası 10 haneli A ve B arasında
				//HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, 22, 1000);
				//HAL_UART_Transmit( &hlpuart1, (uint8_t *)"\r\n", 2, 100);
				  //   strncat(buffer,"Asill000000B", 12);
				// strncat(buffer,"Asil2000000B", 12);
				//  strncat(buffer,"Asil2000000B", 12);
				//   strncat(buffer,"Asil3000000B", 12);
				//  strncat(buffer,"Asil4000000B", 12);
				//  strncat(buffer,"Asil5000000B", 12);
				// strncat(buffer,"Asil6000000B", 12);

				printf("buffer toplam: %s\r\n", buffer);
				/*
				 sıcaklığın ilk 3 veya 4 biti değişirse göndermesini istersen kullan:
				 if((bufferkontrol[0]!=buffer[0])||(bufferkontrol[1]!=buffer[1])||(bufferkontrol[2]!=buffer[2])||(bufferkontrol[3]!=buffer[3])){
				 if((bufferkontrol[0]!=buffer[0])||(bufferkontrol[1]!=buffer[1])||(bufferkontrol[2]!=buffer[2])||(bufferkontrol[3]!=buffer[3]))
				 else HAL_UART_Transmit( &hlpuart1, (uint8_t *)"degısmedi\r\n",11, 100);
				 strcpy(bufferkontrol,buffer);
				 */

				//Bufferı gönderiyioruz sonrasında CMT2119 B transmitterı uykuya sokuyoruz
				bSendMessage(buffer, strlen(buffer));
				while (GPO3_L());
				bIntSrcFlagClr();
				vClearFIFO();
				bGoSleep();
				printf("gonderildi\r\n");
				// HAL_Delay(4000);

			} else {
				//Eger sensorden yanlis veri gelirse veya sensor bozulursa aliciya hatali mesajini gönderir
				printf("gonderilmedi\r\n");
				bSendMessage(tx_buf, 22);
				while (GPO3_L())
					;
				bIntSrcFlagClr();
				vClearFIFO();
				bGoSleep();

			}

		} else {
			//Eger sensorden yanlis veri gelirse veya sensor bozulursa aliciya hatali mesajini gönderir
							printf("gonderilmedi\r\n");
							bSendMessage(tx_buf, 22);
							while (GPO3_L())
								;
							bIntSrcFlagClr();
							vClearFIFO();
							bGoSleep();

		}

		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0
		/** Deactivate the RTC wakeup  **/
		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	}

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
	printf("About to enter the STANDBY MODE\r\n");

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x5A55, RTC_WAKEUPCLOCK_RTCCLK_DIV16)
			!= HAL_OK) {
		Error_Handler();
	}
	printf("STANDBY MODE is ON\r\n");
	GPIO_ALL_DisEnable(); //Güç harcamaması için bütün çevre birimlerini kapatıyoruz

	HAL_PWR_EnterSTANDBYMode(); //uyku moduna alıyoruz

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = ENABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

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
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 124;
  hrtc.Init.SynchPrediv = 295;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 10, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
void GPIO_ALL_DisEnable(void) {
	/*   GPIO_InitTypeDef GPIO_InitStruct;
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	 __HAL_RCC_GPIOB_CLK_ENABLE();
	 __HAL_RCC_GPIOC_CLK_ENABLE();
	 __HAL_RCC_GPIOH_CLK_ENABLE();
	 GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	 GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 GPIO_InitStruct.Pin = GPIO_PIN_All;
	 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	 HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
	 */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_13);
	HAL_GPIO_DeInit(GPIOA, SDIO_Pin);
	HAL_GPIO_DeInit(GPIOA, GPO3_Pin);
	HAL_GPIO_DeInit(GPIOA, BUZZER_Pin);
	HAL_GPIO_DeInit(GPIOA, CSB_Pin);
	HAL_GPIO_DeInit(GPIOA, FCSB_Pin);
	HAL_GPIO_DeInit(GPIOA, SCLK_Pin);
	//HAL_RTC_DeInit(&hrtc);
	HAL_UART_DeInit(&hlpuart1);
	HAL_I2C_DeInit(&hi2c1);
	HAL_ADC_DeInit(&hadc);
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
	while (1) {
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
