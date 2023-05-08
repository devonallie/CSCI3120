/*
* File: A5.c
* Author: Devon Allie
* Date: July 19th
* Version 1.0
*
* Purpose: This programme accepts a series of references and a set frame size to
* 		   test various page replacement algorithms. 
* 
* Note: This code looks like garbage and I apologize. If only there was more
*       time...
*/



#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#define MAX_FRAMES 2500
#define MAX_REFERENCES 100000
#define MAX_REFERENCE_VALUE 5000

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define START_FREQUENCY 1
#define START_AGE 1

typedef struct frame
{
	int age;
	int reference;
	int frequency;
	int timeOfLastReference;
	bool notEmpty; //for special case of entering a zero...
	bool secondChance;
	int optimalTime;
		
}frame;

int main(int argc, char** argv)
{
	if(argc < 2 || (argc > 2 && (atoi(argv[1]) > MAX_FRAMES)))
	{
		printf("Incorrect argument for number of frames available\n");
		
		return EXIT_FAILURE;
	}	
	
	int num_frames = atoi(argv[1]);
	int optimal_faults = 0;
	int fifo_faults = 0;
	int lru_faults = 0;
	int lfu_faults = 0;
	int mfu_faults = 0;
	int second_chance_faults = 0;
	
	int inputIndex = 0;
	int clock = 0;
	int reference[MAX_REFERENCES];
	bool optimal_fault, FIFO_fault, LRU_fault, LFU_fault, MFU_fault, secondChance_fault;
	frame	optimal_page[num_frames],	FIFO_page[num_frames],
			LRU_page[num_frames],		LFU_page[num_frames],
			MFU_page[num_frames],		secondChance_page[num_frames];
	
	while(scanf("%d", &reference[inputIndex]) != EOF) inputIndex++;

	int size = inputIndex;
	
	
	int optimalTime[MAX_REFERENCES];
	int optimalReference[MAX_REFERENCE_VALUE];
	
	inputIndex = 0;
	
/*
* This while loop saves the inputIndex for a given reference at the index
* of the reference itself inside of optimalReference[].
* e.g if the reference is 7 is found at inputIndex 2, the value 2 would be saved
* at the 7th memory location of optimalReference[]. If a value is already
* recorded at, for example, the 7th location, that means 7 has been read twice.
* The current inputIndex value is subtracted from the one stored in
* optimalReference[] and the difference is stored in optimalTime[] at the index
* where the initial 7 appeared. This should setup optimalTime so that at every
* location of reference[] there will be a time to the next instance of the same
* reference stored in optimalReference[].
*/		
	while(inputIndex < size)
	{
		if(optimalReference[reference[inputIndex]] > 0)
		{
			optimalTime[optimalReference[reference[inputIndex]]] = inputIndex - 
			optimalReference[reference[inputIndex]];
		}
		else
		{
			optimalReference[reference[inputIndex]] = inputIndex;
		}
		inputIndex++;
	}
	
	inputIndex = 0;

/*
* The rest of the programme runs in 3 stages. 
*
* The first stage runs through the
* array that holds the reference string to determine if a the reference in the
* reference string is already in any of the frames. Each algorithm has its own
* frame. 
* 
* For the LRU frame, if a reference is already in memory a clock is
* recorded to the reference. 
*
* For the LFU and MFU frame, if the reference is already in memory its frequency
* component is incremented. 
*
* For secondChance, if the reference is already in memory a boolean relating to
* its special bit is set to true. This will give it a second chance.
*
* At each reference, wether in memory or not, is given an optimalTime for the
* optimalTime page and the age of references in both FIFO and secondChance are
* incremented.
*/
	
	while(inputIndex < size)
	{	
		clock++;
		
		optimal_fault = true;
		FIFO_fault = true;
		LRU_fault = true;
		LFU_fault = true;
		MFU_fault = true;
		secondChance_fault = true;
		
		for(int frameNumber = 0; frameNumber < num_frames; frameNumber++)
		{
			if(optimal_page[frameNumber].reference	== reference[inputIndex]
			&& optimal_page[frameNumber].notEmpty)
			{
				optimal_fault = false;
			}
			if(FIFO_page[frameNumber].reference	== reference[inputIndex]
			&& FIFO_page[frameNumber].notEmpty)
				FIFO_fault = false;
				
			if(LRU_page[frameNumber].reference	== reference[inputIndex]
			&& LRU_page[frameNumber].notEmpty)
			{
				LRU_page[frameNumber].timeOfLastReference = clock;
				LRU_fault = false;
			}
			
			if(LFU_page[frameNumber].reference	== reference[inputIndex]
			&& LFU_page[frameNumber].notEmpty)
			{
				LFU_page[frameNumber].frequency++;
				LFU_fault = false;
			}
				
			if(MFU_page[frameNumber].reference	== reference[inputIndex]
			&& MFU_page[frameNumber].notEmpty)
			{
				MFU_page[frameNumber].frequency++;
				MFU_fault = false;
			}
			if(secondChance_page[frameNumber].reference == reference[inputIndex]
			&& secondChance_page[frameNumber].notEmpty)
			{
				secondChance_fault = false;
				secondChance_page[frameNumber].secondChance = true;
			}	
			
			optimal_page[frameNumber].optimalTime = optimalTime[inputIndex];
			secondChance_page[frameNumber].age++;
			FIFO_page[frameNumber].age++;
		}
		
		
		int optimal_index=0, FIFO_index=0, LRU_index=0, LFU_index=0, 
		MFU_index=0, secondChance_index=0;
/*
* The second stage tries to find the index from each page that chooses what
* frame to replace. 
*
* Optimal time finds the reference with the longest time between references.
*
* FIFO finds the index where the oldest reference is.
* 
* LRU finds the index where the reference that hasnt been called for the longest
* time is.
*
* LFU finds the index where the reference was called the least. If there is a
* tie, it goes to the smallest reference value.
*
* MFU finds the index where the reference was called the most. If there is a
* tie, it goes to the smallest reference value.
*
* secondChance goes to the oldest unless it has been referenced again and has a
* special bit assigned, then the next oldest is replaced and the special bit is
* set to false. 
*/
		
		for(int frameNumber = 0; frameNumber < num_frames; frameNumber++)
		{
			if(optimal_page[frameNumber].optimalTime > 
			optimal_page[optimal_index].optimalTime)
				optimal_index = frameNumber;
			if(FIFO_page[frameNumber].age > FIFO_page[FIFO_index].age)
				FIFO_index = frameNumber;
				
			if(LRU_page[frameNumber].timeOfLastReference < 
			LRU_page[LRU_index].timeOfLastReference)
				LRU_index = frameNumber;
				
			if(LFU_page[frameNumber].frequency < LFU_page[LFU_index].frequency)
				LFU_index = frameNumber;
				
			if((LFU_page[frameNumber].frequency == LFU_page[LFU_index].frequency)
			&& (LFU_page[frameNumber].reference < LFU_page[LFU_index].reference))
				LFU_index = frameNumber;
				
			if(MFU_page[frameNumber].frequency > MFU_page[MFU_index].frequency)
				MFU_index = frameNumber;
				
			if((MFU_page[frameNumber].frequency == MFU_page[MFU_index].frequency)
			&& (MFU_page[frameNumber].reference < MFU_page[MFU_index].reference))
				MFU_index = frameNumber;
				
			if(secondChance_page[frameNumber].age > 
			secondChance_page[secondChance_index].age
			&& secondChance_page[frameNumber].secondChance == false)
				secondChance_index = frameNumber;
		}
		
/*
* In the third stage, the frame dictated by the index found in the previous step
* chooses what reference to replace. This is done for each page. If the frame
* isnt full, the index is ignored and the reference is placed at the next
* available place. 
*/		
		
		if(optimal_fault)
		{
			if(optimal_faults < num_frames)
			{
				optimal_page[optimal_faults].reference = reference[inputIndex];
				optimal_page[optimal_faults].notEmpty = true;
			}
			else
			{
				optimal_page[optimal_index].reference = reference[inputIndex];
				optimal_page[optimal_index].notEmpty = true;
			}
			optimal_faults++;
		}
		if(FIFO_fault)
		{
			if(fifo_faults < num_frames)
			{
				FIFO_page[fifo_faults].reference = reference[inputIndex];
				FIFO_page[fifo_faults].age = START_AGE;
				FIFO_page[fifo_faults].notEmpty = true;
			}
			else
			{
				FIFO_page[FIFO_index].reference = reference[inputIndex];
				FIFO_page[FIFO_index].age = START_AGE;
				FIFO_page[FIFO_index].notEmpty = true;
			}
			fifo_faults++;
		}
		if(LRU_fault)
		{
			if(lru_faults < num_frames)
			{
				LRU_page[lru_faults].reference = reference[inputIndex];
				LRU_page[lru_faults].timeOfLastReference = clock;
				LRU_page[lru_faults].notEmpty = true;
			}
			else
			{
				LRU_page[LRU_index].reference = reference[inputIndex];
				LRU_page[LRU_index].timeOfLastReference = clock;
				LRU_page[LRU_index].notEmpty = true;
			}
			lru_faults++;
		}
		if(LFU_fault)
		{
			if(lfu_faults < num_frames)
			{
				LFU_page[lfu_faults].reference = reference[inputIndex];
				LFU_page[lfu_faults].frequency = START_FREQUENCY;
				LFU_page[lfu_faults].notEmpty = true;
			}
			else
			{
				LFU_page[LFU_index].reference = reference[inputIndex];
				LFU_page[LFU_index].frequency = START_FREQUENCY;
				LFU_page[LFU_index].notEmpty = true;
			}
			lfu_faults++;
		}
		if(MFU_fault)
		{
			if(mfu_faults < num_frames)
			{
				MFU_page[mfu_faults].reference = reference[inputIndex];
				MFU_page[mfu_faults].frequency = START_FREQUENCY;
				MFU_page[mfu_faults].notEmpty = true;
			}
			else
			{
				MFU_page[MFU_index].reference = reference[inputIndex];
				MFU_page[MFU_index].frequency = START_FREQUENCY;
				MFU_page[MFU_index].notEmpty = true;
			}
			mfu_faults++;
		}
		if(secondChance_fault)
		{
			if(second_chance_faults < num_frames)
			{
				secondChance_page[second_chance_faults].reference = 
														reference[inputIndex];
				secondChance_page[second_chance_faults].age = START_AGE;
				secondChance_page[second_chance_faults].notEmpty = true;
				secondChance_page[second_chance_faults].secondChance = false;
			}
			else
			{
				secondChance_page[secondChance_index].reference = 
														reference[inputIndex];
				secondChance_page[secondChance_index].age = START_AGE;
				secondChance_page[secondChance_index].notEmpty = true;
				secondChance_page[secondChance_index].secondChance = false;
			}
			second_chance_faults++;
		}

		inputIndex++;
	}
	
	printf("For a system with %d frames:\n", num_frames);
	printf("Optimal page faults: %d\n", optimal_faults);
	printf("FIFO page faults: %d\n", fifo_faults);
	printf("LRU page faults: %d\n", lru_faults);
	printf("LFU page faults: %d\n", lfu_faults);
	printf("MFU page faults: %d\n", mfu_faults);
	printf("Second chance page faults: %d\n", second_chance_faults);
	
	return EXIT_SUCCESS;
}






