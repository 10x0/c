#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
int *x, count = 0;

typedef struct ThreadData
{
  int start;
  int end;
} ThreadData;

void *chechPrime(void *arg)
{
  struct ThreadData *data = (struct ThreadData *)arg;
  int start = data->start;
  int end = data->end;
  int i, j;
  for (i = start; i <= end; i++)
  {
    int check = 0;
    if (x[i] <= 1)
    {
      check = 1;
    }
    for (j = 2; j <= x[i] / 2; ++j)
    {
      if (x[i] % j == 0)
      {
        check = 1;
        break;
      }
    }
    if (check == 0)
    {
      pthread_mutex_lock(&mutex);
      FILE *fp = fopen("prime.txt", "a");
      fprintf(fp, "%d\n", x[i]);
      printf("%d\n", x[i]);
      fclose(fp);
      count++;
      pthread_mutex_unlock(&mutex);
    }
  }
  pthread_exit(NULL);
}

void main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Invalid number of arguments.\n");
    return;
  }
  int noOfThreads = atoi(argv[1]);
  int noOfData = 0, i = 0, temp;

  FILE *fp = fopen("PrimeData1.txt", "r");
  while (fscanf(fp, "%d", &temp) != EOF)
  {
    noOfData++;
  }
  fclose(fp);

  x = malloc(noOfData * sizeof(int));
  if (x == NULL)
  {
    printf("Memory not allocated");
    exit(0);
  }

  fp = fopen("PrimeData1.txt", "r");
  while (fscanf(fp, "%d", &x[i]) != EOF)
  {
    i++;
  }
  fclose(fp);

  int sliceList[noOfThreads];
  int remainder = noOfData % noOfThreads;
  for (i = 0; i < noOfThreads; i++)
  {
    sliceList[i] = noOfData / noOfThreads;
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

  fp = fopen("prime.txt", "w");
  fclose(fp);
  pthread_t thread[noOfThreads];
  for (i = 0; i < noOfThreads; i++)
  {
    pthread_create(&thread[i], NULL, chechPrime, &data[i]);
  }

  for (i = 0; i < noOfThreads; i++)
  {
    pthread_join(thread[i], NULL);
  }
  pthread_mutex_destroy(&mutex);

  printf("Total %d prime numbers in the file: PrimeData1.txt\n", count);
}