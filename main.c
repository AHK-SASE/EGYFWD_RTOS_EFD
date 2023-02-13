/*
 * FreeRTOS Kernel V10.5.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
	
/* Task handlers */
TaskHandle_t Periodic_Transmitter_Handler = NULL;
TaskHandle_t UART_Task_Handler = NULL;
TaskHandle_t Button_1_Monitor_Handler =NULL;
TaskHandle_t Button_2_Monitor_Handler =NULL;
TaskHandle_t Load_1_Simulation_Handler =NULL;
TaskHandle_t Load_2_Simulation_Handler =NULL;

/* Queue used to send and receive pointers to messages between tasks */
QueueHandle_t xPointerQueue = NULL;

/*Task Periodicity*/
#define Button_1_Monitor_PERIOD     	50
#define Button_2_Monitor_PERIOD     	50 
#define Periodic_Transmitter_PERIOD  	100
#define Uart_Receiver_PERIOD   				20
#define Load_1_Simulation_PERIOD    	10
#define Load_2_Simulation_PERIOD    	100
#define Load_1_ms											7000
#define fixed_Priority								2

/* Msgs to be sent to queue by each task */
char * Transmitting_Task = "Transmitting each 100 ms";
char * Button_1_Rising_Edge = "Button1 State: Rising Edge";
char * Button_1_Falling_Edge = "Button1 State: Falling Edge";
char * Button_2_Rising_Edge = "Button2 State: Rising_Edge";
char * Button_2_Falling_Edge = "Button2 State: Falling Edge";
//char TaskState_Buffer[270]; 

/*Tasks time variables*/
unsigned int LD1_InTime=0,	LD1_OutTime=0,	LD1_TotalTime=0;
unsigned int LD2_InTime=0,	LD2_OutTime=0,	LD2_TotalTime=0;
unsigned int B1_InTime=0,		B1_OutTime=0,		B1_TotalTime=0;
unsigned int B2_InTime=0,		B2_OutTime=0,		B2_TotalTime=0;
unsigned int PeriodicTransmitter_InTime=0,	PeriodicTransmitter_OutTime=0,	PeriodicTransmitter_TotalTime=0;
unsigned int Uart_Receiver_InTime=0,				Uart_Receiver_OutTime=0,				Uart_Receiver_TotalTime=0;
unsigned int totalExecution=0,							totalTime=0;
float CPU_Load=0;

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* Tasks implementation */

/*-----------------------------------------------------------*/

void Periodic_Transmitter( void * pvParameters )
{
		
		const TickType_t xFrequency=Periodic_Transmitter_PERIOD;
		TickType_t LastWakeTime;
		LastWakeTime =xTaskGetTickCount();
 		 
	/* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.*/ 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        /* Task code goes here. */
					xQueueSend(	xPointerQueue,
							( void * ) &Transmitting_Task,
                      ( TickType_t ) 0 );
							
					vTaskDelayUntil(&LastWakeTime,xFrequency);				
    }
}

void Uart_Receiver( void * pvParameters )
{  
		char  *receive;
		const TickType_t xFrequency=Uart_Receiver_PERIOD;
		TickType_t LastWakeTime;
		LastWakeTime =xTaskGetTickCount();

	/* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.*/ 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	
    for( ;; )
    {
				//vTaskGetRunTimeStats( TaskState_Buffer );
				//vSerialPutString( TaskState_Buffer, 300);
			if( xPointerQueue != NULL )
   {
      /* Receive a message from the created queue */
      if( xQueueReceive( xPointerQueue,
                         &( receive ),
                         ( TickType_t ) 0 ) == pdPASS )
      {
        							xSerialPutChar('\n');
											vSerialPutString(receive, 28);
      }
   }
			
      /* Task code goes here. */
				vTaskDelayUntil(&LastWakeTime,20);		
    }
}

void Button_1_Monitor( void * pvParameters )
{
		pinState_t u8Button1State;
		
		const TickType_t xFrequency=Button_1_Monitor_PERIOD;	
		TickType_t LastWakeTime;
		LastWakeTime =xTaskGetTickCount();
		
		/* get initial state of button 1*/
		u8Button1State = GPIO_read(PORT_0,PIN0); 
	
	/* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.*/ 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        /* Task code goes here. */
				/* Check if the button state have changed*/
				if(u8Button1State!=(GPIO_read(PORT_0,PIN0)))
				{
					/* check for falling or rising edge, then send a queue msg with the state*/					
					 if( u8Button1State == PIN_IS_HIGH )
						{
							xQueueSend( xPointerQueue,
                      ( void * ) &Button_1_Rising_Edge,
                      ( TickType_t ) 0 );  
							u8Button1State = GPIO_read(PORT_0,PIN0); 

        		}
						else
						{							
							xQueueSend( xPointerQueue,
                      ( void * ) &Button_1_Falling_Edge,
                      ( TickType_t ) 0 );  
							u8Button1State = GPIO_read(PORT_0,PIN0);

						}
				}
				vTaskDelayUntil(&LastWakeTime,xFrequency);		

		}
}

void Button_2_Monitor( void * pvParameters )
{
		pinState_t u8Button2State;
	
		const TickType_t xFrequency	=	Button_2_Monitor_PERIOD;	
		TickType_t LastWakeTime;
		LastWakeTime =xTaskGetTickCount();
	
		/* get initial state of button 2*/
		u8Button2State = GPIO_read(PORT_0,PIN1); 

	/* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.*/ 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    for( ;; )
    {
        /* Task code goes here. */
				/* Check if the button state have changed*/
				if(u8Button2State!=(GPIO_read(PORT_0,PIN1)))
				{
					/* check for falling or rising edge, then send a queue msg with the state*/					
					 if( u8Button2State == PIN_IS_HIGH )
						{
							xQueueSend( xPointerQueue,
                      ( void * ) &Button_2_Rising_Edge,
                      ( TickType_t ) 0 );  
							u8Button2State = GPIO_read(PORT_0,PIN1); 

        		}
						else
							{							
							xQueueSend( xPointerQueue,
                      ( void * ) &Button_2_Falling_Edge,
                      ( TickType_t ) 0 );  
							u8Button2State = GPIO_read(PORT_0,PIN1);

						}
				}
				vTaskDelayUntil(&LastWakeTime,xFrequency);		
		}
}


void Load_1_Simulation( void * pvParameters )
{
    int i;
		const TickType_t xFrequency=Load_1_Simulation_PERIOD;

	  TickType_t LastWakeTime;
		LastWakeTime =xTaskGetTickCount();
	
	/* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.*/ 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        /* Task code goes here. */
				for(i=0;i<Load_1_ms*5;i++) // 7000 = 1 ms  
				{
					i=i;
				}
					
				vTaskDelayUntil(&LastWakeTime,xFrequency);				
    }
}

void Load_2_Simulation( void * pvParameters )
{
		int i;
		const TickType_t xFrequency=Load_2_Simulation_PERIOD;

	  TickType_t LastWakeTime;	 
		LastWakeTime =xTaskGetTickCount();
	
	/* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.*/ 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
		
    for( ;; )
    {
     
				/* Task code goes here. */
				for(i=0;i<Load_1_ms*12;i++)// 7000 = 1 ms 
				{
					i=i;
				}
				
				vTaskDelayUntil(&LastWakeTime,xFrequency);			
    }
}

/*-----------------------------------------------------------*/

 /* Application entry point:
 * Starts all the other tasks, then starts the scheduler. 

-----------------------------------------------------------*/

int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
   /* Create the queue used to send pointers to struct AMessage structures. */
  xPointerQueue = xQueueCreate(1,                    /* The number of items the queue can hold. */
                         sizeof( char *) );/* Size of each item is big enough to hold only a pointer. */                     
                         
	
  /* Create the task, storing the handle. */
	xPeriodicTaskCreate(
                    Load_1_Simulation,       			/* Function that implements the task. */
                    "LOAD1 1 TASK",         			/* Text name for the task. */
                    100,      										/* Stack size in words, not bytes. */
                    ( void * ) 0,    							/* Parameter passed into the task. */
                    fixed_Priority,								/* Priority at which the task is created. */
                    &Load_1_Simulation_Handler ,Load_1_Simulation_PERIOD );      /* Used to pass out the created task's handle. */

	xPeriodicTaskCreate(
                    Load_2_Simulation,       			/* Function that implements the task. */
                    "LOAD 2 TASK",         	 			/* Text name for the task. */
                    100,      										/* Stack size in words, not bytes. */
                    ( void * ) 0,    							/* Parameter passed into the task. */
                    fixed_Priority,								/* Priority at which the task is created. */
                    &Load_2_Simulation_Handler,Load_2_Simulation_PERIOD );      /* Used to pass out the created task's handle. */

  	
  xPeriodicTaskCreate(
                    Periodic_Transmitter,       	/* Function that implements the task. */
                    "Transmitter Task",          	/* Text name for the task. */
                    100,      										/* Stack size in words, not bytes. */
                    ( void * ) 0,    							/* Parameter passed into the task. */
                    fixed_Priority,								/* Priority at which the task is created. */
                    &Periodic_Transmitter_Handler,Periodic_Transmitter_PERIOD );      /* Used to pass out the created task's handle. */

	xPeriodicTaskCreate(
                    Uart_Receiver,       					/* Function that implements the task. */
                    "UART Task",          				/* Text name for the task. */
                    100,      										/* Stack size in words, not bytes. */
                    ( void * ) 0,    							/* Parameter passed into the task. */
                    fixed_Priority,								/* Priority at which the task is created. */
                    &UART_Task_Handler,Uart_Receiver_PERIOD );      /* Used to pass out the created task's handle. */
	xPeriodicTaskCreate(
                    Button_1_Monitor,       			/* Function that implements the task. */
                    "Button 1 Task",          		/* Text name for the task. */
                    100,      										/* Stack size in words, not bytes. */
                    ( void * ) 0,    							/* Parameter passed into the task. */
                    fixed_Priority,								/* Priority at which the task is created. */
                    &Button_1_Monitor_Handler,Button_1_Monitor_PERIOD );      /* Used to pass out the created task's handle. */

	xPeriodicTaskCreate(
                    Button_2_Monitor,       			/* Function that implements the task. */
                    "Button 2 Task",          		/* Text name for the task. */
                    100,      										/* Stack size in words, not bytes. */
                    ( void * ) 0,    							/* Parameter passed into the task. */
                    fixed_Priority,								/* Priority at which the task is created. */
                    &Button_2_Monitor_Handler ,Button_2_Monitor_PERIOD);      /* Used to pass out the created task's handle. */
								

/*	set tasks' tags		*/ 
	vTaskSetApplicationTaskTag( Load_1_Simulation_Handler,		(void*)LD1_TASK_ID );
	vTaskSetApplicationTaskTag( Load_2_Simulation_Handler,		(void*)LD2_TASK_ID );
	vTaskSetApplicationTaskTag(	Button_1_Monitor_Handler,			(void*)B1_TASK_ID);
	vTaskSetApplicationTaskTag(	Button_2_Monitor_Handler,			(void*)B2_TASK_ID);										
	vTaskSetApplicationTaskTag(	Periodic_Transmitter_Handler,	(void *)TRANSMITTER_TASK_ID);
	vTaskSetApplicationTaskTag( UART_Task_Handler,						(void*)UARTRECIEVER_TASK_ID );

/* Now all the tasks have been started - start the scheduler. */
	vTaskStartScheduler();
	
	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;//(main_freq/1000)
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
void vApplicationTickHook(void)
{
	GPIO_write(PORT_0,PIN0,PIN_IS_LOW);
	GPIO_write(PORT_0,PIN0,PIN_IS_HIGH);
}

/*-----------------------------------------------------------*/

