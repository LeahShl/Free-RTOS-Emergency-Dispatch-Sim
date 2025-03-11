/**
 * @file edconfig.h
 * @author Leah
 * @brief configuration for emergency dispatch system
 * 
 */

/**
 * Emergency events
 */
#define CODE_PLC 1 // Police event code
#define CODE_AMB 2 // Ambulance event code
#define CODE_FIR 3 // Fire event code

/**
 * Number of resources
 */
#define N_PCR 3 // Police cab #
#define N_AMB 4 // Ambulance #
#define N_FTR 2 // Fire truck #

/**
 * Time consts
 */
#define MIN_DELAY 1000 // Min amount of miliseconds for event generation
#define MAX_DELAY 5000 // Max amount of miliseconds for event generation
#define EVNT_HNDL_T 3 // number of ticks for event handling

/**
 * Priorities
 */
#define PR_GEN 3 // Priority of event generator
#define PR_DIS 5 // Priority of dispatcher
#define PR_PLC 4 // Priority of police 
#define PR_AMB 6 // Priority of ambulance
#define PR_FIR 7 // Priority of fire department
#define PR_LOG 2 // Priority of logger

/**
 * Log events
 */
#define LOG_LEN 256 // Log message length
#define LOG_CREATE 1 // Event created
#define LOG_SENT 2 // Event sent
#define LOG_PLC_HNDL 3 // Police handled event
#define LOG_AMB_HNDL 4 // Ambulance handled event
#define LOG_FIR_HNDL 5 // Fire department handled event
#define LOG_ERR 6 // Error


/**
 * Misc.
 */
#define QUEUE_SIZE 50 // Size of queue