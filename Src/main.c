/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "progui_fsm.h"
#include "lcd.h"
#include "keypad.h"
#include "flash/w25qxx.h"
#include "lcd_predefs.h"
#include "states.h"
#include "timer_iii.h"
#include "alarm.h"
#include <stdio.h>
#include <stdbool.h>
#include <usb_host.h>
#include <usbh_hid.h>
#include <stdarg.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// LCD Pinmaps
PortPin_Map LCD_RS = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_7 };
PortPin_Map LCD_RW = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_8 };
PortPin_Map LCD_E = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_9 };
PortPin_Map LCD_D4 = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_10 };
PortPin_Map LCD_D5 = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_11 };
PortPin_Map LCD_D6 = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_12 };
PortPin_Map LCD_D7 = { .GPIOx = GPIOE, .GPIO_Pin_x = GPIO_PIN_13 };
// Keypad Pinmaps
PortPin_Map KPD_ROW1 = { .GPIOx = GPIOA, .GPIO_Pin_x = GPIO_PIN_9 };
PortPin_Map KPD_ROW2 = { .GPIOx = GPIOA, .GPIO_Pin_x = GPIO_PIN_8 };
PortPin_Map KPD_ROW3 = { .GPIOx = GPIOC, .GPIO_Pin_x = GPIO_PIN_9 };
PortPin_Map KPD_ROW4 = { .GPIOx = GPIOC, .GPIO_Pin_x = GPIO_PIN_8 };
PortPin_Map KPD_COL1 = { .GPIOx = GPIOD, .GPIO_Pin_x = GPIO_PIN_14 };
PortPin_Map KPD_COL2 = { .GPIOx = GPIOD, .GPIO_Pin_x = GPIO_PIN_13 };
PortPin_Map KPD_COL3 = { .GPIOx = GPIOD, .GPIO_Pin_x = GPIO_PIN_12 };
PortPin_Map KPD_COL4 = { .GPIOx = GPIOD, .GPIO_Pin_x = GPIO_PIN_11 };
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */
enum SFSM System_FSM = SYS_DEINIT;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_RNG_Init(void);
static void MX_RTC_Init(void);
static void MX_UART4_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
void Main_LCD_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static int32_t uart_length = 0;
uint8_t uart_tx_buffer[128];
extern HID_MOUSE_Info_TypeDef mouse_info;
extern LCD_TypeDef *global_lcd;
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
  MX_I2C2_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  MX_UART4_Init();
  MX_USB_HOST_Init();
  MX_SPI3_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	// Turn off LED D2. D2 means a clock failure.
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	Main_LCD_Init();
	i2c_scanCabinets();
	System_FSM = SYS_IDLE;
	LCD_begin(global_lcd);
	scr_idle();
	start_monitor();

	keypad_init(KPD_ROW1, KPD_ROW2, KPD_ROW3, KPD_ROW4, KPD_COL1, KPD_COL2,
			KPD_COL3, KPD_COL4);

	HAL_DBGMCU_EnableDBGStopMode();
	W25qxx_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
		switch (System_FSM) {
		case SYS_IDLE:
			state_sys_idle();
			break;
		case SYS_IDLE_PWDWAIT:
			state_sys_auth();
			break;
		case SYS_IDLEAUTH_WRONGPWD:
			state_idleauth_wrongpwd();
			break;
		case SYS_MGMT:
			state_management();
			break;
		case SYS_OPEN_ALL_WAIT:
			state_openall();
			break;
		case SYS_OPEN_ALL_DONE:
			state_openall_done();
			break;
		case SYS_SETTINGS:
			state_settings();
			break;
		case SYS_RECONF:
			state_hwreconf();
			break;
		case SYS_RECONF_DETECT:
			state_hwreconf_detect();
			break;
		case SYS_RECONF_DONE:
			state_hwreconf_done();
			break;
		case SYS_RESET_PWD:
			state_mgmt_new_password();
			break;
		case SYS_CHOOSE_OP:
			state_choose_kbop();
			break;
		case SYS_DROP_PKG:
			state_pkg_drop();
			break;
		case SYS_DELI_WAIT_CHOICE:
			state_deli_wait_choice();
			break;
		case SYS_DELI_OKFAIL:
			state_deli_query();
			break;
		case SYS_DELI_IF_REPEAT:
			state_deli_ifrepeat();
			break;
		default:
			System_FSM = SYS_IDLE;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System 
  */
  HAL_RCC_EnableCSS();
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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

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
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1499;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 63999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  __TIM3_CLK_DISABLE();
  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1499;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 63999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE9 PE10 
                           PE11 PE12 PE13 PE14 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PD10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD11 PD12 PD13 PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
	uart_length = sprintf(uart_tx_buffer, (uint8_t*) "Event callback triggered");
	HAL_UART_Transmit(&huart4, uart_tx_buffer, (uint16_t) uart_length, 1000);
	HID_KEYBD_Info_TypeDef *keybd_info;
	uint8_t keycode;
	HID_HandleTypeDef *HID_Handle =
			(HID_HandleTypeDef*) phost->pActiveClass->pData;
	if (HID_Handle->Init == USBH_HID_KeybdInit) {
		keybd_info = USBH_HID_GetKeybdInfo(phost);
		keycode = USBH_HID_GetASCIICode(keybd_info);
		uart_length = sprintf(uart_tx_buffer, (uint8_t*) "Key Pressed: 0x%x\n", keycode);
		HAL_UART_Transmit(&huart4, uart_tx_buffer, (uint16_t) uart_length,
				1000);
	}
}

/*Clock Failure detection*/
void HAL_RCC_CSSCallback(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	printf("CLOCK STALLED");
	return;
}

/* External GPIO Interrupt. This should be keyboard. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	// Some software debouncing.
	/* Disable gpio_exti for a while */
	/* So no bouncing interrupt will be called. */
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	for(int i = 0; i<16000; i+=1){}
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	cancelTimer(SHDN_BKLT);
	LCD_BkltOn();


	printf("[GPIO_EXTI] Triggered.\n");
	uart_length = sprintf(uart_tx_buffer, (uint8_t*) "[GPIO_EXTI] GPIO_Pin=%d triggered.\n", GPIO_Pin);
	HAL_UART_Transmit(&huart4, uart_tx_buffer, (uint16_t)uart_length, 1000);
	if (GPIO_Pin == GPIO_PIN_10) {
		switch (System_FSM) {
		case SYS_IDLE:
			keyCache = KEY_FAKE;
			keyAvailable = false;
			// System_FSM = SYS_IDLE_PWDWAIT;
			System_FSM = SYS_CHOOSE_OP;
			return;
			break;
		case SYS_CHOOSE_OP:
			readOneKey();
			System_FSM = SYS_CHOOSE_OP_CALLBACK;
			break;
		case SYS_IDLE_PWDWAIT_KEYREADY:
			readOneKey();
			System_FSM = SYS_IDLE_PWDWAIT_CALLBACK;
			break;
		case SYS_IDLEAUTH_WRONGPWD:
			readOneKey();
			System_FSM = SYS_IDLEAUTH_WRONGPWD_CALLBACK;
			break;
		case SYS_MGMT:
			readOneKey();
			System_FSM = SYS_MGMT_CALLBACK;
			break;
		case SYS_SETTINGS:
			readOneKey();
			System_FSM = SYS_SETTINGS_CALLBACK;
			break;
		case SYS_RECONF:
			readOneKey();
			System_FSM = SYS_RECONF_CALLBACK;
			break;
		case SYS_RESET_PWD:
			readOneKey();
			System_FSM = SYS_RESET_PWD_CALLBACK;
			break;
		case SYS_DROP_PKG:
			readOneKey();
			System_FSM = SYS_DROP_PKG_CALLBACK;
			break;
		case SYS_DELI_OKFAIL:
			readOneKey();
			System_FSM = SYS_DELI_OKFAIL_CALLBACK;
			break;
		case SYS_DELI_IF_REPEAT:
			readOneKey();
			System_FSM = SYS_DELI_IF_REPEAT_CALLBACK;
			break;
		case SYS_ALARM:
			readOneKey();
			System_FSM = SYS_ALARM_CALLBACK;
			break;
		case SYS_IDLE_PWDWAIT_CALLBACK:
		case SYS_IDLEAUTH_WRONGPWD_CALLBACK:
		case SYS_MGMT_CALLBACK:
		case SYS_SETTINGS_CALLBACK:
		case SYS_RECONF_CALLBACK:
		case SYS_RESET_PWD_CALLBACK:
			return;
			break;
		case SYS_RECONF_DONE:
		case SYS_OPEN_ALL_DONE:
			keyAvailable = true;
			keyCache = KEY_FAKE;
			return;
			break;
		default:
			/* A key is pressed when not expected to. */
			/* Reset keyAvailable so keypad does not stuck. */
			keyAvailable = false;
			break;
		}

	}
}

/* Redirect the printf() output. */
int fputc(int c, FILE *f) {
	HAL_UART_Transmit(&huart4, (uint8_t *) &c, 1, 1000);
	return c;
}

int putchar(int c) {
	HAL_UART_Transmit(&huart4, (uint8_t *) &c, 1, 1000);
	return c;
}

int putc(int c, FILE *f) {
	HAL_UART_Transmit(&huart4, (uint8_t *) &c, 1, 1000);
	return c;
}

int puts(const char *c) {
	uart_length = sprintf(uart_tx_buffer, c);
	HAL_UART_Transmit(&huart4, uart_tx_buffer, (uint16_t) uart_length, 1000);
	return *c;
}

void debug_print(char* fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	uart_length = sprintf(uart_tx_buffer, fmt, &argptr);
	va_end(argptr);
	HAL_UART_Transmit(&huart4, uart_tx_buffer, (uint16_t) uart_length, 1000);
}

void Main_LCD_Init(void) {
	global_lcd = LCD_init(&LCD_RS, &LCD_RW, &LCD_E, &LCD_D4, &LCD_D5, &LCD_D6,
			&LCD_D7);
	if (global_lcd) {
		printf("[LCD] LCD Init successful.\n");
	} else {
		printf("[LCD] LCD Init returned NULL.\n");
	}
}

void send_serial(char* msg) {
	uart_length = sprintf(uart_tx_buffer, msg);
	HAL_UART_Transmit(&huart4, uart_tx_buffer, (uint16_t) uart_length, 1000);
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
