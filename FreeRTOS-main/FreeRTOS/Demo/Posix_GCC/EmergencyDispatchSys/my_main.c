#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "edconfig.h"

typedef struct EmergencyEvent_t
{
    int event_id;
} EmergencyEvent_t;

// globals
QueueHandle_t xEventQueue;

void Task_Generator(void *pvParameters)
{
    srand(time(NULL));

    for (;;)
    {
        EmergencyEvent_t event;
        event.event_id = rand() % 3 + 1;
        
        xQueueSend(xEventQueue, &event, portMAX_DELAY);

        int delay = rand() % MAX_DELAY + MIN_DELAY;
        vTaskDelay(pdMS_TO_TICKS(delay));
    }

}

void Task_DebugLogger(void *pvParameters)
{
    EmergencyEvent_t event;

    for (;;)
    {
        if (xQueueReceive(xEventQueue, &event, portMAX_DELAY))
        {
            printf("Event code %d\n", event.event_id);
            vTaskDelay(EVNT_HNDL_T);
        }
    }
 
}

void my_main(void)
{
    // Message queue
    xEventQueue = xQueueCreate(EVNTQ_SIZE, sizeof(EmergencyEvent_t));
    if (xEventQueue == NULL)
    {
        perror("Failed to create queue \n");
    }

    xTaskCreate(Task_Generator, "Generator", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(Task_DebugLogger, "Debug Logger", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

    vTaskStartScheduler();

    printf("Scheduler failed \n");
    while(1);
}
