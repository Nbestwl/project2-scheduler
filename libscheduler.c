/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements.
*/


job_t **job;
priqueue_t *jobQueue;
scheme_t s;
int coreNum;
float waitingTime, turnaroundTime, responseTime;
int job_number;
int preemptive;

int cFCFS (const void * a, const void * b)
{

  return (((job_t*)a)->arrival_time - ((job_t*)b)->arrival_time);

}

int cSJF (const void * a, const void * b)
{

  return (((job_t*)a)->run_time - ((job_t*)b)->run_time);

}

int cPSFJ (const void * a, const void * b)
{

  return (((job_t*)a)->run_time - ((job_t*)b)->run_time);

}

int cPRI (const void * a, const void * b)
{

  return (((job_t*)a)->priority - ((job_t*)b)->priority);

}

int cPPRI (const void * a, const void * b)
{

  return (((job_t*)a)->priority - ((job_t*)b)->priority);

}

int cRR (const void * a, const void * b)
{
  if(((job_t*)a)->job_id == ((job_t*)b)->job_id)
    return 0;

  //return (((job_t*)a)->arrival_time - ((job_t*)b)->arrival_time);
  return -1;
}


/**
  Initalizes the scheduler.
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.
  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  waitingTime = 0.0;
  turnaroundTime = 0.0;
  responseTime = 0.0;
  job_number = 0;

  coreNum = cores;

  job = malloc(cores * sizeof(job_t));
  jobQueue = malloc(sizeof(priqueue_t));


  int x;
  for(x = 0; x < coreNum; x++){
    job[x] = NULL;
  }

  s = scheme;

  if(s == FCFS){

    priqueue_init(jobQueue, cFCFS);
    preemptive = 0;
  }else if(s == SJF){

    priqueue_init(jobQueue, cSJF);
    preemptive = 0;
  }else if(s == PSJF){

    priqueue_init(jobQueue, cPSFJ);
    preemptive = 1;
  }else if(s == PRI){

    priqueue_init(jobQueue, cPRI);
    preemptive = 0;
  }else if(s == PPRI){

    priqueue_init(jobQueue, cPPRI);
    preemptive = 1;
  }else if(s == RR){

    priqueue_init(jobQueue, cRR);
    preemptive = 0;
  }

}




/**
  Called when a new job arrives.
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.
  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param run_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.
 */
int scheduler_new_job(int job_number, int time, int run_time, int priority)
{

  job_t *new_job;
  new_job = malloc(sizeof(job_t));

  new_job->job_id = job_number;
  new_job->arrival_time = time;
  new_job->run_time = run_time;
  new_job->priority = priority;

  int tempIndex = -1;

  int x;

  int full = 1;
  int index = -1;
  for(x = 0; x < coreNum; x++){

    if(job[x] == NULL){
      job[x] = new_job;
      full = 0;
      index = x;
      break;
    }

  }

  if(full == 1){

    if(s == PSJF){

      int longestJob = 0;

      for(x = 0; x < coreNum - 1; x++){

        if(job[x + 1] == NULL){
          break;
        }

        if((job[x + 1]->arrival_time + job[x + 1]->run_time) - time > (job[x + 1]->arrival_time + job[x + 1]->run_time) - time){
          longestJob = x + 1;
        }

      }

        if(((job[longestJob]->arrival_time + job[longestJob]->run_time) - time) > run_time){
          job_t *temp = job[longestJob];
          job[longestJob] = new_job;
          new_job = temp;
          index = longestJob;

        }

    }else if(s == PPRI){

      int largestPriority = 0;

      for(x = 0; x < coreNum - 1; x++){

        if(job[x + 1] == NULL){
          break;
        }

        if(job[x + 1]->priority > job[x]->priority){
          largestPriority = x + 1;
        }

      }

        if(job[largestPriority]->priority > new_job->priority){
          job_t *temp = job[largestPriority];
          job[largestPriority] = new_job;
          new_job = temp;
          index = largestPriority;

        }

    }


    priqueue_offer(jobQueue, new_job);

  }



  return index;
}


/**
  Called when a job has completed execution.
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  job_t *finJob = job[core_id];
  job_number++;
  waitingTime += (time - finJob->arrival_time - finJob->run_time);
  turnaroundTime += (time - finJob->arrival_time);

  free(job[core_id]);
  job[core_id] = NULL;

  void *tempValues = priqueue_poll(jobQueue);


  if(tempValues != NULL){
    scheduler_new_job(((job_t *)tempValues)->job_id, ((job_t *)tempValues)->arrival_time, ((job_t *)tempValues)->run_time, ((job_t *)tempValues)->priority);
    return ((job_t *)tempValues)->job_id;
  }

  return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.
  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{

  job_t *temp = job[core_id];
  job[core_id] = NULL;

  priqueue_offer(jobQueue, temp);



  void *frontJob = priqueue_poll(jobQueue);

  if(frontJob != NULL){
    scheduler_new_job(((job_t *)frontJob)->job_id, time, ((job_t *)frontJob)->run_time, ((job_t *)frontJob)->priority);
    return ((job_t *)frontJob)->job_id;
  }


  return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
  if(job_number > 0)
  {
    return waitingTime/job_number;
  }
  return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
  if(job_number > 0)
  {
    return turnaroundTime/job_number;
  }
  return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
  if(!preemptive)
  {
    return waitingTime/job_number;
  }
  else
  {
    return 0.0;
  }
}


/**
  Free any memory associated with your scheduler.
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  priqueue_destroy(jobQueue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:
    2(-1) 4(0) 1(-1)
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  int x = 0;
  int size = priqueue_size(jobQueue);
  if(size == 0)
  {
    printf("Queue is empty");
    return;
  }
  while(x < size)
  {
    job_t* job = (job_t*)priqueue_at(jobQueue, x);
    printf("Index: %d Job Number:%d Arrival Time: %d Running Time: %d Priority: %d\n",
           x, job->job_id, job->arrival_time, job->run_time, job->priority);
    x++;
  }
}