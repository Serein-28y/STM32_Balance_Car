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
#include "pid.h"
#include "string.h"
#include "stdlib.h"

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
PID_t AnglePID=
{
  .Kp= 7.00,//3.71,//3.4,//3.8,
  .Ki=0.00,//0.30,//0.43,//0.21,
  .Kd=6.21,//3.48,//3.46,//3.0,

  .OutMax=100,
  .OutMin=-100,
};
PID_t SpeedPID=
{
  .Kp=0,
  .Ki=0,
  .Kd=0,

  .OutMax=20.0,
  .OutMin=-20.0,

};


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
  int16_t LeftPWM,RightPWM;
  int16_t AvePWM,DifPWM;

  float LeftSpeed, RightSpeed;		//左速度，右速度
  float AveSpeed, DifSpeed;			//平均速度，差分速度
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

  LED_OFF();
  MyI2C_Init();
  MPU6050_Init();
  InitMotor();
  InitAllEncoderAll();
  BlueInitAllUSART();
  InitAllUSART(); 
  HAL_TIM_Base_Start_IT(&htim1);
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
      PID_Init(&AnglePID);
      PID_Init(&SpeedPID);
      RunFlag=!RunFlag;
    }
    if(RunFlag){LED_ON();}else{LED_OFF();}
    if (BlueSerial_RxFlag == 1) {
	  	//strtok strcmp atoi/atof

	  	//收到原始的数据包
	  	char *Tag = strtok(BlueSerial_RxPacket, ",");
	  	if (strcmp(Tag, "key") == 0) {
	  		//收到并处理按键数据包如：[key,1,up]
	  		//char *Name = strtok(NULL, ",");
	  		//char *Action = strtok(NULL, ",");
	  	} else if (strcmp(Tag, "slider") == 0) {
	  		//收到并处理滑杆数据包如：[slider,1,78]
	  		char *Name = strtok(NULL, ",");
	  		char *Value = strtok(NULL, ",");
	  		/*执行滑杆操作*/
	  		if (strcmp(Name, "AngleKp") == 0)			//如果滑杆名称是AngleKp
	  				{
	  			AnglePID.Kp = atof(Value);				//则把滑杆值赋值给角度环Kp
	  		} else if (strcmp(Name, "AngleKi") == 0)		//如果滑杆名称是AngleKi
	  				{
	  			AnglePID.Ki = atof(Value);				//则把滑杆值赋值给角度环Ki
	  		} else if (strcmp(Name, "AngleKd") == 0)		//如果滑杆名称是AngleKd
	  				{
	  			AnglePID.Kd = atof(Value);				//则把滑杆值赋值给角度环Kd
	  		}else if (strcmp(Name, "SpeedKp") == 0)		//如果滑杆名称是SpeedKp
	  		{
	  			SpeedPID.Kp = atof(Value);				//则把滑杆值赋值给速度环Kp
	  		}
	  		else if (strcmp(Name, "SpeedKi") == 0)		//如果滑杆名称是SpeedKi
	  		{
	  			SpeedPID.Ki = atof(Value);				//则把滑杆值赋值给速度环Ki
	  		}
	  		else if (strcmp(Name, "SpeedKd") == 0)		//如果滑杆名称是SpeedKd
	  		{
	  			SpeedPID.Kd = atof(Value);				//则把滑杆值赋值给速度环Kd
	  		}
	  	} else if (strcmp(Tag, "joystick") == 0) {
	  		//收到并处理摇杆数据包如：[joystick,-10,20,40,67]
	  		int8_t LH = atoi(strtok(NULL, ","));
	  			int8_t LV = atoi(strtok(NULL, ","));
	  			int8_t RH = atoi(strtok(NULL, ","));
	  			int8_t RV = atoi(strtok(NULL, ","));
	  			SpeedPID.Target = LV / 25.0;		//摇杆值LV缩放后，控制速度环目标值
	  			DifPWM = RH / 2;				//摇杆值RH缩放后，控制差分PWM，左右转弯控制

	  			(void)LH;   // 显式告诉编译器：我知道它没用
	  			(void)RV;
	  	}
	  	BlueSerial_RxFlag = 0;
	  }
	  /*OLED显示*/
	  OLED_Clear();
	  OLED_Printf(0, 0, OLED_6X8, "  Angle");						//OLED左侧显示角度环参数
	  OLED_Printf(0, 8, OLED_6X8, "P:%05.2f", AnglePID.Kp);		//角度环Kp
	  OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", AnglePID.Ki);		//角度环Ki
	  OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", AnglePID.Kd);		//角度环Kd
	  OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", AnglePID.Target);	//角度环目标值
	  OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", Angle);			//角度环实际值
	  OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", AnglePID.Out);	//角度环输出值
	  OLED_Printf(0, 56, OLED_6X8, "GY:%+05d", GY);				//显示GY，便于校准陀螺仪零漂

	  OLED_Printf(50, 0, OLED_6X8, "Speed");						//OLED中间显示速度环参数
	  OLED_Printf(50, 8, OLED_6X8, "%05.2f", SpeedPID.Kp);		//速度环Kp
	  OLED_Printf(50, 16, OLED_6X8, "%05.2f", SpeedPID.Ki);		//速度环Ki
	  OLED_Printf(50, 24, OLED_6X8, "%05.2f", SpeedPID.Kd);		//速度环Kd
	  OLED_Printf(50, 32, OLED_6X8, "%+05.1f", SpeedPID.Target);	//速度环目标值
	  OLED_Printf(50, 40, OLED_6X8, "%+05.1f", AveSpeed);			//速度环实际值
	  OLED_Printf(50, 48, OLED_6X8, "%+05.0f", SpeedPID.Out);		//速度环输出值
	  OLED_Update();				//OLED更新

//	  BlueSerial_Printf("[plot,%f,%f]", AnglePID.Target, Angle);	//绘制AnglePID.Target和Angle的波形
	  BlueSerial_Printf("[plot,%f,%f]", SpeedPID.Target, AveSpeed);	//绘制SpeedPID.Target和AveSpeed的波形

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
    if(htim==&htim1)
    {
      static uint16_t Counter0=0;
      Key_Tick();
      Counter0++;
      if(Counter0>=10)
      {
        Counter0=0;
        MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
        GY-=-15;//零漂
        AngleAcc=-atan2(AX, AZ)/3.14159*180;
        AngleAcc+=0.1;
        AngleGyro=Angle+GY/32768.0*2000*0.01;
        float Alpha=0.01;
        Angle=Alpha*AngleAcc+(1-Alpha)*AngleGyro;
//        Angle-=1.2;
      if(Angle>50||Angle<-50) 
      {
        RunFlag=0;
      }
      if(RunFlag)
      {
        AnglePID.Actual=Angle;
        PID_Update(&AnglePID);
        AvePWM=-AnglePID.Out;
        LeftPWM=AvePWM+DifPWM/2;
        RightPWM=AvePWM-DifPWM/2;

        if(LeftPWM>100){LeftPWM=100;}else if(LeftPWM<-100){LeftPWM=-100;}
        if(RightPWM>100){RightPWM=100;}else if(RightPWM<-100){RightPWM=-100;}
        Motor_SetPWM(1,LeftPWM);
        Motor_SetPWM(2,RightPWM);

      }else
      {
        Motor_SetPWM(1,0);
        Motor_SetPWM(2,0);
      }
      }
      
      static uint16_t Counter1=0;
      Counter1++;
      if(Counter1>=50)
      {
        Counter1=0;
        LeftSpeed=Encoder_Get(1)/ 44.0 / 0.05 / 9.27666;
        RightSpeed=Encoder_Get(2)/ 44.0 / 0.05 / 9.27666;
        
        AveSpeed=(LeftSpeed+RightSpeed)/2.0;
        DifSpeed=(LeftSpeed-RightSpeed);

        if(RunFlag)
        {
          SpeedPID.Actual=AveSpeed;
          PID_Update(&SpeedPID);
          AnglePID.Target=SpeedPID.Out;

        }
      }
      TimerCount = __HAL_TIM_GET_COUNTER(&htim1);
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
