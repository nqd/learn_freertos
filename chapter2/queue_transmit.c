#include <stdio.h>
#include <stdlib.h>

/* freertos kernel */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

/* for print */
#include "fileIO.h"

static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);

void vApplicationIdleHook( void );

#define mainTASK_SENDER_PRIORITY        (tskIDLE_PRIORITY + 1)
#define mainTASK_RECEIVER_PRIORITY      (tskIDLE_PRIORITY + 2)

#define mainTASK_STACK_SIZE        ((unsigned short) 2048)

static xQueueHandle xQueue;

static void vSenderTask(void *pvParameters)
{
    long lValueToSend = (long) pvParameters;
    portBASE_TYPE xStatus;

    for (;;) {
        xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);

        if (xStatus != pdPASS) {
            vDisplayMessage("Sender: Could not send to queue\n");
        }

        // vTaskDelay(1000 / portTICK_RATE_MS);
        taskYIELD();
    }
}

static void vReceiverTask(void *pvParameters)
{
    long lValueToGet;
    portBASE_TYPE xStatus;
    portTickType xTicksToWait;

    xTicksToWait = 100/portTICK_RATE_MS;

    /* to avoid warning */
    (void)pvParameters;

    for (;;) {

        xStatus = xQueueReceive(xQueue, &lValueToGet, xTicksToWait);

        if (xStatus == pdPASS) {
            char pcMessage[100];
            snprintf(pcMessage, sizeof(pcMessage), "Receiver: %ld\n", lValueToGet);

            vDisplayMessage(pcMessage);
        }
        else if (xStatus == errQUEUE_EMPTY) {
            vDisplayMessage("Receiver: Buffer empty\n");
        }
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

    /* create queue */
    xQueue = xQueueCreate(5, sizeof(long));

    /* the queue could not be created */
    if (xQueue == NULL) {
        return 1;
    }
    
    /* create two instances of queue sending task 
     * no task handler */
    xTaskCreate(vSenderTask, "Sender 1", mainTASK_STACK_SIZE, (void*)100, mainTASK_SENDER_PRIORITY, NULL);
    xTaskCreate(vSenderTask, "Sender 2", mainTASK_STACK_SIZE, (void*)200, mainTASK_SENDER_PRIORITY, NULL);

    /* create receiving task */
    xTaskCreate(vReceiverTask, "Receiver", mainTASK_STACK_SIZE, NULL, mainTASK_RECEIVER_PRIORITY, NULL);

    /* start the scheduler so the tasks start executing */
    vTaskStartScheduler();

    return 1;
}
