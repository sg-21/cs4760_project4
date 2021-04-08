#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "oss.h"

pcb_t *attach_pcb_table();
simtime_t *attach_sim_clock();
void get_clock_and_table(int n);
int get_outcome();

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: ./user pid msqid quantum\n");
    exit(EXIT_SUCCESS);
  }
  int pid;
  mymsg_t msg;
  int quantum;
  int outcome = 0;
  pid = atoi(argv[1]);
  msqid = atoi(argv[2]);
  quantum = atoi(argv[3]);
  srand(time(0) + (pid + 1));  /* seeding rand. seeding w/ time(0) caused processes spawned too close to have same seed*/

  pcb_t *table;
  simtime_t *simClock;
  simtime_t timeBlocked; /*holds the time that the process was blocked at*/
  simtime_t event;  /*time of the event that will unblock the process*/
  int burst;//burst to calculate unblock time

  get_clock_and_table(pid);
  table = attach_pcb_table();
  simClock = attach_sim_clock();

  /* while loop to wait for messages from oss until we terminate */
  while (outcome != 1) {  
    if ((msgrcv(msqid, &msg, sizeof(msg.mvalue), (pid + 1), 0)) == -1) {
      perror("./user: Error: msgrcv ");
      exit(EXIT_FAILURE);
    }
    
    outcome = get_outcome();

    switch (outcome) {
      case 0:  /* full */
        msg.mvalue = 100;
        break;
      case 1:  /* term */
        msg.mvalue = (rand() % 99) + 1;
        break;
      case 2:  /* block */
        msg.mvalue = ((rand() % 99) + 1) * -1;
        timeBlocked.s = simClock->s;
        timeBlocked.ns = simClock->ns;
        burst = msg.mvalue * (quantum / 100) * pow(2.0, (double)table[pid].priority);
        event.s = (rand() % 4) + 1;
        event.ns = (rand() % 1000) * 1000000; //generating s [0, 999]. * 1000000 to convert to ns

        /* add to wait time total */
        table[pid].waitTime = add_sim_times(table[pid].waitTime, event);
        event = add_sim_times(event, timeBlocked);
        increment_sim_time(&event, (burst * -1));

        /* set status to blocked before telling oss to avoid race condition. OSS is waiting for a message response so it*/
        table[pid].isReady = FALSE;  
        break;

        default:
        break;
    }
                          
    msg.mtype = pid + 100;  /* oss is waiting for a msg w/ type pid+100 */
    
    if (msgsnd(msqid, &msg, sizeof(msg.mvalue), 0) == -1) {
      perror("./user: Error: msgsnd ");
      exit(EXIT_FAILURE);
    }
    
    /* Blocked Outcome */
    if (outcome == 2) {
      /* while loop to wait for event time to pass*/
      while(table[pid].isReady == FALSE){
        /*printf("waiting %ds%9dns\n", event.s, event.ns);*/
        if (event.s > simClock->s) {
          table[pid].isReady = TRUE;
        }
        else if (event.ns >= simClock->ns && event.s >= simClock->s) {
          table[pid].isReady = TRUE;
        }
      }
    }
  }
  return 0;
}

pcb_t *attach_pcb_table() {
  pcb_t *pcbTable;
  pcbTable = shmat(pcbTableId, NULL, 0);

  /* error */
  if (pcbTableId < 0) {  
    perror("./user: Error: shmat ");
    exit(EXIT_FAILURE);
  }
  return pcbTable;
}

simtime_t *attach_sim_clock() {
  simtime_t *simClock;
  simClock = shmat(clockId, NULL, 0);

  /* error */
  if (clockId < 0) {  
    perror("./user: Error: shmat ");
    exit(EXIT_FAILURE);
  }
  return simClock;
}

void get_clock_and_table(int n) {
  /* Getting shared memory for the simulated clock*/
  clockId = shmget(CLOCK_KEY, sizeof(simtime_t), IPC_CREAT | 0777);

  if (clockId < 0) {  
    perror("./user: Error: shmget ");
    exit(EXIT_FAILURE);
  }

  /* Getting shared memory for the pcb table*/
  pcbTableId = shmget(PCB_TABLE_KEY, sizeof(pcb_t) * (s + 1), IPC_CREAT | 0777);
  if (pcbTableId < 0) {
    perror("./user: Error: shmget ");
    exit(EXIT_FAILURE);
  }
  return;
}

/* 0: not terminating or blocked, 1: terminating, 2:not terminating but blocked*/
int get_outcome() {
  int tPercent = 5;   /*% chance of terminating*/
  int bPercent = 5;  /*% chance of getting blocked*/
  int terminating = ((rand() % 100) + 1) <= tPercent ? TRUE : FALSE;
  int blocked = ((rand() % 100) + 1) <= bPercent ? TRUE : FALSE;

  if (terminating) 
    return 1;
  if (blocked) 
    return 2;
  return 0;
}
