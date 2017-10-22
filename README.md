# Bike Computer / GPS Data Logger
Bike Computer / GPS Data Logger builton the STM32F429 Discovery Board

Original Version
----------------
The original version of the bike computer / gps data logger was built on the Tiva Launchpad using the TM4C123XX processor.  The concept was to build a gps data logger that would display lat, lng, speed, bearing, etc.  The display consisted of a TFT 160x128 display from Adafruit (PN #XXXXX).  The display has an sd card holder, making it perfect to write lat/lng position data to a formatted file.  The idea being that one could format the file in a way to import into another program, for example Google Earth.  The gps module was from SparkFun (PN #####).  A photo of the original prototype is shown in project photos.  The project was compiled using Rowely Crossworks.  As of about 10/2017, development on this version stopped in favor of using the STM32F429 Discovery Board.  The main reason for this was that the disco board contains a very nice 320x240 full color display and can be programmed using True Studio, a free IDE for STM processors with no codesize limit.  All that's left is to add user buttons and and SD card breakout board.

Updated Version
---------------
The new / continued version will be built on the STM32F429 Discovery board.  The board contains a full color, 320x240 display integrated into the board, so less soldering is required to built it.  Additional hardware includes user buttons and an SD card holder (user your favorite one, but the one from Sparkfun looks pretty good).  Although the disco board has SDIO capability, the SD card will be controlled using SPI interface (SDIO pins consumed from LCD controller).  Low level hardware drivers are developed using the STM Cube tool.  Top level control uses FreeRTOS.



