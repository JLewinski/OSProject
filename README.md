# AUBURN UNIVERSITY ELEC 5260/6260/6266 Final Project – Real-Time Operating Systems (RTOS)

## Project Requirments:

Design and implement a project that will exercise the ARM/CMSIS-RTOS Real-Time Operating
System on the STM32L476G-Discovery board. The project must contain the following elements.
1. The project must comprise at least seven different RTOS Threads.
2. The project must periodically acquire and use data, in some way, from each of the
following three sensors on the STM32L476G-Discovery board:
    1. L3GD20 3-axis gyroscope
    2. LSM30CTR 3D Magnetometer
    3. LSM30CTR 3D Accelerometer
(Magnetometer and Accelerometer are on the same IC.)
3. Project-related information must be displayed and updated on the LCD.
4. Optionally, you may use LEDs and/or Joystick in some way.
5. The project must exercise the following RTOS features in some appropriate way:
    1. One or both of “Thread Flags” and “Event Flags”
    2. One or both of “Mutex” and “Semaphore”
    3. “Message Queue”

You may design the project to do whatever you wish, provided that it meets the above
requirements.

The final project should be demonstrated in my office by the deadline. The following
deliverables should be provided at that time.

1. A one or two-page description of the project.
2. A hard copy of your programs (not the drivers).
3. A .zip file of your project (the entire project directory) so that I could run it later.

### CMSIS-RTOS RTX Information and Resources
- In the “Books” pane of uVision5, under “Tools User’s Guide”, the document “MDKARM Getting Started (PDF)” contains a chapter on CMSIS-RTOS2 (pages 26-36).
- On page 29 of that document is a link to the Keil web site that contains complete
information on CMSIS-RTOS2, including tutorials for creating projects, configuring
- CMSIS-RTOS RTX, and using the different RTOS functions and data structures.
<http://www.keil.com/pack/doc/CMSIS/RTOS2/html/index.html>
- The Keil installation directory has a sample “Blinky” project that uses CMSIS-RTOS2 on
a different board (STM32L496G-DISCOVERY) from ours, but is similar. On my PC
this is located at:
C:\Keil\ARM\Pack\Keil\STM32L4xx_DFP\2.0.0\MDK\Boards\ST\STM32L476G-DISCO\Blinky
You can copy the Blinky project to your own directory and modify its settings for your board.
Open the “Pack Installer” (button in the menu bar), select STM32L496G-Discovery in the
“Boards” tab, and then select CMSIS-RTOS2 Blinky in the “Examples” tab.

## Project Description:

### Setup (main.c)

This includes the proper headers for the gyroscope and compass (l3gd20 and lsm303c).

The main function sets up all the BSP helpers that we need to make use of the hardware. It sets up the system clock to use a faster clock frequency to allow for more precise thread management and real time clock.

It enables the hal_delay function to use the osKernelTicks instead of the hal ticks by modifying the "HAL_GetTick(void)" function.

Finally, it enables the JoyStick to communicate with the threads:

- SEL Button: toggles the workFlag to enable/disable LEDs and clock
- Up/Down Buttons: switches which sensor data is being displayed
- Left/Right Buttons: switches which dimension data is being displayed

The JoyStick allows these threads to be managed asynchronously.

### Threads

####  1. app_main

Initializes the state variables and creates other threads. This does not run after its initial run.

#### 2. gyro

Gets data from the gyroscope every 50ms. It first acquires the dataMutex to make sure that something isn't trying to access the data at the same time. This also ensures that other threads won't be getting data at the same time.

#### 3. acc

Gets data from the accelerometer every 50ms. It first acquires the dataMutex to make sure that something isn't trying to access the data at the same time. This also ensures that other threads won't be getting data at the same time.

#### 4. gyro

Gets data from the magnetometer every 50ms. It first acquires the dataMutex to make sure that something isn't trying to access the data at the same time. This also ensures that other threads won't be getting data at the same time.

#### 5. compute

First acquires the dataMutex. It then calculates if a specific data value is positive or negative. This data value is specified by the state variables (sensor and dimension). It then turns on the red LED if the data is negative or turns on the green LED if the data is positive. It turns off whatever LED is not in use. If the value is 0 it turns off both the green and red LEDs. Before it manages the LED states it releases the dataMutex because it is no longer accessing the data. This runs every 50ms when the workFlag is enabled. If the workFlag is disabled it turns off both the green and red LEDs.

### 6. printCharacters

Prints the state variables. Depending on what sensor is selected it will print "GYR", "ACC", or "MAG" (gyroscope, accelerometer, magnetometer respectively). It will also print an 'X', 'Y', or 'Z' after the three letter to display the dimension state. It will only print the values when they are changed and does this based on the thread flag. It waits until one of the first 3 LSB (least significant bit(s)) are raised. Then, after acquiring the displayMutex, prints the sensor characters if the 3rd LSB is high and prints the dimension character if the 2nd LSB is high. It clears the bits that are high when first sees the flag to prevent it from constantly updating the display when it doesn't need to. The event flag is raised by the JoyStick interrupt which was mentioned earlier.

### 7. printTime

This thread prints a message which consists of 2 characters into the last two spots on the LCD. It is called printTime because the clock is passing the message to it, but it has the potential to print any data passed to it (not just time). This thread first prints '0', '0' as an initial placeholder and then waits to receive the message before printing anything else. The messageQueue it uses only allows for one message in the queue to prevent it from missing any data. Additionally, before it prints any data (except the initial data) it acquires the displayMutex. It releases the displayMutex after displaying the message.

### 8. clock

This is a real time clock which keeps track of seconds (with a max of 99 seconds). It is only enabled when the workFlag is raised. To ensure that it is an accurate clock it first calculates the tickCount that is one second away and then increments the time and passes the time using the clockPrintQueue. It also has a timeout of 500ms when attempting to write to the clockPrintQueue to prevent the clock from delaying.

### Notes:

The printCharacters thread and printTime thread could be easily combined to use one method or the other and are only separated to show different ways to do it.

