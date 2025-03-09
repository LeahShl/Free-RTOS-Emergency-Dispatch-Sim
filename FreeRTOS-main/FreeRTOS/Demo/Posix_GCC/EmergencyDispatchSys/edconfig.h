/**
 * @file edconfig.h
 * @author Leah
 * @brief configuration for emergency dispatch system
 * 
 */

/**
 * Emergency codes
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
#define MIN_GEN_S 1 // Min amount of seconds for event generation
#define MAX_GEN_S 5 // Max amount of seconds for event generation
#define EVNT_HNDL_T 3 // # of ticks for event handling

/**
 * Priorities
 */
#define PR_GEN 3 // Priority of event generator
#define PR_DIS 3 // Priority of dispatcher
#define PR_PLC 3 // Priority of police 
#define PR_AMB 3 // Priority of ambulance
#define PR_FIR 3 // Priority of fire department
#define PR_LOG 3 // Priority of logger