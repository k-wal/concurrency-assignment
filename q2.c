#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include "q2_init.c"


//thread arrays for all voters and evms
pthread_t voter_thread[100];
pthread_t evm_thread[100];


//voter function, to be called when thread created
void* voter_function(void* Voter)
{
	Booth* booth=((struct voter1*)Voter)->booth;
	int booth_index=((struct voter1*)Voter)->booth->booth_index;
	int voter_index=((struct voter1*)Voter)->voter_index;
	voter_wait_for_evm(voter_index,booth);
	voter_in_slot(voter_index,booth);
	//add to the total number of voter threads are equal
	pthread_mutex_lock(&total_voters_lock);
	total_voters++;
	pthread_mutex_unlock(&total_voters_lock);	
}

//evm function, to be called when evm created
void* evm_function(void* Evm)
{
	int booth_index=((struct evm1*)Evm)->booth->booth_index;
	int evm_index=((struct evm1*)Evm)->evm_index;
	Booth* booth=((struct evm1*)Evm)->booth;
	
	//keep calling function till no voters are left to be assigned	
	while(booth->left_voters>0)
	{
		int count=rand()%10 + 1;
		//call only if a voter has been assigned
		if(polling_ready_evm(evm_index,count,booth)!=count)
		{
			evm_voting(evm_index,booth);
		}
	}
	//add to the total number of evm threads that are over
	pthread_mutex_lock(&total_evms_lock);
	total_evms++;
	pthread_mutex_unlock(&total_evms_lock);
}



int main()
{
	pthread_mutex_init(&total_evms_lock, NULL);
	pthread_mutex_init(&total_voters_lock, NULL);
	int evm_index=0,voter_index=0;
	int t_booth,i,n,j;
	printf("Enter total number of booths : ");
	scanf("%d",&t_booth);
	Booth* booth_list[100];
	n=t_booth;

	//loop for iniitialization
	for(i=1; i<=n; i++)
	{
		Booth* booth;
		booth_list[i]=booth_init(booth,i);
	    
	}

	//loop to create evm and voter threads
	for(i=1; i<=n; i++)
	{
		Booth* booth=booth_list[i];
		
		for(j=1; j<=booth->total_evms;j++)
	    {
    	    Evm *current_evm = (Evm *)malloc(sizeof(Evm));
        	current_evm->booth=booth;
	        current_evm->evm_index=j;
    	    int error=pthread_create(&(evm_thread[evm_index]),NULL,&evm_function,(void*)current_evm);
        	if (error != 0) 
            	printf("\nThread can't be created :[%s]", strerror(error)); 
    	    evm_index++;
    	}
    	for(j=1; j<=booth->total_voters;j++)
    	{
     	    Voter *current_voter = (Voter*)malloc(sizeof(Voter));
        	current_voter->booth=booth;
        	current_voter->voter_index=j;
        	int error=pthread_create(&(voter_thread[voter_index]),NULL,&voter_function,(void*)current_voter);
        	if (error != 0) 
        	   	printf("\nThread can't be created :[%s]", strerror(error)); 
        	voter_index++;
    	}
	}
	//keep checking if all threads are done
	while(1)
	{
		//if all threads are done, exit
		if(total_voters==voter_index && total_evms==evm_index)
		{
			return 0;
		}
	}
}


