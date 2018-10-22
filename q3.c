#include <sys/types.h> 
#include <sys/wait.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <time.h> 
#include <pthread.h>
#include <string.h>

long long int c[10000];

void selectionsort(long long int a[],int l,int r)
{
	int i,j,min;
	for(i=l; i<r; i++)
	{
		min=i;
		for(j=i+1; j<=r; j++)
		{
			if(a[j]<a[min])
			{
				min=j;
			}
		}
		long long int temp;
		temp=a[i];
		a[i]=a[min];
		a[min]=temp;
	}
}

//concurrent merge sort with processes
void mergesort(long long int a[], int l, int r)
{
	//printf("mergesort : %d %d\n",l,r);
	//do selection sort if less than 5 elements
	if(r-l+1<=5)
	{
		selectionsort(a,l,r);
		return;
	}
	if(l!=r)
	{
		long long int mid;
		mid=(l+r)/2;
		pid_t lpid,rpid;
		lpid=fork();
		if(lpid<0)
		{
			perror("Error creating left child\n");
			_exit(-1);
		}
		//if inside the left child
		if(lpid==0)
		{
			mergesort(a,l,mid);
			_exit(0);
		}
		//if inside the parent
		else
		{
			rpid=fork();
			if(rpid<0)
			{
				perror("Error creating right child\n");
				_exit(-1);
			}
			//if inside right child
			if(rpid==0)
			{
				mergesort(a,mid+1,r);
				_exit(0);		
			}
			//if inside parent
			else
			{
				int status;
				waitpid(lpid, &status, 0); 
				waitpid(rpid, &status, 0); 
				long long int copy[10000],i,j,k;
				i=l;
				j=mid+1;
				k=l;
				while(i<=mid && j<=r)
				{
					if(a[i]>a[j])
					{
						copy[k]=a[j];
						j++;
					}
					else
					{
						copy[k]=a[i];
						i++;
					}
					k++;
				}
				if(i<=mid)
				{
					while(i<=mid)
					{
						copy[k]=a[i];
						i++;
						k++;
					}

				}
				if(j<=r)
				{
					while(j<=r)
					{
						copy[k]=a[j];
						j++;
						k++;
					}

				}
				for(i=l; i<=r; i++)
				{
					a[i]=copy[i];
				}
			}
		}
		
	}
}

typedef struct arg1
{
	int l;
	int r;
}arg;


//concurrent merge sort with threads
void* mergesort3(void* args)
{
	int l=((struct arg1*)args)->l;
	int r=((struct arg1*)args)->r;
	
	//do selection sort if less than 5 elements
	if(r-l+1<=5)
	{
		selectionsort(c,l,r);
		return NULL;
	
	}
	else if(l!=r)
	{
		long long int mid;
		mid=(l+r)/2;
		pthread_t rthread,lthread;
		arg *arg1 = (arg*)malloc(sizeof(arg));
		arg1->l=l;
		arg1->r=mid;
		int error=pthread_create(&lthread,NULL,&mergesort3,(void*)arg1);
		if (error != 0)
		{ 
			printf("\nLeft Thread can't be created :[%s]", strerror(error));
			return NULL;
		}
		arg *arg2=(arg*)malloc(sizeof(arg));
		arg2->l=mid+1;
		arg2->r=r;
		error=pthread_create(&rthread,NULL,&mergesort3,(void*)arg2);
		if (error != 0)
		{ 
			printf("\nRight Thread can't be created :[%s]", strerror(error));
			return NULL;
		}
		pthread_join(lthread,NULL);
		pthread_join(rthread,NULL);

		long long int copy[10000],i,j,k;
		i=l;
		j=mid+1;
		k=l;
		while(i<=mid && j<=r)
		{
			if(c[i]>c[j])
			{
				copy[k]=c[j];
				j++;
			}
			else
			{
				copy[k]=c[i];
				i++;
			}
			k++;
		}
		if(i<=mid)
		{
			while(i<=mid)
			{
				copy[k]=c[i];
				i++;
				k++;
			}

		}
		if(j<=r)
		{
			while(j<=r)
			{
				copy[k]=c[j];
				j++;
				k++;
			}

		}
		for(i=l; i<=r; i++)
		{
			c[i]=copy[i];
		}
	}
}

//normal mergesort
void mergesort2(long long int a[], int l, int r)
{
	//printf("l : %d, r : %d\n",l,r);
	if(l!=r)
	{
		long long int mid;
		mid=(l+r)/2;
		mergesort2(a,l,mid);
		mergesort2(a,mid+1,r);
		long long int copy[10000],i,j,k;
		i=l;
		j=mid+1;
		k=l;
		while(i<=mid && j<=r)
		{
			if(a[i]>a[j])
			{
				copy[k]=a[j];
				j++;
			}
			else
			{
				copy[k]=a[i];
				i++;
			}
			k++;
		}
		if(i<=mid)
		{
			while(i<=mid)
			{
				copy[k]=a[i];
				i++;
				k++;
			}

		}
		if(j<=r)
		{
			while(j<=r)
			{
				copy[k]=a[j];
				j++;
				k++;
			}

		}
		for(i=l; i<=r; i++)
		{
			a[i]=copy[i];
		}
	}
}

int is_sorted(long long int a[],int n)
{
	int i;
	for(i=0; i<n-1; i++)
	{
		if(a[i]>a[i+1])
		{
			return 0;
		}
	}
	return 1;
}


int main()
{
	int n,i,shmid,error;
	long long int *a;
	long long int b[100000];
	key_t key=IPC_PRIVATE;
	scanf("%d",&n);
	
	//segment lenght : number of elements in array*size of one element
	size_t size=sizeof(long long int)*n;

	//creating the shared segment
	shmid=shmget(key,size,IPC_CREAT|0666);
	if(shmid<0)
	{
		perror("error in creating shared segment");
		_exit(1);
	}
	a=shmat(shmid,NULL,0);
	if(a==(long long int*)-1)
	{
		perror("error in attaching shared segment to data space");
		_exit(1);
	}
	for(i=0; i<n; i++)
	{
	//	printf("%d\n",i);
		scanf("%lld",&a[i]);
		b[i]=a[i];
		c[i]=a[i];
	}
	
	//mergesort with processes
	clock_t begin1=clock();
	mergesort(a,0,n-1);
	clock_t end1=clock();
	double time_spent_concurrent=0.0;
	time_spent_concurrent=(double)(end1-begin1)/CLOCKS_PER_SEC;
	int is_sorted_1=is_sorted(a,n);
	
	//mergesort normal
	clock_t begin2=clock();
	mergesort2(b,0,n-1);
	clock_t end2=clock();
	double time_spent_non_concurrent=0.0;
	time_spent_non_concurrent=(double)(end2-begin2)/CLOCKS_PER_SEC;
	int is_sorted_2=is_sorted(b,n);

	//mergesort with thread
	pthread_t sortthread;
	arg *args = (arg*)malloc(sizeof(arg));
	args->l=0;
	args->r=n-1;
	clock_t begin3=clock();
	error=pthread_create(&sortthread,NULL,&mergesort3,(void*)args);
	if (error != 0)
	{ 
		printf("\nMergeSort Thread can't be created :[%s]", strerror(error));
	}
	pthread_join(sortthread,NULL);
	clock_t end3=clock();
	double time_spent_threads=0.0;
	time_spent_threads=(double)(end3-begin3)/CLOCKS_PER_SEC;
	int is_sorted_3=is_sorted(c,n);

	FILE* out_file = fopen("ouput3","w");

	//writing output of processes in outfile
	for(i=0; i<n; i++)
	{
		fprintf(out_file,"%lld ",a[i]);
	}
	fprintf(out_file,"\n");
	fclose(out_file);

	//detaching shared segment 
	error=shmdt(a);
	if(error==-1)
	{
		perror("error while detaching shared segment");
		_exit(1);
	}

	//deleting the segment
	error=shmctl(shmid,IPC_RMID,NULL);
	if(error==-1)
	{
		perror("error while deleting segment");
		_exit(1);
	}
	
	
	printf("For n = %d\n",n);	
	printf("Time taken in merge sort with processes: %f seconds \n",time_spent_concurrent);
	printf("Time taken in merge sort with threads : %f seconds \n",time_spent_threads);
	printf("Time taken in normal merge sort : %f seconds \n",time_spent_non_concurrent);
	
	//checking sorted
	if(is_sorted_1==0)
		printf("Merge sort with processes failed\n");
	if(is_sorted_3==0)
		printf("Merge sort with threads failed\n");
	if(is_sorted_2==0)
		printf("Normal merge sort failed\n");


	//writing in report file
	FILE* report_file = fopen("report3","a");
	fprintf(report_file,"For n = %d\n",n);	
	fprintf(report_file,"Time taken in merge sort with processes: %f seconds \n",time_spent_concurrent);
	fprintf(report_file,"Time taken in merge sort with threads : %f seconds \n",time_spent_threads);
	fprintf(report_file,"Time taken in normal merge sort : %f seconds \n",time_spent_non_concurrent);
	if(is_sorted_1==0)
		fprintf(report_file,"Merge sort with processes failed\n");
	if(is_sorted_3==0)
		fprintf(report_file,"Merge sort with threads failed\n");
	if(is_sorted_2==0)
		fprintf(report_file,"Normal merge sort failed\n");
	fprintf(report_file,"\n");
	fclose(report_file);

	return 0;
}
