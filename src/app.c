/*----------------------------------------------------------------------------
 * Name:        app.c
 * Purpose:     The threads of the application to be run using RR. This is
 *              work of the program.
 * Designer:    Jacob Lewinski
 *----------------------------------------------------------------------------*/
#include "app.h"
#include "stm32l476g_discovery.h"
#include "stm32l476g_discovery_gyroscope.h"
#include "stm32l476g_discovery_compass.h"
#include "stm32l476g_discovery_glass_lcd.h"

int ledON = 0;
osMutexId_t dataMutex, displayMutex;
float gyroXYZ[3];
int16_t accXYZ[3], magXYZ[3];

//Sets up other threads and sets up mutexes, event flags, and Message Queues
void app_main(void *argument)
{
    dimension = dimensionX;
    sensor = Gyroscope;
    tid_compute = osThreadNew(compute, NULL, NULL);
    tid_print = osThreadNew(printCharacters, NULL, NULL);
    tid_clock = osThreadNew(clock, NULL, NULL);
    tid_gyro = osThreadNew(gyro, NULL, NULL);
    tid_acc = osThreadNew(acc, NULL, NULL);
    tid_mag = osThreadNew(mag, NULL, NULL);
    tid_printMess = osThreadNew(printTime, NULL, NULL);
    dataMutex = osMutexNew(NULL);
    displayMutex = osMutexNew(NULL);
    clockPrintQueue = osMessageQueueNew(1, 64, NULL);
    workFlag = osEventFlagsNew(NULL);

    //This thread will never be ready again
    osDelay(osWaitForever);
    for (;;)
        ;
}

//Gets data from gyroscope
void gyro(void *argument)
{
    while (1)
    {
        osMutexAcquire(dataMutex, osWaitForever);
        BSP_GYRO_GetXYZ(gyroXYZ);
        osMutexRelease(dataMutex);
        osDelay(50);
    }
}

//Gets data from accelerometer
void acc(void *argument)
{
    while (1)
    {
        osMutexAcquire(dataMutex, osWaitForever);
        BSP_COMPASS_AccGetXYZ(accXYZ);
        osMutexRelease(dataMutex);
        osDelay(50);
    }
}

//Gets data from magnetometer
void mag(void *argument)
{
    while (1)
    {
        osMutexAcquire(dataMutex, osWaitForever);
        BSP_COMPASS_MagGetXYZ(magXYZ);
        osMutexRelease(dataMutex);

        osDelay(50);
    }
}

/* Displays if event flag is on
 * Red if data > 0
 * Green if data < 0
*/
void compute(void *argument)
{
    uint8_t index = (uint8_t)dimension;
    int red, green;
    while (1)
    {
        if (osEventFlagsGet(workFlag))
        {
            index = (uint8_t)dimension;
            osMutexAcquire(dataMutex, osWaitForever);
            switch (sensor)
            {
            case Gyroscope:
                red = gyroXYZ[index] > 0;
                green = gyroXYZ[index] < 0;
                break;
            case Accelerometer:
                red = accXYZ[index] > 0;
                green = accXYZ[index] < 0;
                break;
            case Magnetometer:
                red = magXYZ[index] > 0;
                green = magXYZ[index] < 0;
                break;
            default:
                red = 0;
                green = 0;
                break;
            }
            osMutexRelease(dataMutex);
        }
        else
        {
            red = 0;
            green = 0;
        }

        if (red)
        {
            BSP_LED_On(LED_RED);
        }
        else
        {
            BSP_LED_Off(LED_RED);
        }
        if (green)
        {
            BSP_LED_On(LED_GREEN);
        }
        else
        {
            BSP_LED_Off(LED_GREEN);
        }
        osDelay(50);
    }
}

//Print out state of the program (sensor and dimension)
void printCharacters(void *argument)
{
    //init flags to 7 to display first state
    int flags = 7;

    //Make sure flags are cleared
    osThreadFlagsClear(7);

    //Possible strings to display (sensors)
    uint8_t statusString[3][3] = {{'G', 'Y', 'R'}, {'A', 'C', 'C'}, {'M', 'A', 'G'}};
    uint8_t direction;
    while (1)
    {
        //Having it timeout at 100ms because it should never need to wait that long for the display mutex to become free
        osMutexAcquire(displayMutex, 100);
        if (flags & 4)
        {
            BSP_LCD_GLASS_DisplayChar(statusString[sensor], POINT_OFF, DOUBLEPOINT_OFF, 0);
            BSP_LCD_GLASS_DisplayChar(statusString[sensor] + 1, POINT_OFF, DOUBLEPOINT_OFF, 1);
            BSP_LCD_GLASS_DisplayChar(statusString[sensor] + 2, POINT_OFF, DOUBLEPOINT_ON, 2);
        }
        if (flags & 2)
        {
            direction = (uint8_t)dimension + 'X';
            BSP_LCD_GLASS_DisplayChar(&direction, POINT_OFF, DOUBLEPOINT_ON, 3);
        }
        //clear the flag
        osThreadFlagsClear(flags);
        osMutexRelease(displayMutex);

        flags = osThreadFlagsWait(7, osFlagsWaitAny, osWaitForever);
    }
}

//Print out the total amount of time (00-99) that data has been active
void printTime(void *argument)
{
    uint32_t msg[2] = {'0', '0'};
    BSP_LCD_GLASS_DisplayChar(msg + 1, POINT_OFF, DOUBLEPOINT_OFF, 4);
    BSP_LCD_GLASS_DisplayChar(msg, POINT_OFF, DOUBLEPOINT_OFF, 5);
    while (1)
    {
        osMessageQueueGet(clockPrintQueue, msg, NULL, 1000);
        //Having it timeout at 100ms because it should never need to wait that long for the display mutex to become free
        osMutexAcquire(displayMutex, 100);

        BSP_LCD_GLASS_DisplayChar(msg + 1, POINT_OFF, DOUBLEPOINT_OFF, 4);
        BSP_LCD_GLASS_DisplayChar(msg, POINT_OFF, DOUBLEPOINT_OFF, 5);
        osMutexRelease(displayMutex);
    }
}

//Real time clock in seconds (00-99)
void clock(void *argument)
{
    uint32_t msg[2] = {'0', '0'}, ticks;
    while (1)
    {
        ticks = osKernelGetTickCount() + 1000;
        osEventFlagsWait(workFlag, 1, osFlagsNoClear, osWaitForever);
        msg[0]++;
        if (msg[0] > '9')
        {
            msg[1]++;
            msg[0] = '0';
            if (msg[1] > '9')
            {
                msg[1] = '0';
            }
        }
        if (osMessageQueuePut(clockPrintQueue, msg, 1, 500) != osOK)
        {
            osDelayUntil(ticks);
        }
        else
        {
            osDelayUntil(ticks);
        }
    }
}