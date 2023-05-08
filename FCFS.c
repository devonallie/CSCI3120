//FCFS Algorithm

#include    <inttypes.h>
#include    <math.h>
#include    <stdio.h>
#include    <stdlib.h>
#include	<time.h>

double rand_exp(double lambda)
{
    int64_t divisor = (int64_t)RAND_MAX + 1;
    double u_0_to_almost_1;
    double raw_value;

    u_0_to_almost_1 = (double)random() / divisor;

    raw_value = log(1 - u_0_to_almost_1) / -lambda;

    return round(raw_value * 1000000.) / 1000000.;
}

typedef struct jobDataType
{
	int64_t turnaroundTime;
	int64_t waitTime;
	int64_t responseTime;
	int64_t creationTime;
	int64_t burstTime;

}jobDataType;

int main(void)
{
	int64_t total_jobs = 100; //209399 is MAX
	
	int64_t init_jobs = 5;
	
	int64_t tick_time = 10*1000;
	
	int64_t sched_time = 10;
	
	int64_t cs_time = 50;
	
	int64_t global_time = 0;
	
	double prob_compute_time = 1;
	
	jobDataType jobArray[total_jobs];
	
	for(int i = 0; i < init_jobs; i++)
	{		
		jobArray[i].creationTime = 0;
		jobArray[i].burstTime = rand_exp(prob_compute_time)*1000000;
	}
	
	for(int i = init_jobs; i < total_jobs; i++)
	{
		while(random() % 100 != 15)
		{
			global_time += tick_time;
		}
		
		global_time += tick_time;
		
		jobArray[i].creationTime = global_time;
		jobArray[i].burstTime = rand_exp(prob_compute_time)*1000000;
		
	}
	
	int64_t currentTime = jobArray[0].creationTime + jobArray[0].burstTime;
	
	jobArray[0].turnaroundTime = jobArray[0].burstTime;
	jobArray[0].waitTime = 0;
	jobArray[0].responseTime = 0;
	for(int i = 1; i < total_jobs; i++)
	{	
		if(currentTime > jobArray[i].creationTime)
		{
			jobArray[i].waitTime = currentTime - jobArray[i].creationTime + sched_time + 2*cs_time; //wait for previous job's cs switch to scheduler, then wait for scheduler, then wait for current jobs cs switch to execute
			
			jobArray[i].responseTime = jobArray[i].waitTime;
			
			jobArray[i].turnaroundTime 	=	jobArray[i].waitTime 
										+	jobArray[i].burstTime;
										
			currentTime += jobArray[i].burstTime + sched_time + 2*cs_time;
		}
		else
		{	
			jobArray[i].waitTime = 0;
			
			jobArray[i].responseTime = 0;
			
			jobArray[i].turnaroundTime = jobArray[i].burstTime;
			
			currentTime = jobArray[i].creationTime + jobArray[i].burstTime;
		}
	}

	int64_t sum1 = 0;
	int64_t sum2 = 0;
	
	for(int i = 0; i < total_jobs; i++)
	{
		//printf("creation time: %ld burst time: %ld, wait time: %ld, turnaround time: %ld\n", jobArray[i].creationTime, jobArray[i].burstTime, jobArray[i].waitTime, jobArray[i].turnaroundTime);

		sum1 += jobArray[i].turnaroundTime;
		sum2 += jobArray[i].waitTime;
	}
	
	int64_t averageTurnaroundTime = sum1/total_jobs;
	int64_t averageWaitTime = sum2/total_jobs;
	int64_t averageResponseTime = averageWaitTime;
	
	printf("The average turn around time is:	%ld\n", averageTurnaroundTime);
	printf("The average wait time is:		%ld\n", averageWaitTime);
	printf("The average response time is:		%ld\n", averageResponseTime);

	return 0;
}



















