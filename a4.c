/*
* File:		A4.c
* Author:	Devon Allie
* Date:		June 4th 2020
* Version:	2.0
*
* Purpose:	The goal of this program is to explore the utility of a mutex when
*			developing multithreaded programs. The program creates a specified
*			number of threads that wait in the thread_code() function. The
*			threads wait for the first thread in main() to change the shared
*			variable, shared_variable, to 1. When a thread detects that
*			shared_variable has changed to 0, it attempt to change
*			shared_variable back to 0. All threads attempt this simultaneously
*			leading to descrepencies between the number of times
*			shared_variable is to 0 by the threads and the number of times
*			it is set to 1 by main. This competition is iterated based on the
*			iteration argument given at run-time. The inclusion of a mutex lock
*			prevents more than one thread to change the variable at a time. 
*
* Notes:	The program checks for the presents of a -sync argument. If
*			present, the mutex functionality is enabled. The next two arguments
*			become the number of threads and iterations respectively. 
*/







#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<stdbool.h>
#include<string.h>

#define THREAD_MAX 200
#define THREAD_DEFAULT 5
#define ITERATION_DEFAULT 1000
#define USEC_SLEEP 10

bool syncFlag = false;
pthread_mutex_t lock;
int shared_data;

/*
* When a thread is created it prints to the terminal and runs in the first
* while loop and checks if shared_data has been changed to 1. If
* shared_data remains at 0, the thread waits 10 microseconds before checking
* again. If the variable has changed, the thread attempts to change 
* shared_variable back to 0 and incrememnt its personal account of the 
* number of changes it has made. If the "-sync" argument is given, the mutex
* prevents all other threads from modifying shared_variable once a single
* thread tries to change it. When main() makes shared_variable 2, the thread
* ends and returns the number of times the thread has changed shared_variables.
*/

void* thread_code(void * p)
{
	int count = 0;
	int id = *(int *)p;
	printf("I am thread %d, starting up now\n", id);
	while(shared_data != 2)
	{
		if(syncFlag)pthread_mutex_lock(&lock);
		if(shared_data == 1)
		{
			shared_data = 0;
			count++;
		}
		if(syncFlag)pthread_mutex_unlock(&lock); 
		usleep(USEC_SLEEP);
	}
	
	printf("I am thread %d; I have change the value %d times\n", id, count);
	pthread_exit((void*)(intptr_t)count);
}

/*
* main() takes at most 3 arguments. If "-sync" is an argument, the syncFlag
* is set. The following arguments determine the number of threads and
* iterations respectively. main() runs a loop that creates the number of
* threads indicated by threadCount. Each thread is given a unique id and
* is sent to thread_code() to wait for shared_variable to change. main()
* then changes shared_variable back to 1 for the amount set by iterationCount.
* Once main() has iterated the desired number of times, shared_variable
* is set to 2, allowing all the threads to exit. main() joins the threads
* and summates their individual counts. If "-sync" is set, the mutex is then
* destroyed.
*/


int main(int argc, char * argv[])
{
	pthread_t thread[THREAD_MAX];
	int threadCount, iterationCount;
	int i = 0;
	int id[THREAD_MAX];
	int threadIterations = 0;
	void* count;
	
	threadCount = THREAD_DEFAULT;
	iterationCount = ITERATION_DEFAULT;
	shared_data = 0;
	
	switch(argc)
	{
		case 3:
		threadCount = strtoul(argv[1], NULL, 10);
		iterationCount = strtoul(argv[2], NULL, 10);
		break;
		
		case 4:
		switch(strcmp(argv[1], "-sync"))
		{
			case 0:
			pthread_mutex_init(&lock, NULL);
			syncFlag = true;
			break;
			
			default:
			printf("With 3 arguments, the first must be the sync flag\n");
			break;
		}
		threadCount = strtoul(argv[2], NULL, 10);
		iterationCount = strtoul(argv[3], NULL, 10);
		break;
		
		default:
		printf("Incorrect number of arguments\n");
		printf("The thread count will default to %d\n", THREAD_DEFAULT);
		printf("The iteration count will default to %d\n", ITERATION_DEFAULT);
	}

	while(i < threadCount)
	{	

		id[i] = i;
		pthread_create(&thread[i], NULL, thread_code, &id[i]);
		i++;
	}

	i = 0;

	while(i < iterationCount)
	{		
		if(shared_data == 0)
		{
			shared_data = 1;
			i++;
		}
		usleep(USEC_SLEEP);
	}

	shared_data = 2;

	for(int i = 0; i < threadCount; i++)
	{
		pthread_join(thread[i], &count);
		threadIterations += (intptr_t)count;
	}
	pthread_mutex_destroy(&lock);
	printf("main(): setting sv to 2 and joining the threads\n");
	printf("main(): all threads have finished\n");
	printf("main(): I set sv to 1 %d times, ", iterationCount);
	printf("the threads reset it %d times\n", threadIterations);
	
	return 0;
}	











