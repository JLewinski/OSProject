#include <stdio.h>
#include "cs43l22.h"
#include "n25q128a.h"
#include "stm32l4xx.h" // Device header
#include "stm32l476g_discovery.h"
#include "stm32l476g_discovery_audio.h"
#include "stm32l476g_discovery_glass_lcd.h"
#include "stm32l476g_discovery_qspi.h"
#include "stm32l476g_discovery_gyroscope.h"
#include "stm32l476g_discovery_compass.h"
#include "stm32l476g_discovery_idd.h"
#include "RTX/Library/IAR/IDE/RTE_Components.h"
#include "Include/cmsis_os2.h"

osThreadId_t tid_phaseA;

void phaseA(void *argument)
{
    //something
}

void app_main(void *argument)
{
    tid_phaseA = osThreadNew(phaseA, NULL, NULL);
    osDelay(osWaitForever);
    for (;;)
        ;
}

int main()
{
    SystemCoreClockUpdate();
    //Initialize Joystick
    BSP_JOY_Init(JOY_MODE_EXTI);
    BSP_QSPI_Init();

    //need to enable interrupts before enabling components requiring delays
    __enable_irq();
    BSP_LCD_GLASS_Init();
    osKernelInitialize();
    osThreadNew(app_main, NULL, NULL);
    if (osKernelGetState() == osOK)
    {
        osKernelStart();
    }

    //won't execute unless above fails
    for (;;)
        ;
}