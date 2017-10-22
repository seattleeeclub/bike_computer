# Bike Computer / GPS Data Logger
Bike Computer / GPS Data Logger builton the STM32F429 Discovery Board

Original Version
----------------
The original version of the bike computer / gps data logger was built on the Tiva Launchpad using the TM4C123XX processor.  The concept was to build a gps data logger that would display lat, lng, speed, bearing, etc.  The display consisted of a TFT 160x128 display from Adafruit (PN #XXXXX).  The display has an sd card holder, making it perfect to write lat/lng position data to a formatted file.  The idea being that one could format the file in a way to import into another program, for example Google Earth.  The gps module was from SparkFun (PN #####).  A photo of the original prototype is shown in project photos.  The project was compiled using Rowely Crossworks.  As of about 10/2017, development on this version stopped in favor of using the STM32F429 Discovery Board.

Photo of Original Prototype

![alt text](https://github.com/seattleeeclub/bike_computer/blob/master/photos_hardware/original/datalogger_091217.jpg)

Updated Version
---------------
The new / continued version will be built on the STM32F429 Discovery board.  The board contains a full color, 320x240 display integrated into the board, so less soldering is required to built it.  Additional hardware includes user buttons and an sdcard holder (user your favorite one, but the one from Sparkfun looks pretty good).  Although the disco board has SDIO capability, the sdcard will be controlled using SPI interface (SDIO pins consumed from LCD controller).  Low level hardware drivers are developed using the STM Cube tool.  Top level control uses FreeRTOS.
Project is built using TrueStudio and auto-gen code from STMCube software.
Project Location: bike_computer/source/stm32f429/cube/bike_computer
Supporting Files/Components: same heirarchial level as "cube" folder.  FreeRTOS and simplehsm at same level as Source

Using the Cube Tool, we need at least the following peripherals initialized:
-User Buttons (3, configured as interrupts, with cooresponding rtos task)
-User LEDS (Debug + Extras, visual indicators, use the two on board leds for now)
-LCD Peripheral (config as 2 layer, map to sdram memory locations, set frame buffer size to window size)
-SDRAM Peripheral (follow along with standard libs example to configure this)
-UART (GPS - the one from Sparkfun outputs set of lat/lng every second)
-SPI (sdcard - control sdcard via spi interface.  Following along with recommended init proceedure from FatFS site)
-USB (control via terminal)



