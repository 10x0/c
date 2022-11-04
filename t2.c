#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

float sum;
pthread_mutex_t mutex;
typedef struct ThreadData
{
  int start;
  int end;
} ThreadData;

double npwr(int n)
{
  int r = 1;
  int i;
  for (i = 0; i < n; i++)
  {
    r = -1 * r;
  }
  return r;
}

void *calculateSum(void *arg)
{
  struct ThreadData *data = (struct ThreadData *)arg;
  int start = data->start;
  int end = data->end;
  int i;
  float temp;
  for (i = start; i <= end; i++)
  {
    temp = npwr(i) / (2 * i + 1);
    pthread_mutex_lock(&mutex);
    sum += temp;
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

void main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Invalid number of arguments.\n");
    return;
  }
  int iterations = atoi(argv[1]);
  int noOfThreads = atoi(argv[2]);
  int i;

  int sliceList[noOfThreads];
  int remainder = iterations % noOfThreads;
  for (i = 0; i < noOfThreads; i++)
  {
    sliceList[i] = iterations / noOfThreads;
    if (remainder > 0)
    {
      sliceList[i]++;
      remainder--;
    }
  }

  struct ThreadData data[noOfThreads];
  for (i = 0; i < noOfThreads; i++)
  {
    if (i == 0)
    {
      data[i].start = 0;
    }
    else
    {
      data[i].start = data[i - 1].end + 1;
    }
    data[i].end = data[i].start + sliceList[i] - 1;
  }

  pthread_t thread[noOfThreads];
  for (i = 0; i < noOfThreads; i++)
  {
    pthread_create(&thread[i], NULL, calculateSum, &data[i]);
  }

  for (i = 0; i < noOfThreads; i++)
  {
    pthread_join(thread[i], NULL);
  }
  pthread_mutex_destroy(&mutex);

  float PI = 4 * sum;
  printf("PI: %f\n", PI);
}