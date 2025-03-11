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
#define PR_GEN 4 // Priority of event generator
#define PR_DIS 5 // Priority of dispatcher
#define PR_PLC 3 // Priority of police 
#define PR_AMB 6 // Priority of ambulance
#define PR_FIR 7 // Priority of fire department
#define PR_LOG 2 // Priority of logger

/**
 * Misc.
 */
#define EVNTQ_SIZE 50 // Size of the event queue