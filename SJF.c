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
	
	int64_t sorter;
	
	jobDataType jobArray[total_jobs];
	
	for(int i = 0; i < init_jobs; i++)
	{		
		jobArray[i].creationTime = 0;
		jobArray[i].burstTime = rand_exp(prob_compute_time)*1000000;
	}
	/*
	for(int i = 0; i < init_jobs; i++)
	{
		for(int j = i + 1; j < init_jobs; j ++)
		{
			if(jobArray[i].burstTime > jobArray[j].burstTime)
			{
				sorter = jobArray[i].burstTime;
				jobArray[i].burstTime = jobArray[j].burstTime;
				jobArray[j].burstTime = sorter;
			}
		}
	}
	*/
	int64_t currentTime = 0;
	int64_t processTime = 0;
	int64_t endTime = 0;
		
	int i = 0;
	int j = init_jobs;
	
	while(i < total_jobs)
	{	
		currentTime++;
		processTime++;
		
		if((currentTime % tick_time == 0) && j < total_jobs)
		{
			if(random() % 100 == 15)
			{
				jobArray[j].creationTime = currentTime;
				jobArray[j].burstTime = rand_exp(prob_compute_time)*1000000;
				j++;
			}
		}
			
		if(processTime == jobArray[i].creationTime + jobArray[i].burstTime)
		{			
			if(i == 0)
			{
				jobArray[i].waitTime = cs_time + sched_time;
			}
			else
			{	
				jobArray[i].waitTime =    endTime 
										- jobArray[i].creationTime 
										+ cs_time + sched_time
										+ jobArray[i - 1].waitTime;
			}
			endTime = processTime;
			
			jobArray[i].turnaroundTime 	=	jobArray[i].waitTime 
								        +	jobArray[i].burstTime;

			i++;	
			
			processTime = jobArray[i].creationTime;
		}
	}
	
	int64_t sum1 = 0;
	int64_t sum2 = 0;
	
	for(int i = 0; i < total_jobs; i++)
	{
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



















