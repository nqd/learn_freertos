#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "partest.h"

#include "BlockQ.h"
#include "blocktim.h"
#include "countsem.h"
#include "crflash.h"
#include "crhook.h"
#include "death.h"
#include "dynamic.h"
#include "fileIO.h"
#include "flop.h"
#include "GenQTest.h"
#include "integer.h"
#include "mevents.h"
#include "PollQ.h"
#include "print.h"
#include "QPeek.h"
#include "recmutex.h"
#include "semtest.h"

static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);

void vApplicationIdleHook( void );


#define mainTASK1_PRIORITY       (tskIDLE_PRIORITY + 1)
#define mainTASK1_STACK_SIZE        ((unsigned short) 2048)


static void vTask1(void *pvParameters)
{
    const char *pcTaskName = "Task1 is running\r\n";

    // get rid of unused parameter warning
    (void) pvParameters;

    for (;;) {
        vDisplayMessage(pcTaskName);

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

static void vTask2(void *pvParameters)
{
    const char *pcTaskName = "Task2 is running\r\n";

    // get rid of unused parameter warning
    (void)pvParameters;

    for (;;) {
        vDisplayMessage(pcTaskName);

        vTaskDelay(1000 / portTICK_RATE_MS);
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
    vPrintInitialise();

    /* create vTask1, stack 1000 words, no parameters, priority 1
     * no task handler */
    xTaskCreate(vTask1, "Task 1", mainTASK1_STACK_SIZE, NULL, mainTASK1_PRIORITY, NULL);

    /* task 2 */
    xTaskCreate(vTask2, "Task 2", mainTASK1_STACK_SIZE, NULL, mainTASK1_PRIORITY, NULL);

    /* start the scheduler so the tasks start executing */
    vTaskStartScheduler();

    return 1;
}
