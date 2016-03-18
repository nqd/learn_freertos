/* gatekeeper tasks
 * the gatekeeper task has sole ownership of printf
 * other tasks need to access printf can only do by using
 * the services of the gatekeeper
 */
#include <stdio.h>
#include <stdlib.h>

/* freertos kernel */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

/* for print */
#include "fileIO.h"

static void vPrintTask(void *pvParameters);

void vApplicationIdleHook(void);
void vApplicationTickHook(void);

#define mainTASK1_PRIORITY       (tskIDLE_PRIORITY + 1)
#define mainTASK2_PRIORITY       (tskIDLE_PRIORITY + 2)
#define mainTASK_STDIO_GATEKEEPER_TASK_PRIORITY (tskIDLE_PRIORITY + 0)

#define mainTASK1_STACK_SIZE        ((unsigned short) 2048)
#define mainQUEUE_MAX            5

xQueueHandle xPrintQueue;

static char *pcStringToPrint[] = {
    "Task 1 *********************\n",
    "Task 2 ---------------------\n",
    "Tick hook interrupt ########\n"
};

/*
void vApplicationTickHook(void)
{
    static int iCount = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    if (++iCount >= 200) {
        xQueueSendToFrontFromISR(xPrintQueue,
            &pcStringToPrint[2],
            &xHigherPriorityTaskWoken);

        iCount = 0;
    }
}
*/

static void vStdioGatekeeperTask(void *pvParameters)
{
    char *pcString;

    /* avoid warning */
    (void) pvParameters;

    for (;;) {
        xQueueReceive(xPrintQueue, &pcString, portMAX_DELAY);

        printf("%s", pcString);
        fflush(stdout);
    }
}

static void vPrintTask(void *pvParameters)
{
    int iIndex = (int)pvParameters;

    for (;;) {
        // vDisplayMessage(pcTaskname);

        xQueueSendToBack(xPrintQueue, &pcStringToPrint[iIndex], 0);

        vTaskDelay(rand() & 0x1ff);
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

    /* create printing queue */
    xPrintQueue = xQueueCreate(mainQUEUE_MAX, sizeof (char*));

    if (xPrintQueue == NULL) {
        while(1);
    }

    /* seed the random generator */
    srand(123);

    xTaskCreate(vPrintTask, "Print 1", mainTASK1_STACK_SIZE, (void*) 0, mainTASK1_PRIORITY, NULL);

    /* task 2 */
    xTaskCreate(vPrintTask, "Print 2", mainTASK1_STACK_SIZE, (void*) 1, mainTASK2_PRIORITY, NULL);

    xTaskCreate(vStdioGatekeeperTask, "Stdio gatekeeper task", mainTASK1_STACK_SIZE, NULL, mainTASK_STDIO_GATEKEEPER_TASK_PRIORITY, NULL);

    /* start the scheduler so the tasks start executing */
    vTaskStartScheduler();

    return 1;
}
