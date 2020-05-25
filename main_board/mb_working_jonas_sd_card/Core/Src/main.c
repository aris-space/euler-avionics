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
#include "cmsis_os.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tasks/task_controller.h"
#include "tasks/task_sens_read.h"
#include "tasks/task_state_est.h"
#include "tasks/task_motor_control.h"
#include "tasks/task_fsm.h"
#include "util.h"
#include "Sensor_Fusion_Helper/env.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SD_HandleTypeDef hsd1;
DMA_HandleTypeDef hdma_sdmmc1_rx;
DMA_HandleTypeDef hdma_sdmmc1_tx;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart7;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for task_state_est */
osThreadId_t task_state_estHandle;
uint32_t task_sfBuffer[ 2048 ];
osStaticThreadDef_t task_sfControlBlock;
const osThreadAttr_t task_state_est_attributes = {
  .name = "task_state_est",
  .stack_mem = &task_sfBuffer[0],
  .stack_size = sizeof(task_sfBuffer),
  .cb_mem = &task_sfControlBlock,
  .cb_size = sizeof(task_sfControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_controller */
osThreadId_t task_controllerHandle;
uint32_t task_controllerBuffer[ 2048 ];
osStaticThreadDef_t task_controllerControlBlock;
const osThreadAttr_t task_controller_attributes = {
  .name = "task_controller",
  .stack_mem = &task_controllerBuffer[0],
  .stack_size = sizeof(task_controllerBuffer),
  .cb_mem = &task_controllerControlBlock,
  .cb_size = sizeof(task_controllerControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_sens_read */
osThreadId_t task_sens_readHandle;
uint32_t task_sensor_reaBuffer[ 2048 ];
osStaticThreadDef_t task_sensor_reaControlBlock;
const osThreadAttr_t task_sens_read_attributes = {
  .name = "task_sens_read",
  .stack_mem = &task_sensor_reaBuffer[0],
  .stack_size = sizeof(task_sensor_reaBuffer),
  .cb_mem = &task_sensor_reaControlBlock,
  .cb_size = sizeof(task_sensor_reaControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_sd_card */
osThreadId_t task_sd_cardHandle;
uint32_t task_sd_cardBuffer[ 2048 ];
osStaticThreadDef_t task_sd_cardControlBlock;
const osThreadAttr_t task_sd_card_attributes = {
  .name = "task_sd_card",
  .stack_mem = &task_sd_cardBuffer[0],
  .stack_size = sizeof(task_sd_cardBuffer),
  .cb_mem = &task_sd_cardControlBlock,
  .cb_size = sizeof(task_sd_cardControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_motor_cont */
osThreadId_t task_motor_contHandle;
uint32_t task_motor_contBuffer[ 2048 ];
osStaticThreadDef_t task_motor_contControlBlock;
const osThreadAttr_t task_motor_cont_attributes = {
  .name = "task_motor_cont",
  .stack_mem = &task_motor_contBuffer[0],
  .stack_size = sizeof(task_motor_contBuffer),
  .cb_mem = &task_motor_contControlBlock,
  .cb_size = sizeof(task_motor_contControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tas_fsm */
osThreadId_t tas_fsmHandle;
uint32_t tas_fsmBuffer[ 2048 ];
osStaticThreadDef_t tas_fsmControlBlock;
const osThreadAttr_t tas_fsm_attributes = {
  .name = "tas_fsm",
  .stack_mem = &tas_fsmBuffer[0],
  .stack_size = sizeof(tas_fsmBuffer),
  .cb_mem = &tas_fsmControlBlock,
  .cb_size = sizeof(tas_fsmControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
baro_data_t sb1_baro = { 0 };
imu_data_t sb1_imu = { 0 };
sb_data_t sb1_data = { 0 };
baro_data_t sb2_baro = { 0 };
imu_data_t sb2_imu = { 0 };
sb_data_t sb2_data = { 0 };
baro_data_t sb3_baro = { 0 };
imu_data_t sb3_imu = { 0 };
sb_data_t sb3_data = { 0 };
state_est_data_t state_est_data = { 0 };
env global_env = { 0 };
flight_phase_detection_t global_flight_phase_detection = { 0 };
osMutexId_t environment_mutex;
osMutexId_t fsm_mutex;
osMutexId_t state_est_mutex;
osMessageQueueId_t log_queue;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI3_Init(void);
static void MX_SPI1_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_SPI2_Init(void);
static void MX_UART7_Init(void);
void StartDefaultTask(void *argument);
extern void vTaskStateEst(void *argument);
extern void vTaskController(void *argument);
extern void vTaskSensRead(void *argument);
extern void vTaskSdCard(void *argument);
extern void vTaskMotorCont(void *argument);
extern void vTaskFSM(void *argument);

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
  (DBGMCU)->APB1FZ = 0x7E01BFF;
  (DBGMCU)->APB2FZ = 0x70003;
  init_env(&global_env);
  reset_flight_phase_detection(&global_flight_phase_detection);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();
  MX_SDMMC1_SD_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_UART7_Init();
  /* USER CODE BEGIN 2 */
  MX_USB_DEVICE_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* Sensor Board 1 Mutex */
   const osMutexAttr_t sb1_mutex_attr = {
     "sb1_mutex",                              // human readable mutex name
     osMutexPrioInherit,                       // attr_bits
     NULL,                                     // memory for control block
     0U                                        // size for control block
   };

   sb1_mutex = osMutexNew(&sb1_mutex_attr);

   /* Sensor Board 2 Mutex */
    const osMutexAttr_t sb2_mutex_attr = {
      "sb2_mutex",                              // human readable mutex name
      osMutexPrioInherit,                       // attr_bits
      NULL,                                     // memory for control block
      0U                                        // size for control block
    };

    sb2_mutex = osMutexNew(&sb2_mutex_attr);

    /* Sensor Board 3 Mutex */
     const osMutexAttr_t sb3_mutex_attr = {
       "sb3_mutex",                              // human readable mutex name
       osMutexPrioInherit,    					 // attr_bits
       NULL,                                     // memory for control block
       0U                                        // size for control block
     };

     sb3_mutex = osMutexNew(&sb3_mutex_attr);

     /* State Estimation Output Mutex */
      const osMutexAttr_t state_est_mutex_attr = {
        "state_est_mutex",                              // human readable mutex name
        osMutexPrioInherit,    					 // attr_bits
        NULL,                                     // memory for control block
        0U                                        // size for control block
      };

      state_est_mutex = osMutexNew(&state_est_mutex_attr);

      /* FSM Output Mutex */
       const osMutexAttr_t fsm_mutex_attr = {
         "fsm_mutex",                              // human readable mutex name
         osMutexPrioInherit,    					 // attr_bits
         NULL,                                     // memory for control block
         0U                                        // size for control block
       };

       fsm_mutex = osMutexNew(&fsm_mutex_attr);

       /* Environment Mutex */
        const osMutexAttr_t environment_mutex_attr = {
          "environment_mutex",                              // human readable mutex name
          osMutexPrioInherit,    					 // attr_bits
          NULL,                                     // memory for control block
          0U                                        // size for control block
        };

        environment_mutex = osMutexNew(&environment_mutex_attr);

#ifdef DEBUG
  const osMutexAttr_t print_mutex_attr = {
	  "print_mutex",                            // human readable mutex name
	  osMutexPrioInherit,    					  // attr_bits
	  NULL,                                     // memory for control block
	  0U                                        // size for control block
	};

  print_mutex = osMutexNew(&print_mutex_attr);
#endif
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  log_queue = osMessageQueueNew(LOG_QUEUE_SIZE, sizeof(log_entry_t), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of task_state_est */
  task_state_estHandle = osThreadNew(vTaskStateEst, NULL, &task_state_est_attributes);

  /* creation of task_controller */
  task_controllerHandle = osThreadNew(vTaskController, NULL, &task_controller_attributes);

  /* creation of task_sens_read */
  task_sens_readHandle = osThreadNew(vTaskSensRead, NULL, &task_sens_read_attributes);

  /* creation of task_sd_card */
  task_sd_cardHandle = osThreadNew(vTaskSdCard, NULL, &task_sd_card_attributes);

  /* creation of task_motor_cont */
  task_motor_contHandle = osThreadNew(vTaskMotorCont, NULL, &task_motor_cont_attributes);

  /* creation of tas_fsm */
  tas_fsmHandle = osThreadNew(vTaskFSM, NULL, &tas_fsm_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
 
  /* We should never get here as control is now taken by the scheduler */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART7|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_SLAVE;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  hspi3.Init.Mode = SPI_MODE_SLAVE;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 115200;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
