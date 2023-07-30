#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t candado;

int total_sum = 0;
void * partial_sum(void * arg) {
  int j = 0;
  int ni=((int*)arg)[0];
  int nf=((int*)arg)[1];
  int suma_local = 0;

  for (j = ni; j <= nf; j++)
    suma_local = suma_local + j;
    
  pthread_mutex_lock(&candado);
  total_sum = total_sum + suma_local;
  pthread_mutex_unlock(&candado);

  pthread_exit(0);
}

int main(void) {
  pthread_t th1, th2;
  int num1[2]={  1,   4999};
  int num2[2]={5000, 10000};

  /* Two threads are created */
  pthread_create(&th1, NULL, partial_sum, (void*)num1);
  pthread_create(&th2, NULL, partial_sum, (void*)num2);

  /* the main thread waits until both threads complete */
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  printf("total_sum=%d and it should be 50005000\n", total_sum);

  return 0;
}
