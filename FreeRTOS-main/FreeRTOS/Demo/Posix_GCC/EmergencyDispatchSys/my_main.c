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

SemaphoreHandle_t xSemPoliceCabs;
SemaphoreHandle_t xSemAmbulances;
SemaphoreHandle_t xSemFiretrucks;

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
        if (xQueueReceive(xEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            LogEvent_t log;
            log.event_id = event.event_id;

            switch (event.event_id)
            {
            case CODE_PLC:
                xQueueSend(xPoliceQueue, &event, portMAX_DELAY);
                log.log_type = LOG_SENT;
                strcpy(log.msg, "[Dispatcher] Sent event to police");
                break;

            case CODE_AMB:
                xQueueSend(xAmbulanceQueue, &event, portMAX_DELAY);
                log.log_type = LOG_SENT;
                strcpy(log.msg, "[Dispatcher] Sent event to first aid");
                break;

            case CODE_FIR:
                xQueueSend(xFireDeptQueue, &event, portMAX_DELAY);
                log.log_type = LOG_SENT;
                strcpy(log.msg, "[Dispatcher] Sent event to fire department");
                break;
            
            default:
                log.log_type = LOG_ERR;
                strcpy(log.msg, "[Dispatcher] Unknown event type");
                break;
            }

            xQueueSend(xLogQueue, &log, portMAX_DELAY);
        }
    }
 
}

void Task_Police(void *pvParameters)
{
    EmergencyEvent_t event;

    for(;;)
    {
        if(xQueueReceive(xPoliceQueue, &event, portMAX_DELAY) == pdPASS)
        {
            LogEvent_t log;
            log.event_id = event.event_id;
            log.log_type = LOG_PLC_RCV;
            strcpy(log.msg, "[Police] Received event");
            xQueueSend(xLogQueue, &log, portMAX_DELAY);

            if(xSemaphoreTake(xSemPoliceCabs, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_PLC_HNDL;
                strcpy(log.msg, "[Police] Finihed handling event");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemPoliceCabs);
            }
            else if (xSemaphoreTake(xSemAmbulances, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_PLC_HNDL;
                strcpy(log.msg, "[Police] Finihed handling event WITH BORROWED AMBULANCE");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemAmbulances);
            }
            else if (xSemaphoreTake(xSemFiretrucks, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_PLC_HNDL;
                strcpy(log.msg, "[Police] Finihed handling event WITH BORROWED FIRETRUCK");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemFiretrucks);
            }
        }
    }
}

void Task_Ambulance(void *pvParameters)
{
    EmergencyEvent_t event;

    for(;;)
    {
        if(xQueueReceive(xAmbulanceQueue, &event, portMAX_DELAY) == pdPASS)
        {
            LogEvent_t log;
            log.event_id = event.event_id;
            log.log_type = LOG_AMB_RCV;
            strcpy(log.msg, "[Ambulance] Received event");
            xQueueSend(xLogQueue, &log, portMAX_DELAY);

            if(xSemaphoreTake(xSemAmbulances, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_AMB_HNDL;
                strcpy(log.msg, "[Ambulance] Finihed handling event");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemAmbulances);
            }
            else if (xSemaphoreTake(xSemPoliceCabs, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_AMB_HNDL;
                strcpy(log.msg, "[Ambulance] Finihed handling event WITH BORROWED POLICE CAB");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemPoliceCabs);
            }
            else if (xSemaphoreTake(xSemFiretrucks, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_AMB_HNDL;
                strcpy(log.msg, "[Ambulance] Finihed handling event WITH BORROWED FIRETRUCK");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemFiretrucks);
            }
        }
    }
}

void Task_FireDepartment(void *pvParameters)
{
    EmergencyEvent_t event;

    for(;;)
    {
        if(xQueueReceive(xFireDeptQueue, &event, portMAX_DELAY) == pdPASS)
        {
            LogEvent_t log;
            log.event_id = event.event_id;
            log.log_type = LOG_FIR_RCV;
            strcpy(log.msg, "[Fire Dept] Received event");
            xQueueSend(xLogQueue, &log, portMAX_DELAY);

            if(xSemaphoreTake(xSemFiretrucks, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_FIR_HNDL;
                strcpy(log.msg, "[Fire Dept] Finihed handling event");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemFiretrucks);
            }
            else if(xSemaphoreTake(xSemAmbulances, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_FIR_HNDL;
                strcpy(log.msg, "[Fire Dept] Finihed handling event WITH BORROWED AMBULANCE");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemAmbulances);
            }
            else if (xSemaphoreTake(xSemPoliceCabs, portMAX_DELAY) == pdPASS)
            {
                vTaskDelay(pdMS_TO_TICKS(HNDL_DELAY));

                log.log_type = LOG_FIR_HNDL;
                strcpy(log.msg, "[Fire Dept] Finihed handling event WITH BORROWED POLICE CAB");
                xQueueSend(xLogQueue, &log, portMAX_DELAY);

                xSemaphoreGive(xSemPoliceCabs);
            }
        }
    }
}

void Task_Logger(void *pvParameters)
{
    LogEvent_t log;

    for (;;)
    {
        if (xQueueReceive(xLogQueue, &log, portMAX_DELAY) == pdPASS)
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

    xSemPoliceCabs = xSemaphoreCreateCounting(N_PCR, N_PCR);
    xSemAmbulances = xSemaphoreCreateCounting(N_AMB, N_AMB);
    xSemFiretrucks = xSemaphoreCreateCounting(N_FTR, N_FTR);

    xTaskCreate(Task_Generator, "Generator", configMINIMAL_STACK_SIZE, NULL, PR_GEN, NULL);

    xTaskCreate(Task_Dispatcher, "Dispatcher", configMINIMAL_STACK_SIZE, NULL, PR_DIS, NULL);
    xTaskCreate(Task_Police, "Police", configMINIMAL_STACK_SIZE, NULL, PR_PLC, NULL);
    xTaskCreate(Task_Ambulance, "Ambulance", configMINIMAL_STACK_SIZE, NULL, PR_AMB, NULL);
    xTaskCreate(Task_FireDepartment, "Fire Department", configMINIMAL_STACK_SIZE, NULL, PR_FIR, NULL);
    xTaskCreate(Task_Logger, "Logger", configMINIMAL_STACK_SIZE, NULL, PR_LOG, NULL);

    vTaskStartScheduler();

    printf("Scheduler failed \n");
    while(1);
}
