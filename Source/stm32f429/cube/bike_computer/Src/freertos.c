/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "gpio.h"		//toggle LED_Green, LED_Red

#include "Memory.h"		//SDRAM layer definitions
#include "ltdc.h"		//layer control

#include "TouchPanel.h"

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	while(1)
	{

	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
__weak void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */

	while(1)
	{

	}
}
/* USER CODE END 5 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
	uint8_t buffer[16];
	int n;

	TouchPanelData tpData = TouchPanel_getPosition();

	LCD_SetTextBackColor(RED);
	LCD_SetTextLineColor(BLUE);
	LCD_Clear(0, RED);
	HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_0, 0);
	HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_0, 1);


	/* Infinite loop */
	for(;;)
	{
		//use only layer 0
		//HAL_GPIO_TogglePin(GPIOG, LED_Red_Pin);
		//HAL_GPIO_TogglePin(GPIOG, LED_Green_Pin);

		tpData = TouchPanel_getPosition();

		//write the position
		n = sprintf((char*)buffer, "Xpos: %d   ", tpData.xPos);
		LCD_DrawStringLength(0,1, buffer, (uint8_t)n);
		n = sprintf((char*)buffer, "Ypos: %d   ", tpData.yPos);
		LCD_DrawStringLength(0,2, buffer, (uint8_t)n);
		n = sprintf((char*)buffer, "Xraw: %d   ", tpData.xRawPos);
		LCD_DrawStringLength(0,3, buffer, (uint8_t)n);
		n = sprintf((char*)buffer, "Yraw: %d   ", tpData.yRawPos);
		LCD_DrawStringLength(0,4, buffer, (uint8_t)n);

		osDelay(200);

/*

		uint16_t chipID = TouchPanel_readChipID();
		uint8_t IDVersion = TouchPanel_readIDVersion();
		tpData = TouchPanel_getPosition();

		if ((chipID == TOUCH_PANEL_CHIP_ID) && (IDVersion == TOUCH_PANEL_ID_VER))
		{
			//hold red low and toggle green
			HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
			HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);
			LCD_DrawString(0,4, "---<SUCCESS>---");

			//write the position
			n = sprintf((char*)buffer, "Xpos: %d", tpData.xPos);
			LCD_DrawStringLength(0,1, buffer, (uint8_t)n);
			n = sprintf((char*)buffer, "Ypos: %d", tpData.yPos);
			LCD_DrawStringLength(0,2, buffer, (uint8_t)n);
			n = sprintf((char*)buffer, "Zpos: %d", tpData.zPos);
			LCD_DrawStringLength(0,3, buffer, (uint8_t)n);


		}
		else
		{
			HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
			HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);
			LCD_DrawString(0,4, "---<FAILURE>---");
		}


		HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_0, 0);
		HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_0, 1);
		osDelay(1000);

		//rotate 0 into 1 at diffferent angles
		HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_1, 0);
		HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_1, 1);

		for (int i = 0 ; i < 90 ; i++)
		{
			LCD_RotateBuffer(0, 1, 4 * i, 120, 160);
			osDelay(20);
		}

		osDelay(1000);

		//put pixels
		LCD_Clear(0, BLUE);
		LCD_DrawLine(0, 0, 0, 240, 240, WHITE);
		LCD_DrawLine(0, 240, 0, 0, 240, RED);
		HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_0, 0);
		HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_0, 1);
		osDelay(3000);

*/


	}

  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
