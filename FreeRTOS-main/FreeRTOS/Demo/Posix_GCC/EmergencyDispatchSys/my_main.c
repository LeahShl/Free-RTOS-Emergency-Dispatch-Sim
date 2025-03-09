#include <stdio.h>
#include <unistd.h>
//#include <bool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define QUEUE_LENGTH 5
#define STR_SIZE 100
#define CALL_DELAY 3000
#define DISPATCH_DELAY 2000
#define TECH_DELAY 5000

typedef struct ServiceCall_t
{
    int call_id;
    char issue_description[STR_SIZE];
} ServiceCall_t;

// globals
QueueHandle_t xServiceCallQueue;
SemaphoreHandle_t xServiceCallMutex;
int currentCall = 0;

void Task_CallCenter(void *pvParameters)
{
    int call_id = 0;
    for (;;)
    {
        ServiceCall_t serviceCall;
        serviceCall.call_id = call_id++;
        snprintf(serviceCall.issue_description, sizeof (serviceCall.issue_description), "Issue %d \n", serviceCall.call_id);
        if(xQueueSend(xServiceCallQueue, &serviceCall, portMAX_DELAY) != pdPASS)
        {
            printf("Failed sending service call to queue\n");
        }

        vTaskDelay(pdMS_TO_TICKS(CALL_DELAY));
    }

}

void Task_Dispatcher(void *pvParameters)
{
    ServiceCall_t serviceCall;
    for (;;)
    {
        if (xQueueReceive(xServiceCallQueue, &serviceCall, portMAX_DELAY) == pdPASS)
        {
            printf("[Dispatcher] Assigned issue %d to Technician\n", serviceCall.call_id);
            vTaskDelay(pdMS_TO_TICKS(DISPATCH_DELAY));
        }
        
    }

}

void Task_Technician(void *pvParameters)
{
    for (;;)
    {
        printf("[Technician] waiting for a service call...\n");
        if (xSemaphoreTake(xServiceCallMutex, portMAX_DELAY) == pdPASS)
        {
            currentCall = 42;
        }
        xSemaphoreGive(xServiceCallMutex);
        printf("[Technician] finished service call...\n");
        vTaskDelay(pdMS_TO_TICKS(TECH_DELAY));
    }
 
}

void Task_EquipmentManager(void *pvParameters)
{
    for (;;)
    {
        printf("EquipmentManager is working... \n");
        sleep(1); 
    }

}

void Task_Logger(void *pvParameters)
{
    for (;;)
    {
        printf("Logger is working... \n");
        sleep(1); 
    }
}

void error(const char *msg)
{
    printf("ERROR: %s\n", msg);
    for(;;);
}

void main_myproject(void)
{
    // Message queue
    xServiceCallQueue = xQueueCreate(QUEUE_LENGTH, sizeof(ServiceCall_t));
    if (xServiceCallQueue == NULL)
    {
        error("Failed to create queue \n");
    }

    // Create mutex
    xServiceCallMutex = xSemaphoreCreateMutex();
    if (xServiceCallMutex == NULL)
    {
        error("Failed to create mutex \n");
    }

    // Create tasks - lastly created before calling scheduler
    xTaskCreate(Task_CallCenter, "CallCenter", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(Task_Dispatcher, "Dispatcher", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(Task_Technician, "Technician", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(Task_EquipmentManager, "EquipmentManager", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(Task_Logger, "Logger", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start scheduler - always the last line
    vTaskStartScheduler();

    // if the system reaches this point, the scheduler has failed
    printf("Scheduler failed \n");
    while(1);
}
