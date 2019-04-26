/*----------------------------------------------------------------------------
 * Name:        main.c
 * Purpose:     OS Project: Sets up the program and handles Interupts
 * Designer:    Jacob Lewinski
 *----------------------------------------------------------------------------*/
#include "l3gd20.h"
#include "lsm303c.h"
#include "stm32l4xx.h" // Device header
#include "stm32l476g_discovery.h"
#include "stm32l476g_discovery_glass_lcd.h"
#include "stm32l476g_discovery_gyroscope.h"
#include "stm32l476g_discovery_compass.h"
#include "RTX/Library/IAR/IDE/RTE_Components.h"
#include "app.h"

//----------------PIN Interrupts for JOY STICK------------------
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

/* Up and down change the sensor
 * Left and right change the dimension
 * Sel pauses/resumes
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
    case SEL_JOY_PIN: //Pause/Resume data analysis
        if (osEventFlagsGet(workFlag))
        {
            osEventFlagsClear(workFlag, 1);
        }
        else
        {
            osEventFlagsSet(workFlag, 1);
        }
        break;
    case DOWN_JOY_PIN: //Change sensor
        if (sensor > Gyroscope)
        {
            sensor--;
            osThreadFlagsSet(tid_print, 4);
        }
        break;
    case UP_JOY_PIN: //Change sensor
        if (sensor < Magnetometer)
        {
            sensor++;
            osThreadFlagsSet(tid_print, 4);
        }
        break;
    case RIGHT_JOY_PIN: //Change dimension
        if (dimension != dimensionZ)
        {
            dimension++;
            osThreadFlagsSet(tid_print, 2);
        }
        break;
    case LEFT_JOY_PIN: //Change dimension
        if (dimension != dimensionX)
        {
            dimension--;
            osThreadFlagsSet(tid_print, 2);
        }
        break;
    default:
        //do nothing(could set it to erase)
        break;
    }
}

//Force HAL to get ticks from the os instead of how it normally does
uint32_t HAL_GetTick(void)
{
    return osKernelGetTickCount();
}

//Stops the program if there is an error
void Error_Handler(void)
{
    while (1)
        ;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 8000000
  *            PLL_M                          = 1
  *            PLL_N                          = 20
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @retval None
  */
void SystemClock_Config(void)
{
    /* oscillator and clocks configs */
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Disable Power Control clock */
    __HAL_RCC_PWR_CLK_DISABLE();

    /* 80 Mhz from MSI 8Mhz */
    /* MSI is enabled after System reset, activate PLL with MSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    RCC_OscInitStruct.PLL.PLLQ = 4;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Disable Power Control clock */
    __HAL_RCC_PWR_CLK_DISABLE();
}

//Set up HAL, CLK, BSP, and OS
int main()
{
    HAL_Init();
    SystemClock_Config();
    SystemCoreClockUpdate();

    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_RED);
    BSP_JOY_Init(JOY_MODE_EXTI);
    BSP_LCD_GLASS_Init();
    BSP_COMPASS_Init();
    BSP_GYRO_Init();

    osKernelInitialize();
    osThreadNew(app_main, NULL, NULL);
    if (osKernelGetState() == osKernelReady)
    {
        osKernelStart();
    }

    for (;;)
        ;
}
