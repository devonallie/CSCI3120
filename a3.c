/*
 * File:	
 * Author:	
 * Date:	
 * Version:	
 *
 * Purpose:
 */


#include    <stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include    <string.h>
#include    <inttypes.h>
#include    <math.h>
#include	<time.h>

#ifdef DEBUG
#define D_PRNT(...) fprintf(stderr, __VA_ARGS__)
#else
#define D_PRNT(...)
#endif

#define     DEFAULT_INIT_JOBS	    5
#define     DEFAULT_TOTAL_JOBS	    100
#define     DEFAULT_LAMBDA	    ((double)1.0)
#define     DEFAULT_SCHED_TIME	    10		    // usec
#define     DEFAULT_CONT_SWTCH_TIME 50		    // usec
#define     DEFAULT_TICK_TIME	    10		    // msec
#define     DEFAULT_PROB_NEW_JOB    ((double)0.15)
#define     DEFAULT_RANDOMIZE	    false

enum sched_alg_T { UNDEFINED, RR, SJF, FCFS };
char * alg_names[] = { "UNDEFINED", "RR", "SJF", "FCFS" };

double rand_exp(double lambda);

struct simulation_params
{
    enum sched_alg_T sched_alg;
    int init_jobs;
    int total_jobs;
    double lambda;
    int sched_time;
    int cont_swtch_time;
    int tick_time;
    double prob_new_job;
    bool randomize;
};

typedef struct jobDataType
{
	int64_t turnaroundTime;
	int64_t waitTime;
	int64_t responseTime;
	int64_t creationTime;
	int64_t burstTime;

}jobDataType;

char * progname;

void
usage(const char * message)
{
    fprintf(stderr, "%s", message);
    fprintf(stderr, "Usage: %s <arguments>\nwhere the arguments are:\n",
	    progname);
    fprintf(stderr, "\t-alg [rr|sjf|fcfs]\n"
	    "\t[-init_jobs <n1 (int)>\n"
	    "\t[-total_jobs <n2 (int)>]\n"
	    "\t[-prob_comp_time <lambda (double)>]\n"
	    "\t[-sched_time <ts (int, microseconds)>]\n"
	    "\t[-cs_time <cs (int, microseconds)>]\n"
	    "\t[-tick_time <cs (int, milliseconds)>]\n"
	    "\t[-prob_new_job <pnj (double)>]\n"
	    "\t[-randomize]\n");
}


/*
 * Name:	process_args()
 * Purpose:	Process the arguments, checking for validity where possible.
 * Arguments:	argc, argv and a struct to hold the values in.
 * Outputs:	Error messages only.
 * Modifies:	The struct argument.
 * Returns:	0 on success, non-0 otherwise.
 * Assumptions:	The pointers are valid.
 * Bugs:	This is way too long.  Maybe I should reconsider how
 *		much I dislike getopt().
 * Notes:	Sets the struct to the default values before
 *		processing the args.
 */

int
process_args(int argc, char * argv[], struct simulation_params * sps)
{
    // Process the command-line arguments.
    // The only one which doesn't have a default (and thus must be
    // specified) is the choice of scheduling algorithm.

    char c;
    int i;
    
    for (i = 1; i < argc; i++)
    {
		if (!strcmp(argv[i], "-alg"))
		{
			i++;
			if (!strcmp(argv[i], "rr"))
			sps->sched_alg = RR;
			else if (!strcmp(argv[i], "sjf"))
			sps->sched_alg = SJF;
			else if (!strcmp(argv[i], "fcfs"))
			sps->sched_alg = FCFS;
			else
			{
				usage("Error: invalid scheduling algorithm (-alg).\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-init_jobs"))
		{
			i++;
			if (sscanf(argv[i], "%d%c", &sps->init_jobs, &c) != 1
			|| sps->init_jobs < 0)
			{
				usage("Error: invalid argument to -init_jobs\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-total_jobs"))
		{
			i++;
			if (sscanf(argv[i], "%d%c", &sps->total_jobs, &c) != 1
			|| sps->total_jobs < 0)
			{
				usage("Error: invalid argument to -total_jobs\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-prob_comp_time"))
		{
			i++;
			if (sscanf(argv[i], "%le%c", &sps->lambda, &c) != 1
			|| sps->lambda < 0)
			{
				usage("Error: invalid argument to -prob_comp_time\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-sched_time"))
		{
			i++;
			if (sscanf(argv[i], "%d%c", &sps->sched_time, &c) != 1
			|| sps->sched_time < 0)
			{
				usage("Error: invalid argument to -sched_time\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-cs_time"))
		{
			i++;
			if (sscanf(argv[i], "%d%c", &sps->cont_swtch_time, &c) != 1
			|| sps->cont_swtch_time < 0)
			{
				usage("Error: invalid argument to -cs_time\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-tick_time"))
		{
			i++;
			if (sscanf(argv[i], "%d%c", &sps->tick_time, &c) != 1
			|| sps->tick_time < 0)
			{
				usage("Error: invalid argument to -tick_time\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-prob_new_job"))
		{
			i++;
			if (sscanf(argv[i], "%le%c", &sps->prob_new_job, &c) != 1
			|| sps->prob_new_job < 0)
			{
				usage("Error: invalid argument to -prob_new_job\n");
				return 1;
			}
		}
		else if (!strcmp(argv[i], "-randomize"))
		{
			sps->randomize = true;
			srandom(time(NULL));
		}
	}
    return 0;
}
/*
 * Name:	main(int argc, char * argv[])
 * Purpose:	This program **attempts** to implement a FCFS and SJF scheduling 
 * algorithm. A set of parameters are passed through main and initialized to
 * be the parameters of the job set. 
 * Arguments:	argc, argv and a struct to hold the values in.
 * Outputs:	All the parameters as well as the averages for
 * turn around time, wait time, and response time. 
 * Modifies:	The struct argument.
 * Returns:	0 on success, non-0 otherwise.
 * Assumptions:	The pointers are valid.
 * Bugs:	Sometimes when running with the -randomize argument, the program
 			ceases...
 * Notes:	Sets the struct to the default values before
 *		processing the args.
 */

int
main(int argc, char * argv[])
{
    progname = argv[0];
    struct simulation_params sim_params = {
	.sched_alg = UNDEFINED,
	.init_jobs = DEFAULT_INIT_JOBS,
	.total_jobs = DEFAULT_TOTAL_JOBS,
	.lambda = DEFAULT_LAMBDA,
	.sched_time = DEFAULT_SCHED_TIME,
	.cont_swtch_time = DEFAULT_CONT_SWTCH_TIME,
	.tick_time = DEFAULT_TICK_TIME,
	.prob_new_job = DEFAULT_PROB_NEW_JOB,
	.randomize = DEFAULT_RANDOMIZE
    };

    double average_response_time = 0;
    double average_waiting_time = 0;
    double average_turnaround_time = 0;

    if (process_args(argc, argv, &sim_params) != 0)
	return EXIT_FAILURE;
/*
* Run the following program if the scheduling algorithm is either SJF or FCFS
*/
	
	if((sim_params.sched_alg == FCFS) || sim_params.sched_alg == SJF)
	{		
		sim_params.tick_time *= 1000;
		
		int64_t sorter;
/*
* An array of jobs that store the details of each job
*/
		jobDataType jobArray[sim_params.total_jobs];

/*
* Creating the initial jobs
*/
		
		for(int i = 0; i < sim_params.init_jobs; i++)
		{		
			jobArray[i].creationTime = 0;
			jobArray[i].burstTime = rand_exp(sim_params.lambda)*1000000;
		}
/*
* This forloop orders the initial jobs from least to greatest if the scheduling
* algorithm is SJF
*/
		if(sim_params.sched_alg == SJF)
		{
			for(int i = 0; i < sim_params.init_jobs; i++)
			{
				for(int j = i + 1; j < sim_params.init_jobs; j ++)
				{
					if(jobArray[i].burstTime > jobArray[j].burstTime)
					{
						sorter = jobArray[i].burstTime;
						jobArray[i].burstTime = jobArray[j].burstTime;
						jobArray[j].burstTime = sorter;
					}
				}
			}
		}
		int64_t currentTime = 0;
		int64_t processTime = 0;
		int64_t endTime = 0;
			
		int i = 0;
		int j = sim_params.init_jobs;
/*
* While loop runs through each job and increments two timers
*/		
		while(i < sim_params.total_jobs)
		{	
			currentTime++;
			processTime++;
/*
* If the current time has completed a clock tick, a job is created with the 
* probability of prob_new_job. Default is 15%
*/	
			if((currentTime % sim_params.tick_time == 0) 
				&& j < sim_params.total_jobs)
			{
				if(random() % 100 == sim_params.prob_new_job*100)
				{
					jobArray[j].creationTime = currentTime;
					jobArray[j].burstTime = rand_exp(sim_params.lambda)*1000000;
					j++;
					if(sim_params.sched_alg == SJF)
					{
						for(int a = 0; a < sim_params.init_jobs; a++)
						{
							for(int b = a + 1; b < sim_params.init_jobs; b ++)
							{
								if(jobArray[a].burstTime > jobArray[b].burstTime)
								{
									sorter = jobArray[a].burstTime;
									jobArray[a].burstTime = jobArray[b].burstTime;
									jobArray[a].burstTime = sorter;
								}
							}
						}
					}
				}
			}

/*
* Only analyze timings once a job has been completed. endTime represents the 
* point in time that the previous job finished. The difference between endTime
* and the creationTime of the next job is how long that job had to wait (plus
* the time the previous job spent waiting). 
*/
				
			if(processTime == jobArray[i].creationTime + jobArray[i].burstTime)
			{			
				if(i == 0)
				{
					jobArray[i].waitTime = sim_params.cont_swtch_time
										 + sim_params.sched_time;
				}
				else
				{	
					jobArray[i].waitTime =    endTime 
											- jobArray[i].creationTime 
											+ sim_params.cont_swtch_time
											+ sim_params.sched_time
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
/*
* summates both the waiting time and the turn around time and finds their
* averages
*/
		for(int i = 0; i < sim_params.total_jobs; i++)
		{
			sum1 += jobArray[i].turnaroundTime;
			sum2 += jobArray[i].waitTime;
		}
		
		average_turnaround_time = sum1/sim_params.total_jobs;
		average_waiting_time = sum2/sim_params.total_jobs;
		average_response_time = average_waiting_time;
	}
	
    D_PRNT("This is a debug stmt\n");
    D_PRNT("average_turnaround_time = %.6f\n", average_turnaround_time);

    printf("For a simulation using the %s scheduling algorithm\n",
	   alg_names[sim_params.sched_alg]);
    printf("with the following parameters:\n");
    printf("    init jobs           = %d\n", sim_params.init_jobs);
    printf("    total jobs          = %d\n", sim_params.total_jobs);
    printf("    lambda              = %.6f\n", sim_params.lambda);
    printf("    sched time          = %d\n", sim_params.sched_time);
    printf("    context switch time = %d\n", sim_params.cont_swtch_time);
    printf("    tick time           = %d\n", sim_params.tick_time);
    printf("    prob of new job     = %.6f\n", sim_params.prob_new_job);
    printf("    randomize           = %s\n",
	   sim_params.randomize ? "true" : "false");
    printf("the following results were obtained:\n");
    printf("    Average response time:   %10.6lf\n", average_response_time);
    printf("    Average turnaround time: %10.6lf\n", average_turnaround_time);
    printf("    Average waiting time:    %10.6lf\n", average_waiting_time);

    return EXIT_SUCCESS;
}

double rand_exp(double lambda)
{
    int64_t divisor = (int64_t)RAND_MAX + 1;
    double u_0_to_almost_1;
    double raw_value;

    u_0_to_almost_1 = (double)random() / divisor;

    raw_value = log(1 - u_0_to_almost_1) / -lambda;

    return round(raw_value * 1000000.) / 1000000.;
}

