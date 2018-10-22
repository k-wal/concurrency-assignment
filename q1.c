#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 


//0 for player, 1 for refree

struct args {
    int thread_index;
    int current_players;
};

pthread_t tid[500];
int games=0,player1_index,player2_index,refree_index,team=0,ready_team=0,waiting_players=0;
pthread_mutex_t refree_lock,player1_lock,player2_lock,waiting_players_lock,ready_team_lock,team_lock,games_lock;

//function called byb refree/player right after being created
void enterAcademy(int thread_index,int type)
{
	if(type==0)
	{
		printf("Player with index %d entering academy\n",thread_index);
	}
	else
	{
		printf("Refree with index %d entering academy\n",thread_index);
	}
	return;
}

//function called by player/refree after forming in groups of 3
void enterCourt(int thread_index,int type)
{	
	if(type==0)
	{
		printf("Player with index %d entering court\n",thread_index);
	}
	else
	{
		printf("Refree with index %d entering court\n",thread_index);
	}
	
	return;
}

//function called by player
void warmUp(int thread_index)
{
	printf("Player with index %d warming up\n",thread_index);
	sleep(1);
	pthread_mutex_lock(&ready_team_lock);
	ready_team++;
	pthread_mutex_unlock(&ready_team_lock);
	printf("Player with index %d done warming up\n",thread_index);
	
	return;
}

//function called by refree
void adjustEquipment(int thread_index)
{
	printf("Refree with index %d adjusting equipment\n",thread_index);
	sleep(0.5);
	pthread_mutex_lock(&ready_team_lock);
	ready_team++;
	pthread_mutex_unlock(&ready_team_lock);
	printf("Refree with index %d done adjusting equipment\n",thread_index);
	
	return;
}

//function called by refree/player to meet organizer
void meetOrganizer(int thread_index,int type,int current_players)
{
	pthread_mutex_lock(&waiting_players_lock);
	waiting_players++;
	pthread_mutex_unlock(&waiting_players_lock);
	//if its a player
	if(type==0)
	{
		if(current_players%2==1)
		{
			pthread_mutex_lock(&player1_lock);
			printf("Person %d is now player 1\n",thread_index);
			pthread_mutex_lock(&team_lock);
			team++;
			pthread_mutex_unlock(&team_lock);
			pthread_mutex_lock(&waiting_players_lock);
			waiting_players--;
			pthread_mutex_unlock(&waiting_players_lock);
			player1_index=thread_index;
		}
		else
		{
			pthread_mutex_lock(&player2_lock);
			printf("Person %d is now player 2\n",thread_index);
			pthread_mutex_lock(&team_lock);
			team++;
			pthread_mutex_unlock(&team_lock);
			pthread_mutex_lock(&waiting_players_lock);
			waiting_players--;
			pthread_mutex_unlock(&waiting_players_lock);
			player2_index=thread_index;
		}
	}
	if(type==1)
	{
		pthread_mutex_lock(&refree_lock);
		printf("Person %d is now refree\n",thread_index);
		pthread_mutex_lock(&team_lock);
		team++;
		pthread_mutex_unlock(&team_lock);
		refree_index=thread_index;
	}
	while(1)
	{
		if(team==3)
		{
			printf("team formed with %d\n",thread_index);
			return;
		}
	}
}

//startgame, called by refree to start game and free organizer
void startGame(void)
{
	while(1)
	{
		if(ready_team==3)
		{
			printf("Game started by players %d, %d and refree %d\n",player1_index,player2_index,refree_index);
			pthread_mutex_lock(&team_lock);
			team=0;
			pthread_mutex_unlock(&team_lock);
			pthread_mutex_lock(&ready_team_lock);
			ready_team=0;
			pthread_mutex_unlock(&ready_team_lock);
			pthread_mutex_unlock(&player1_lock);
			pthread_mutex_unlock(&player2_lock);
			pthread_mutex_unlock(&refree_lock);
			printf("Organizer free\n");
			pthread_mutex_lock(&games_lock);
			games++;
			pthread_mutex_unlock(&games_lock);
			
			return;
					
		}
	}
}

//function called when player is created
void* player_function(void *arg)
{
	int current_players=((struct args*)arg)->current_players;
	int thread_index=((struct args*)arg)->thread_index;
	enterAcademy(thread_index,0);
	meetOrganizer(thread_index,0,current_players);
	enterCourt(thread_index,0);
	warmUp(thread_index);
}

//function called when refree is created
void* refree_function(void *arg)
{
	int current_players=((struct args*)arg)->current_players;
	int thread_index=((struct args*)arg)->thread_index;
	enterAcademy(thread_index,1);
	meetOrganizer(thread_index,1,current_players);
	enterCourt(thread_index,1);
	adjustEquipment(thread_index);
	startGame();
}

int main()
{
	if(pthread_mutex_init(&player1_lock, NULL) != 0) 
    { 
        printf("\nplayer1 lock init failed\n"); 
        return 1; 
    }
    if(pthread_mutex_init(&player2_lock, NULL) != 0) 
    { 
        printf("\nplayer2 lock init failed\n"); 
        return 1; 
    } 
	if(pthread_mutex_init(&refree_lock, NULL) != 0) 
    { 
        printf("\nplayer1 lock init failed\n"); 
        return 1; 
    } 
	if(pthread_mutex_init(&waiting_players_lock, NULL) != 0) 
    { 
        printf("\nwaiting_players lock init failed\n"); 
        return 1; 
    }
    if(pthread_mutex_init(&ready_team_lock, NULL) != 0) 
    { 
        printf("\nready_team lock init failed\n"); 
        return 1; 
    }
    if(pthread_mutex_init(&team_lock, NULL) != 0) 
    { 
        printf("\nteam lock init failed\n"); 
        return 1; 
    }
    if(pthread_mutex_init(&games_lock, NULL) != 0) 
    { 
        printf("\ngames lock init failed\n"); 
        return 1; 
    }  
	  
	 
	 
	int i,n,error;
	int current_players=0,current_refrees=0;
	scanf("%d",&n);
	for(i=0; i<3*n; i++)
	{
		struct args *index = (struct args *)malloc(sizeof(struct args));
   		index->thread_index=i;
		sleep(rand()%3);
		int this;
		int r = n - current_refrees;
		int p = 2*n - current_players;
		int p_prob = 100*p/(p+r);
		int r_prob = 100-p_prob;
		int random1 = rand()%100 + 1;
		if(p_prob>random1)
		{
			//creating players
			current_players++;
			index->current_players=current_players;
			this=0;
			error=pthread_create(&(tid[i]),NULL,&player_function,(void*)index);
			if (error != 0) 
            	printf("\nThread can't be created :[%s]", strerror(error)); 
			else
				printf("Person with index %d entering as player\n",i);
		}
		else
		{
			//creating refrees
			current_refrees++;
			index->current_players=current_players;
			this=1;
			error=pthread_create(&(tid[i]),NULL,&refree_function,(void*)index);
			if (error != 0) 
            	printf("\nThread can't be created :[%s]", strerror(error));
            else
            	printf("Person with index %d entering as refree\n",i); 
		}


	}
	//joining all threads
	for(i=0; i<3*n; i++)
	{
		pthread_join(tid[i],NULL);
	}
	return 0;
}