/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>
#include "GPIO.h" 
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			1
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 90 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN		( 8 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configQUEUE_REGISTRY_SIZE 	0


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define configUSE_EDF_SCHEDULER			1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1
#define configUSE_APPLICATION_TASK_TAG  1

#define configGENERATE_RUN_TIME_STATS 1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()	
#define portGET_RUN_TIME_COUNTER_VALUE()	(T1TC)
/*-----------------------------------------------------------*/

/*Set Tasks IDs*/
#define TASK_ID												(int)(pxCurrentTCB->pxTaskTag) 
#define LD1_TASK_ID    								0x01
#define LD2_TASK_ID    								0x02
#define B1_TASK_ID     								0x03
#define B2_TASK_ID     								0x04
#define TRANSMITTER_TASK_ID  					0x05
#define UARTRECIEVER_TASK_ID   				0x06

/*Set GPIO Trace Pins*/
#define IDLE_TRACE_PIN               	PIN3
#define UART_TRACE_PIN               	PIN4
#define LD1_TRACE_PIN                	PIN5
#define LD2_TRACE_PIN                	PIN6
#define TRANSMIT_TRACE_PIN           	PIN7
#define B1_TRACE_PIN                 	PIN8
#define B2_TRACE_PIN                 	PIN9

/*extern tasks time variables*/
extern unsigned int LD1_InTime,		LD1_OutTime,	LD1_TotalTime;
extern unsigned int LD2_InTime,		LD2_OutTime,	LD2_TotalTime;
extern unsigned int B1_InTime,		B1_OutTime,		B1_TotalTime;
extern unsigned int B2_InTime,		B2_OutTime,		B2_TotalTime;
extern unsigned int PeriodicTransmitter_InTime,	PeriodicTransmitter_OutTime,	PeriodicTransmitter_TotalTime;
extern unsigned int Uart_Receiver_InTime,				Uart_Receiver_OutTime,				Uart_Receiver_TotalTime;
extern unsigned int totalExecution,							totalTime;
extern float CPU_Load;

/*-----------------------------------------------------------*/

#define traceTASK_SWITCHED_IN() do\
{\
			if(TASK_ID ==0)\
			{\
					GPIO_write(PORT_0,IDLE_TRACE_PIN,PIN_IS_HIGH);\
			}\
			else if(TASK_ID ==LD1_TASK_ID)\
			{\
					GPIO_write(PORT_0,LD1_TRACE_PIN,PIN_IS_HIGH);\
					LD1_InTime=T1TC;\
			}\
			else if(TASK_ID ==LD2_TASK_ID)\
			{\
					GPIO_write(PORT_0,LD2_TRACE_PIN,PIN_IS_HIGH);\
					LD2_InTime=T1TC;\
			}\
			else if(TASK_ID ==B1_TASK_ID)\
			{\
					GPIO_write(PORT_0,B1_TRACE_PIN,PIN_IS_HIGH);\
					B1_InTime=T1TC;\
			}\
			else if(TASK_ID ==B2_TASK_ID)\
			{\
					GPIO_write(PORT_0,B2_TRACE_PIN,PIN_IS_HIGH);\
					B2_InTime=T1TC;\
			}\
			else if(TASK_ID ==TRANSMITTER_TASK_ID)\
			{\
					GPIO_write(PORT_0,TRANSMIT_TRACE_PIN,PIN_IS_HIGH);\
					PeriodicTransmitter_InTime=T1TC;\
			}\
			else if(TASK_ID ==UARTRECIEVER_TASK_ID)\
			{\
					GPIO_write(PORT_0,UART_TRACE_PIN,PIN_IS_HIGH);\
					Uart_Receiver_InTime=T1TC;\
			}\
}while(0)


#define traceTASK_SWITCHED_OUT() do\
{\
			if(TASK_ID == 0)\
			{\
					GPIO_write(PORT_0,IDLE_TRACE_PIN,PIN_IS_LOW);\
			}\
			else if(TASK_ID ==LD1_TASK_ID)\
			{\
					GPIO_write(PORT_0,LD1_TRACE_PIN,PIN_IS_LOW);\
					LD1_OutTime=T1TC;\
					LD1_TotalTime+=(LD1_OutTime-LD1_InTime);\
			}\
			else if(TASK_ID ==LD2_TASK_ID)\
			{\
					GPIO_write(PORT_0,LD2_TRACE_PIN,PIN_IS_LOW);\
					LD2_OutTime=T1TC;\
					LD2_TotalTime+=(LD2_OutTime-LD2_InTime);\
			}\
			else if(TASK_ID ==B1_TASK_ID)\
			{\
					GPIO_write(PORT_0,B1_TRACE_PIN,PIN_IS_LOW);\
					B1_OutTime=T1TC;\
					B1_TotalTime+=(B1_OutTime-B1_InTime);\
			}\
			else if(TASK_ID ==B2_TASK_ID)\
			{\
					GPIO_write(PORT_0,B2_TRACE_PIN,PIN_IS_LOW);\
					B2_OutTime=T1TC;\
					B2_TotalTime+=(B2_OutTime-B2_InTime);\
			}\
			else if(TASK_ID ==TRANSMITTER_TASK_ID)\
			{\
					GPIO_write(PORT_0,TRANSMIT_TRACE_PIN,PIN_IS_LOW);\
					PeriodicTransmitter_OutTime=T1TC;\
					PeriodicTransmitter_TotalTime+=(PeriodicTransmitter_OutTime-PeriodicTransmitter_InTime);\
			}\
			else if(TASK_ID ==UARTRECIEVER_TASK_ID)\
			{\
					GPIO_write(PORT_0,UART_TRACE_PIN,PIN_IS_LOW);\
					Uart_Receiver_OutTime=T1TC;\
					Uart_Receiver_TotalTime+=(Uart_Receiver_OutTime-Uart_Receiver_InTime);\
			}\
			totalExecution=(LD1_TotalTime+LD2_TotalTime+B1_TotalTime+B2_TotalTime+PeriodicTransmitter_TotalTime+Uart_Receiver_TotalTime);\
			totalTime=T1TC;\
			CPU_Load=(float)(totalExecution)/(float)totalTime*100;\
}while(0)

#endif /* FREERTOS_CONFIG_H */
