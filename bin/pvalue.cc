#include "TMath.h"
unsigned long fact(unsigned int);
double upperincompletegamma(double s, double x);
double gamma(double x);
double chicdf(unsigned int k, double x);
int main()
{
  printf("%lu\n", fact(4));
  printf("%f\n", gamma(5/2.0));
  printf("%f\n", upperincompletegamma(1.5, 5.0));
  printf("%f\n", chicdf(5, 10.0));
}

unsigned long fact(unsigned int n)
{
  if (n == 0)
    return 1;
  if (n == 1)
    return 1;
  return n * fact(n - 1);
}

double upperincompletegamma(double s, double x)
{
  double res = 0.0;
  for (unsigned int k = 0; k < 100; k ++)
    {
      
      res += TMath::Power(-x, (double) k)/(fact(k) * (s + k));
      printf("res %f %u \n", res, k);
    }
  res *= TMath::Power(x, s);
  return res;
}

double gamma(double x)
{
  if (x == 0.5)
    return TMath::Pi();
  return (x - 1) * gamma(x- 1);
}

double chicdf(unsigned int k, double x)
{
  return upperincompletegamma(k/2.0, x/2.0)/gamma(k/2.0);
}
