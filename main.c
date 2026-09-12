/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pcf8574_lcd1602_linuslim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PCF8574_ADDRESS 0x27
#define PCF8574_ADDRESS_SHIFTED PCF8574_ADDRESS << 1

#define C13_TOGGLE_DELAY 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
uint32_t C13_LastToggle = 0;

LCD1602_HandleTypeDef MyLCD1602;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void LIM_LED_BLINK(GPIO_TypeDef* GPIO, uint16_t PIN, uint32_t* LastToggle, uint32_t ToggleDelay) {
	if(HAL_GetTick() - *LastToggle >= ToggleDelay) {
		HAL_GPIO_TogglePin (GPIOC,GPIO_PIN_13);
		*LastToggle = HAL_GetTick();
	}
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	/*
	P0 → RS
	P1 → RW
	P2 → E
	P3 → Back light
	P4 → D4
	P5 → D5
	P6 → D6
	P7 → D7
	*/

	uint8_t Message_A_Upper_Nibble_Enable_Low  = 0b01001001;
	uint8_t Message_A_Upper_Nibble_Enable_High = 0b01001101;
	uint8_t Message_A_Lower_Nibble_Enable_Low  = 0b00011001;
	uint8_t Message_A_Lower_Nibble_Enable_High = 0b00011101;
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
  /* USER CODE BEGIN 2 */

  MyLCD1602.hi2c = &hi2c1;
  MyLCD1602.address = PCF8574_ADDRESS_SHIFTED;

  LCD1602_Initialization_4BitsMode(&MyLCD1602);

  HAL_I2C_Master_Transmit(&hi2c1, MyLCD1602.address, &Message_A_Upper_Nibble_Enable_Low, 1, HAL_MAX_DELAY);
  HAL_I2C_Master_Transmit(&hi2c1, MyLCD1602.address, &Message_A_Upper_Nibble_Enable_High, 1, HAL_MAX_DELAY);
  HAL_I2C_Master_Transmit(&hi2c1, MyLCD1602.address, &Message_A_Upper_Nibble_Enable_Low, 1, HAL_MAX_DELAY);

  HAL_I2C_Master_Transmit(&hi2c1, MyLCD1602.address, &Message_A_Lower_Nibble_Enable_Low, 1, HAL_MAX_DELAY);
  HAL_I2C_Master_Transmit(&hi2c1, MyLCD1602.address, &Message_A_Lower_Nibble_Enable_High, 1, HAL_MAX_DELAY);
  HAL_I2C_Master_Transmit(&hi2c1, MyLCD1602.address, &Message_A_Lower_Nibble_Enable_Low, 1, HAL_MAX_DELAY);

  HAL_Delay(1000);

  LCD1602_Send_Data(&MyLCD1602,'B');

  HAL_Delay(1000);

  LCD1602_Set_Cursor(&MyLCD1602, 1, 8);
  LCD1602_Send_Data(&MyLCD1602,'C');

  HAL_Delay(1000);

  LCD1602_Set_Cursor(&MyLCD1602, 2, 15);
  LCD1602_Send_Data(&MyLCD1602,'D');

  HAL_Delay(1000);

  LCD1602_Clear_Display(&MyLCD1602);

  LCD1602_Send_Data(&MyLCD1602,'E');
  LCD1602_Send_Data(&MyLCD1602,'F');

  LCD1602_Set_Cursor(&MyLCD1602, 2, 1);
  LCD1602_Send_String(&MyLCD1602,"Hello LCD1602");

  HAL_Delay(1000);

  LCD1602_Clear_Display(&MyLCD1602);


  uint32_t Timer = 0;
  uint8_t i = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  LIM_LED_BLINK(GPIOC, GPIO_PIN_13, &C13_LastToggle, C13_TOGGLE_DELAY);
	  if(HAL_GetTick() - Timer >= 1000)
	  {
		  LCD1602_Set_Cursor(&MyLCD1602, 1, 1);
		  LCD1602_Printf(&MyLCD1602, "Seconds: %d", ++i);
		  LCD1602_Set_Cursor(&MyLCD1602, 2, 1);
		  LCD1602_Printf(&MyLCD1602, "Multiple: %d", i*2);
		  Timer = HAL_GetTick();
	  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
#ifdef USE_FULL_ASSERT
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
