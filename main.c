#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

sem_t counterAccess; /// semaphore for the counter variable
sem_t Sembuffer; ///  semaphore for the buffer
sem_t emptyBuffer; ///  semaphore for the empty buffer
sem_t fullBuffer; ///  semaphore for the full buffer

pthread_t *mcounter;
pthread_t monitor;
pthread_t collector;
int mCount,n;

// queue variables
int *buffer;
int sizeOfBuffer;
int front = 0;
int rear = -1;
int itemCount = 0;

int peek();
bool isEmpty();
bool isFull();
int size();
void insert(int data);
int removeData();
void* mCounterFunc(void *arg);
niCounters(int n);
void* monitorFunc(void *arg);
void* collectorFunc(void *arg);





int main()
{

    printf("Enter number of Counter Threads: ");
    scanf("%d",&n);
    sizeOfBuffer = n;
    sem_init(&counterAccess,0,1); /// init the semaphores
    sem_init(&Sembuffer,0,1);
    sem_init(&emptyBuffer,0,sizeOfBuffer);
    sem_init(&fullBuffer,0,0);
    buffer = malloc(n* sizeof(int));
    iniCounters(n);

    return 0;
}

void iniCounters(int n)
{
    int randomNumber;
    mcounter = malloc(n*sizeof(pthread_t));

    /// create counter threads based on user input n
    for(int i = 0 ; i < n ; i++)
    {
        pthread_create(&mcounter[i],NULL,mCounterFunc,i);
    }
    pthread_create(&monitor,NULL,monitorFunc,NULL); // create monitor thread
    pthread_create(&collector,NULL,collectorFunc,NULL); // create collector thread

    for(int i = 0 ; i < n ; i++)
    {

        pthread_join(mcounter[i],NULL);

    }
    pthread_join(monitor,NULL);
    pthread_join(collector,NULL);

}


void* mCounterFunc(void *arg)
{
    while(1)
    {
        int randomNumber = rand() % 20; // from 0 to 19
        sleep(5);
        printf("Counter thread %d: Received a message \n",arg);
        printf("Counter thread %d: Waiting to wirte\n",arg);
        sem_wait(&counterAccess);
        /// critical section
        mCount += randomNumber;
        printf("Counter thread %d: Now adding to counter %d\n",arg,mCount);
        sem_post(&counterAccess);
    }
}


void* monitorFunc(void *arg)
{
    while(1)
    {
        printf("Monitor thread: waiting to read counter\n");
        sleep((rand()%5));
        sem_wait(&emptyBuffer);
        sem_wait(&Sembuffer);
        // critical section
        printf("Monitor thread: Reading counter %d\n",mCount);
        if(!isFull())
        {
            printf("Monitor thread: Writing to buffer at position %d\n",size());
            insert(mCount);
            mCount = 0;
        }
        else
        {
            printf("Monitor thread: Buffer is full!!\n");
        }
        sem_post(&Sembuffer);
        sem_post(&fullBuffer);


    }

}

void* collectorFunc(void *arg)
{
    while(1)
    {
        sleep((rand()%5));
        sem_wait(&fullBuffer);
        sem_wait(&Sembuffer);
        ///critical section
        /// chack if buffer is empty
        if(isEmpty())
        {
            printf("Collector thread: Nothing in the buffer!!\n");
        }
        else
        {
            printf("Collector thread: Reading from buffer position %d\n", size());
            int removedData = removeData();

        }
        sem_post(&Sembuffer);
        sem_post(&emptyBuffer);


    }

}


int peek()
{
    return buffer[front];
}

bool isEmpty()
{
    return itemCount == 0;
}

bool isFull()
{
    return itemCount == sizeOfBuffer;
}

int size()
{
    return itemCount;
}

//enqueue function
void insert(int data)
{

    if(!isFull())
    {

        if(rear == sizeOfBuffer-1)
        {
            rear = -1;
        }

        buffer[++rear] = data;
        itemCount++;
    }
}

// dequeue function
int removeData()
{
    int data = buffer[front++];

    if(front == sizeOfBuffer)
    {
        front = 0;
    }

    itemCount--;
    return data;
}


