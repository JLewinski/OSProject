/*----------------------------------------------------------------------------
 * Name:        app.c
 * Purpose:     Defines variables and functions to be used by app.c and some
 *              which are referenced/used by main.c
 * Designer:    Jacob Lewinski
 *----------------------------------------------------------------------------*/
#include "Include/cmsis_os2.h"

#ifndef MyAPP
#define MyAPP

//IDs for threads
osThreadId_t tid_print, tid_compute, tid_clock, tid_gyro, tid_acc, tid_mag, tid_printMess;
//Queue to pass data to the last 2 inputs for the LCD
osMessageQueueId_t clockPrintQueue;

//Event to toggle displaying data
osEventFlagsId_t workFlag;

void app_main(void *argument);
void compute(void *argument);
void printCharacters(void *argument);
void clock(void *argument);
void gyro(void *argument);
void acc(void *argument);
void mag(void *argument);
void printTime(void *argument);

//To select which sensor is displaying data
enum Sensor
{
    Gyroscope,
    Accelerometer,
    Magnetometer
} sensor;

//To select which dimension (X, Y, or Z) is being displayed
enum Dimension
{
    dimensionX,
    dimensionY,
    dimensionZ
} dimension;

#endif