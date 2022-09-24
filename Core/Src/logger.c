/*
 * logger.c
 *
 *  Created on: Sep 24, 2022
 *      Author: petarlalov
 */

#include <stddef.h>
#include "logger.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdio.h>
#include "usb_device.h"
#include "usbd_cdc_if.h"

#define LINE_LEN 50
#define LOGGER_DEPTH 50

static void _logger_task(void * pvParameters);

QueueHandle_t _logger_queue GLOBAL_DATA;
TaskHandle_t _task_handle;

static StackType_t _logger_task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( configMINIMAL_STACK_SIZE * sizeof( StackType_t ) ) ) );
static StaticTask_t _logger_task_buffer;

static void start_logger_task( void )
{
	_task_handle = xTaskCreateStatic(_logger_task,
			"LoggerTask",
			configMINIMAL_STACK_SIZE,
			NULL,
			portPRIVILEGE_BIT,
			_logger_task_stack,
			&_logger_task_buffer);
}

static void _logger_task(void* pvParameters)
{
	MX_USB_DEVICE_Init();
	vTaskDelay(1000);
//	printf("_logger_task\r\n");

	while(1)
	{
		char line[LINE_LEN+1] = {0,};

		xQueueReceive(_logger_queue, line, portMAX_DELAY);
		size_t len = strlen(line);
		CDC_Transmit_FS((uint8_t*)line, len);
	}
}

int logger_open()
{
	_logger_queue = xQueueCreate(LOGGER_DEPTH, LINE_LEN);
	start_logger_task();

	return 0;
}

int logger_write(char* line)
{
	size_t line_len = strlen(line);
	int idx = 0;

	// Split the line into LINE_LEN segments
	while(line_len > LINE_LEN)
	{
		xQueueSend(_logger_queue, &line[idx], 10);

		idx += LINE_LEN;
		line_len -= LINE_LEN;
	}
	// Send the last segment if segmented is necessary
	xQueueSend(_logger_queue, &line[idx], 10);

	return 0;
}

int logger_close(void)
{
	vTaskDelete(_task_handle);
	vQueueDelete(_logger_queue);

	return 0;
}
