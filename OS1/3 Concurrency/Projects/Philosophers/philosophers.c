//ENRIQUE CAVANILLAS PUGA


#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];
pthread_mutex_t forks[NR_PHILOSOPHERS];


void init()
{
    int i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i],NULL);
    
}

void think(int i) {
    printf("Philosopher %d thinking... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n" , i);

}

void eat(int i) {
    printf("Philosopher %d eating... \n" , i);
    sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n" , i);

}

void toSleep(int i) {
    printf("Philosopher %d sleeping... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n" , i);
    
}

void* philosopher(void* i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);

    if(nPhilosopher == 0){ //intercambiamos los tenedores
        left = right;
        right = 0;
    }
    while(1)
    {
        
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)

        pthread_mutex_lock(&forks[left]);
        printf("Philosopher %d takes his left fork %d\n", nPhilosopher, left);
        pthread_mutex_lock(&forks[right]);
        printf("Philosopher %d takes his right fork %d\n", nPhilosopher, right);

        eat(nPhilosopher);

        pthread_mutex_unlock(&forks[left]);
        printf("Philosopher %d left his left fork %d\n", nPhilosopher, left);
        pthread_mutex_unlock(&forks[right]);
        printf("Philosopher %d left his right fork %d\n", nPhilosopher, right);
        
        // PUT FORKS BACK ON THE TABLE
        
        toSleep(nPhilosopher);
   }

}

int main()
{
    init();
    unsigned long i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);
    return 0;
} 
