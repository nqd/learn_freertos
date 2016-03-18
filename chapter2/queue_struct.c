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

#define mainTASK_SENDER_PRIORITY        (tskIDLE_PRIORITY + 2)
#define mainTASK_RECEIVER_PRIORITY      (tskIDLE_PRIORITY + 1)

#define mainTASK_STACK_SIZE        ((unsigned short) 2048)

#define mainSENDER_1    1
#define mainSENDER_2    2

#define mainQUEUE_SIZE  3

typedef struct {
    unsigned char ucValue;
    unsigned char ucType;
} xData;

static const xData xStructToSend[2] =
{
    {100, mainSENDER_1},
    {200, mainSENDER_2}
};

static xQueueHandle xQueue;

static void vSenderTask(void *pvParameters)
{
    portTickType xTicksToWait = 100/portTICK_RATE_MS;

    portBASE_TYPE xStatus;

    for (;;) {
        xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);

        if (xStatus != pdPASS) {
            vDisplayMessage("Sender: Could not send to queue\n");
        }

        // vTaskDelay(1000 / portTICK_RATE_MS);
        taskYIELD();
    }
}

static void vReceiverTask(void *pvParameters)
{
    xData lValueToGet;
    portBASE_TYPE xStatus;

    /* to avoid warning */
    (void)pvParameters;

    for (;;) {

        if (uxQueueMessagesWaiting(xQueue) != mainQUEUE_SIZE) {
            vDisplayMessage("Queue should have been full\n");
        }

        xStatus = xQueueReceive(xQueue, &lValueToGet, 0);

        if (xStatus == pdPASS) {
            char pcMessage[100];
            snprintf(pcMessage, sizeof(pcMessage), "Receiver: %d\n", lValueToGet.ucValue);

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
    xQueue = xQueueCreate(mainQUEUE_SIZE, sizeof(xData));

    /* the queue could not be created */
    if (xQueue == NULL) {
        return 1;
    }
    
    /* create two instances of queue sending task 
     * no task handler */
    xTaskCreate(vSenderTask, "Sender 1", mainTASK_STACK_SIZE, (void*)&xStructToSend[0], mainTASK_SENDER_PRIORITY, NULL);
    xTaskCreate(vSenderTask, "Sender 2", mainTASK_STACK_SIZE, (void*)&xStructToSend[1], mainTASK_SENDER_PRIORITY, NULL);

    /* create receiving task */
    xTaskCreate(vReceiverTask, "Receiver", mainTASK_STACK_SIZE, NULL, mainTASK_RECEIVER_PRIORITY, NULL);

    /* start the scheduler so the tasks start executing */
    vTaskStartScheduler();

    return 1;
}
