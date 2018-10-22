#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 

int total_voters=0;
int total_evms=0;
pthread_mutex_t total_voters_lock,total_evms_lock;

//booth structure definition
typedef struct booth1
{
    int active_evm_index;
	//int active_evms[100];
    //int voter_active;
    //int evm_active;
    int booth_index;
    int total_evms;
    int total_voters;
    int left_voters;
    //int waiting_evms;   
    pthread_mutex_t booth_lock;
    pthread_cond_t booth_cond,ready_evm;
} Booth;

//evm structure definition
typedef struct evm1
{
	int evm_index;
	Booth* booth;
}Evm;

//voter structure definition
typedef struct voter1
{	
	int voter_index;
	Booth* booth;
}Voter;

//if all voters are done, send signal to evms
void check_waiting_evms(Booth* booth)
{   pthread_mutex_lock(&booth->booth_lock);
    if(booth->left_voters==0)
    {    
        pthread_cond_broadcast(&booth->ready_evm);
    }
    pthread_mutex_unlock(&booth->booth_lock);

}


//returns number of empty slots in evm, only returns if no slots left or no voters left 
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
        
        //if a previous evm has sent signal but not yet used by a voter,put in queue
        while(booth->active_evm_index!=-1)
        {
            if(booth->left_voters<=0)
            {
                pthread_mutex_unlock(&booth->booth_lock);
                return count;
            }
            pthread_cond_wait(&booth->ready_evm,&booth->booth_lock);
        }
        booth->active_evm_index=evm_index;
        //booth->active_evms[booth->evm_active]=evm_index;
        //booth->evm_active++;
        booth->left_voters--;
        pthread_cond_signal(&booth->booth_cond);
        pthread_mutex_unlock(&booth->booth_lock);
        count--;
        
    }        
    
    sleep(1);
    return count;
    //return count-v;
}

//to signify evm going to voting stage
void evm_voting(int evm_index,Booth* booth)
{
    printf("EVM %d of Booth %d starting voting\n",evm_index,booth->booth_index);
    sleep(3);
    printf("EVM %d of Booth %d finished voting\n",evm_index,booth->booth_index);
}

//voter waits for evm to be assigned to it
void voter_wait_for_evm(int voter_index,Booth* booth)
{
    printf("Voter %d of booth %d is waiting for EVM\n",voter_index,booth->booth_index);
    pthread_mutex_lock(&booth->booth_lock);
    pthread_cond_wait(&booth->booth_cond,&booth->booth_lock);        
    
    //active_evm is the evm that is currently active
    printf("Voter %d at booth %d moves to EVM %d\n",voter_index,booth->booth_index,booth->active_evm_index);
    
    //booth->voter_active++;
    booth->active_evm_index=-1;
    pthread_cond_signal(&booth->ready_evm);
    pthread_mutex_unlock(&booth->booth_lock);
    check_waiting_evms(booth);

}


void voter_in_slot(int voter_index,Booth* booth)
{
//  printf("Voter %d of Booth %d is now in slot\n",voter_index,booth->booth_index);
//  sleep(1);
    return;
}

//booth initialization function to take input and initialize all booth variables except threads
Booth* booth_init(Booth* booth,int i)
{    
    booth=(Booth*)malloc(sizeof(Booth));
    int x;
    printf("Enter total number of evms for booth %d : ",i);
    scanf("%d",&x);
    booth->total_evms=x;
    printf("Enter total number of voters for booth %d : ",i);
    scanf("%d",&x);
    //booth->evm_active=0;
    //booth->voter_active=0;
    booth->total_voters=x;
    booth->booth_index=i;
    booth->active_evm_index=-1;
    booth->left_voters=booth->total_voters;
    if(pthread_mutex_init(&booth->booth_lock, NULL) != 0) 
    { 
        printf("booth lock init failed\n"); 
    }
    if(pthread_cond_init(&booth->booth_cond,NULL)!=0)
    {
        printf("booth condition init failed\n");
    }
    return booth;
}