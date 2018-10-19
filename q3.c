#include <sys/types.h> 
#include <sys/wait.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <time.h> 

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
		//printf("not equal\n");
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
			//printf("in left child\n");
			mergesort(a,l,mid);
			_exit(0);
		}
		//if inside the parent
		else
		{
			//printf("in parent\n");
			rpid=fork();
			if(rpid<0)
			{
				perror("Error creating right child\n");
				_exit(-1);
			}
			//if inside right child
			if(rpid==0)
			{
				//printf("in right child\n");
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
	}
	
	clock_t begin1=clock();
	mergesort(a,0,n-1);
	clock_t end1=clock();
	double time_spent_concurrent=0.0;
	time_spent_concurrent=(double)(end1-begin1)/CLOCKS_PER_SEC;

	clock_t begin2=clock();
	mergesort2(b,0,n-1);
	clock_t end2=clock();
	double time_spent_non_concurrent=0.0;
	time_spent_non_concurrent=(double)(end2-begin2)/CLOCKS_PER_SEC;

	FILE* out_file = fopen("ouput3","w");

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
	printf("Time taken in concurrent merge sort : %f seconds \n",time_spent_concurrent);
	printf("Time taken in normal merge sort : %f seconds \n",time_spent_non_concurrent);

	FILE* report_file = fopen("report3","a");
	fprintf(report_file,"For n = %d\n",n);	
	fprintf(report_file,"Time taken in concurrent merge sort : %f seconds \n",time_spent_concurrent);
	fprintf(report_file,"Time taken in normal merge sort : %f seconds \n",time_spent_non_concurrent);
	fprintf(report_file,"\n");
	fclose(report_file);

	return 0;
}
