#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "edconfig.h"

typedef struct EmergencyEvent_t
{
    int event_id;
} EmergencyEvent_t;

typedef struct LogEvent_t
{
    int event_id;
    int log_type;
    char msg[LOG_LEN];
} LogEvent_t;

QueueHandle_t xEventQueue;
QueueHandle_t xPoliceQueue;
QueueHandle_t xAmbulanceQueue;
QueueHandle_t xFireDeptQueue;
QueueHandle_t xLogQueue;

void Task_Generator(void *pvParameters)
{
    srand(time(NULL));

    for (;;)
    {
        EmergencyEvent_t event;
        event.event_id = rand() % 3 + 1;

        LogEvent_t log;
        log.event_id = event.event_id;
        log.log_type = LOG_CREATE;
        strcpy(log.msg, "[Generator] Created event");
        
        xQueueSend(xEventQueue, &event, portMAX_DELAY);
        xQueueSend(xLogQueue, &log, portMAX_DELAY);

        int delay = rand() % MAX_DELAY + MIN_DELAY;
        vTaskDelay(pdMS_TO_TICKS(delay));
    }

}

void Task_Dispatcher(void *pvParameters)
{
    EmergencyEvent_t event;

    for (;;)
    {
        if (xQueueReceive(xEventQueue, &event, portMAX_DELAY))
        {
            // TODO: send event to department

            LogEvent_t log;
            log.event_id = event.event_id;
            log.log_type = LOG_SENT;
            strcpy(log.msg, "[Dispatcher] Sent event");

            xQueueSend(xLogQueue, &log, portMAX_DELAY);
        }
    }
 
}

void Task_Logger(void *pvParameters)
{
    LogEvent_t log;

    for (;;)
    {
        if (xQueueReceive(xLogQueue, &log, portMAX_DELAY))
        {
            printf("Event type %d: %s [status: %d]\n", log.event_id, log.msg, log.log_type);
        }
    }
}

void my_main(void)
{
    xEventQueue = xQueueCreate(QUEUE_SIZE, sizeof(EmergencyEvent_t));
    if (xEventQueue == NULL)
    {
        perror("Failed to create event queue \n");
    }

    xPoliceQueue = xQueueCreate(QUEUE_SIZE, sizeof(EmergencyEvent_t));
    if (xPoliceQueue == NULL)
    {
        perror("Failed to create police queue \n");
    }

    xAmbulanceQueue = xQueueCreate(QUEUE_SIZE, sizeof(EmergencyEvent_t));
    if (xAmbulanceQueue == NULL)
    {
        perror("Failed to create ambulance queue \n");
    }

    xFireDeptQueue = xQueueCreate(QUEUE_SIZE, sizeof(EmergencyEvent_t));
    if (xFireDeptQueue == NULL)
    {
        perror("Failed to create fire department queue \n");
    }

    xLogQueue = xQueueCreate(QUEUE_SIZE, sizeof(LogEvent_t));
    if (xLogQueue == NULL)
    {
        perror("Failed to create log queue \n");
    }


    xTaskCreate(Task_Generator, "Generator", configMINIMAL_STACK_SIZE, NULL, PR_GEN, NULL);
    xTaskCreate(Task_Dispatcher, "Dispatcher", configMINIMAL_STACK_SIZE, NULL,PR_DIS, NULL);
    xTaskCreate(Task_Logger, "Logger", configMINIMAL_STACK_SIZE, NULL, PR_LOG, NULL);

    vTaskStartScheduler();

    printf("Scheduler failed \n");
    while(1);
}
