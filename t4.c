#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lodepng.h"

int **x, **y;
typedef struct ThreadData
{
  int start;
  int end;
  int width;
  int height;
} ThreadData;

void *applyBlur(void *arg)
{
  struct ThreadData *data = (struct ThreadData *)arg;
  int start = data->start;
  int end = data->end;
  int width = data->width;
  int height = data->height;
  int i, j, n;
  for (n = start; n <= end; i++)
  {
    int column = n, row = 0, sumR = 0, sumG = 0, sumB = 0;

    if (column >= width)
    {
      row = column / width;
      if (column % width != 0)
        row++;
      column = column % width;
    }
    for (i = row - 1; i < row + 2; i++)
    {
      for (j = column - 1; j < column + 2; j++)
      {
        if (i > height || i < 0 || j > width || j < 0)
        {
          continue;
        }
        else
        {
          sumR += x[i][4 * j + 0];
          sumG += x[i][4 * j + 1];
          sumB += x[i][4 * j + 2];
        }
      }
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

  unsigned error;
  unsigned char *image;
  unsigned int width, height;
  int i, j;

  error = lodepng_decode32_file(&image, &width, &height, "baby.png");
  if (error)
  {
    printf("Error while decoding image %u: %s", error, lodepng_error_text(error));
  }

  x = (int **)malloc(height * sizeof(int *));
  y = (int **)malloc(height * sizeof(int *));
  for (i = 0; i < height; i++)
  {
    x[i] = (int *)malloc(width * 4 * sizeof(int));
    y[i] = (int *)malloc(width * 4 * sizeof(int));
  }

  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      x[i][4 * j + 0] = image[4 * width * i + 4 * j + 0];
      x[i][4 * j + 1] = image[4 * width * i + 4 * j + 1];
      x[i][4 * j + 2] = image[4 * width * i + 4 * j + 2];
      x[i][4 * j + 3] = image[4 * width * i + 4 * j + 3];
    }
  }

  int totalPixel = width * height;
  int sliceList[noOfThreads];
  int remainder = totalPixel % noOfThreads;
  for (i = 0; i < noOfThreads; i++)
  {
    sliceList[i] = totalPixel / noOfThreads;
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
    data[i].width = width;
    data[i].height = height;
  }

  pthread_t *thread = malloc(noOfThreads * sizeof(pthread_t));
  for (i = 0; i < noOfThreads; i++)
  {
    pthread_create(&thread[i], NULL, applyBlur, &data[i]);
  }

  for (i = 0; i < noOfThreads; i++)
  {
    pthread_join(thread[i], NULL);
  }

  size_t pngsize;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      image[4 * width * i + 4 * j + 0] = y[i][4 * j + 0];
      image[4 * width * i + 4 * j + 1] = y[i][4 * j + 1];
      image[4 * width * i + 4 * j + 2] = y[i][4 * j + 2];
      image[4 * width * i + 4 * j + 3] = y[i][4 * j + 3];
    }
  }
  error = lodepng_encode32(&image, &pngsize, image, width, height);
  if (error)
  {
    printf("Error while encoding image %u: %s", error, lodepng_error_text(error));
  }
  else
  {
    lodepng_save_file(image, pngsize, "output.png");
    printf("Check 'output.png' for blurred image.\n");
  }
  free(image);
}