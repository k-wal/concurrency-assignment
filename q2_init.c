#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 

int total_voters=0;
int total_evms=0;
pthread_mutex_t total_voters_lock,total_evms_lock;


typedef struct booth1
{
	int active_evm_index;
    int booth_index;
    int total_evms;
    int total_voters;
    int left_voters;
    
    pthread_mutex_t booth_lock;
    pthread_cond_t booth_cond,ready_evm;
} Booth;

typedef struct evm1
{
	int evm_index;
	Booth* booth;
}Evm;

typedef struct voter1
{	
	int voter_index;
	Booth* booth;
}Voter;


//returns number of empty slots in evm, only returns 
int polling_ready_evm(int evm_index,int count,Booth* booth)
{
    sleep(1);
    printf("EVM %d of booth %d has %d slots empty\n",evm_index,booth->booth_index,count);
    while(count>0 && booth->left_voters>0)
    {
        pthread_mutex_lock(&booth->booth_lock);
        if(booth->left_voters<=0)
        {
            pthread_mutex_unlock(&booth->booth_lock);
            break;
        }
        //if a previous evm has sent signal but not yet used by a voter
        if(booth->active_evm_index!=-1)
        {
            pthread_cond_wait(&booth->ready_evm,&booth->booth_lock);
        }
        booth->active_evm_index=evm_index;
        booth->left_voters--;
        pthread_cond_signal(&booth->booth_cond);
        pthread_mutex_unlock(&booth->booth_lock);
        count--;
            
    }
    sleep(1);
    return count;
}

void evm_voting(int evm_index,Booth* booth)
{
    printf("EVM %d of Booth %d starting voting\n",evm_index,booth->booth_index);
    sleep(3);
    printf("EVM %d of Booth %d finished voting\n",evm_index,booth->booth_index);
}

void voter_wait_for_evm(int voter_index,Booth* booth)
{
    printf("Voter %d of booth %d is waiting for EVM\n",voter_index,booth->booth_index);
    pthread_mutex_lock(&booth->booth_lock);
    pthread_cond_wait(&booth->booth_cond,&booth->booth_lock);
    printf("Voter %d at booth %d moves to EVM %d\n",voter_index,booth->booth_index,booth->active_evm_index);
    booth->active_evm_index=-1;
    pthread_cond_signal(&booth->ready_evm);
    pthread_mutex_unlock(&booth->booth_lock);

}

void voter_in_slot(int voter_index,Booth* booth)
{
//  printf("Voter %d of Booth %d is now in slot\n",voter_index,booth->booth_index);
//  sleep(1);
    return;
}

void booth_init(Booth* booth,int i)
{
    int j;
    booth->booth_index=i;
    booth->active_evm_index=-1;
    booth->left_voters=booth->total_voters;
    if(pthread_mutex_init(&booth->booth_lock, NULL) != 0) 
    { 
        printf("booth lock init failed\n"); 
        //return 1; 
    }
    if(pthread_cond_init(&booth->booth_cond,NULL)!=0)
    {
        printf("booth condition init failed\n");
        //return 1;
    }
}