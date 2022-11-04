#include <stdio.h>
#include <stdlib.h>

void main()
{
  FILE *fp = fopen("datasetLR1.txt", "r");
  float x[100], y[100];
  int n = 0;

  float sumX = 0, sumY = 0;
  float sumXsquare = 0, sumYsquare = 0;
  float sumXY = 0;

  float a, b;
  float input, output;

  while (fscanf(fp, "%f,%f", &x[n], &y[n]) != EOF)
  {
    n++;
  }
  fclose(fp);

  for (int i = 0; i < n; i++)
  {
    sumX += x[i];
    sumY += y[i];
    sumXsquare += x[i] * x[i];
    sumYsquare += y[i] * y[i];
    sumXY += x[i] * y[i];
  }

  a = ((sumY * sumXsquare) - (sumX * sumXY)) / ((n * sumXsquare) - (sumX * sumY));
  b = ((n * sumXY) - (sumX * sumY)) / ((n * sumXsquare) - (sumX * sumX));

  printf("Equation: y = %f * x + %f \n", b, a);

  printf("Enter value of x: ");
  scanf("%f", &input);

  output = b * input + a;

  printf("\nPredicted value for %f is %f.\n", input, output);
}