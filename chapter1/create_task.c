#include <stdio.h>
#include <stdlib.h>

/* freertos kernel */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

/* for print */
#include "fileIO.h"

static void vTask(void *pvParameters);

void vApplicationIdleHook( void );

#define mainTASK1_PRIORITY       (tskIDLE_PRIORITY + 1)
#define mainTASK2_PRIORITY       (tskIDLE_PRIORITY + 2)

#define mainTASK1_STACK_SIZE        ((unsigned short) 2048)

#define mainDELAY_LOOP_COUNT      10000000

const char *pcTextForTask1 = "Task 1 is running\n";
const char *pcTextForTask2 = "Task 2 is running\n";

static void vTask(void *pvParameters)
{
    char *pcTaskname = (char *)pvParameters;

    for (;;) {
        vDisplayMessage(pcTaskname);

        vTaskDelay(1000 / portTICK_RATE_MS);
        /*
        volatile unsigned long ul;
        for (ul=0; ul<mainDELAY_LOOP_COUNT; ul++) {
            // do nothing
        }
        */
    }
}

/* This is just an example implementation of the idle hook. */
void vApplicationIdleHook( void )
{
}

/* This is just an example implementation of the "queue send" trace hook. */
void vMainQueueSendPassed( void )
{
}

int main(void)
{
    /* Initialise hardware and utilities. */
    
    /* create vTask1, stack 1000 words, no parameters, priority 1
     * no task handler */
    xTaskCreate(vTask, "Task 1", mainTASK1_STACK_SIZE, (void*)pcTextForTask1, mainTASK1_PRIORITY, NULL);

    /* task 2 */
    xTaskCreate(vTask, "Task 2", mainTASK1_STACK_SIZE, (void*)pcTextForTask2, mainTASK2_PRIORITY, NULL);

    /* start the scheduler so the tasks start executing */
    vTaskStartScheduler();

    return 1;
}
