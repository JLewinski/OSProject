# AUBURN UNIVERSITY ELEC 5260/6260/6266 Final Project – Real-Time Operating Systems (RTOS)

## Project specifications:

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