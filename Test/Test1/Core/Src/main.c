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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "hhMPU6050.h"
#include "hhI2C.h"
#include "hhMotor.h"
#include "hhEncoderSpeed.h"
#include "Serial.h"
#include "math.h"
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

/* USER CODE BEGIN PV */
  uint16_t KeyNum=0;
  uint16_t Num=0;
  int16_t i=0;
  int16_t AX,AY,AZ,GX,GY,GZ;
  int16_t PWML,PWMR;
  float SpeedL,SpeedR;
  float AngleAcc;//加速度传感器测得的角度
  float AngleGyro;//陀螺仪测得的角度
  float Angle;//互补滤波后的角度
  uint16_t TimerCount=0;
  uint8_t RunFlag=0;//运行标志位
  
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init(); 
  LED_Init();
  Key_Init();
  HAL_TIM_Base_Start_IT(&htim1);
  LED_OFF();
  MyI2C_Init();
  MPU6050_Init();
  InitMotor();
  InitAllEncoderAll();
  BlueInitAllUSART();
  InitAllUSART();
/*   hhSerialSendString("hello");
	Serial_Printf("world"); */


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KeyNum=Key_GetNum();
    if(KeyNum==1)
    {
      RunFlag=!RunFlag;
    }
    if(RunFlag)
    {
      LED_ON();
    }else
    {
      LED_OFF();
    }




  
    OLED_Printf(0, 0, OLED_8X16, "%+06d", AX);		//显示AX
		OLED_Printf(0, 16, OLED_8X16, "%+06d", AY);		//显示AY
		OLED_Printf(0, 32, OLED_8X16, "%+06d", AZ);		//显示AZ
		OLED_Printf(64, 0, OLED_8X16, "%+06d", GX);		//显示GX
		OLED_Printf(64, 16, OLED_8X16, "%+06d", GY);	//显示GY
		OLED_Printf(64, 32, OLED_8X16, "%+06d", GZ);	//显示GZ 
		OLED_Printf(64, 48, OLED_8X16, "C:%05d", TimerCount);//显示计数器
//	   BlueSerial_Printf("[plot,%f]",AngleAcc );//发送角度数据到蓝牙串口
	  BlueSerial_Printf("[plot,%f,%f,%f]",AngleAcc, AngleGyro, Angle );//发送通过加速度传感器和陀螺仪得到的角度数据到蓝牙串口
	  
//    Serial_Printf("[plot,%f]",AngleAcc );//发送角度数据到蓝牙串口
		OLED_Update();
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */


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

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
  {
    static uint16_t Counter0=0;
    Counter0++;
    if(htim==&htim1)
    {
      Key_Tick();
      id(Counter0>=10)
      {
        Counter0=0;
        MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
        AngleAcc=-atan2(AX, AZ)/3.14159*180;
        AngleGyro=Angle+GY/32768.0*2000*0.01;
        float Alpha=0.01;
        Angle=Alpha*AngleAcc+(1-Alpha)*AngleGyro;

      }
    
      
    }
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
