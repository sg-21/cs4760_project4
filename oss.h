/*Data structures and variables used for ipc*/
#ifndef OSS_H
#define OSS_H

#include <sys/types.h>

#define TRUE 1
#define FALSE 0

FILE *logFile;//log file
const key_t PCB_TABLE_KEY = 110594; /*key for shared PCB Table*/
const key_t CLOCK_KEY = 110197; /*key for shared simulated clock */
const key_t MSG_KEY = 052455; /*key for message queue*/
int pcbTableId; /*shmid for PCB Table*/
int clockId; /*shmid for simulated clock*/
int msqid; /*id for message queue*/

/*simulated time value used for the simulated clock*/
typedef struct {
  unsigned int s;
  unsigned int ns;
} simtime_t;

/*pseudo-process control block used for PCB Table*/
typedef struct {
  /*simulated process id, range is [0,5]*/
  int pid;
  /*Process priority*/
  int priority;
  /*if the process is ready to run*/
  int isReady;
  /*Arrivial time*/
  simtime_t arrivalTime;
  /*CPU time used*/
  simtime_t cpuTime;
  /*Time in the system*/
  simtime_t sysTime;
  /*Time used in the last burst*/
  simtime_t burstTime;
  /*Total sleep time. time waiting for an event*/
  simtime_t waitTime;


} pcb_t;

/*msg struct for msgqueue*/
typedef struct {
  long mtype;
  int mvalue;
} mymsg_t;

/*increment given simulated time by given increment*/
void increment_sim_time(simtime_t *simTime, int increment) {
  simTime->ns += increment;
  if (simTime->ns >= 1000000000) {
    simTime->ns -= 1000000000;
    simTime->s += 1;
  }
}
/* returns a - b*/
simtime_t subtract_sim_times(simtime_t a, simtime_t b) {
  simtime_t diff = { .s = a.s - b.s, 
                    .ns = a.ns - b.ns };
  if(diff.ns < 0) {
    diff.ns += 1000000000;
    diff.s -= 1;
  }
  return diff;
}
/*returns a + b*/
simtime_t add_sim_times(simtime_t a, simtime_t b) {
  simtime_t sum = { .s = a.s + b.s, 
                    .ns = a.ns + b.ns };
  if(sum.ns >= 1000000000) {
    sum.ns -= 1000000000;
    sum.s += 1;
  }
  return sum;
}

/*returns simtime / divisor*/
simtime_t divide_sim_time(simtime_t simTime, int divisor) {
  simtime_t quotient = { .s = simTime.s / divisor, .ns = simTime.ns / divisor };
  return quotient;
}

pcb_t create_pcb(int priority, int pid, simtime_t currentTime) {
  pcb_t pcb = { .pid = pid,
                .priority = priority,
                .isReady = TRUE, 
                .arrivalTime = {.s = currentTime.s, .ns = currentTime.ns}, 
                .cpuTime = {.s = 0, .ns = 0}, 
                .sysTime = {.s = 0, .ns = 0}, 
                .burstTime = {.s = 0, .ns = 0},
                .waitTime = {.s = 0, .ns = 0}};
  return pcb;
}

#endif
