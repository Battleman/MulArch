/*
============================================================================
Filename    : pi.c
Author      : Olivier Cloux
SCIPER		 : 236079
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

double calculate_pi (int num_threads, unsigned long int samples);

int main (int argc, const char *argv[]) {

  int num_threads;
  unsigned long num_samples;
  double pi;
  if (argc != 3) {
    printf("Invalid input! Usage: ./pi <num_threads> <num_samples> \n");
    return 1;
  } else {
    num_threads = atoi(argv[1]);
    num_samples = atol(argv[2]);
  }

  set_clock();
  pi = calculate_pi (num_threads, num_samples);

  printf("- Using %d threads: pi = %.15g computed in %.4gs.\n", num_threads, pi, elapsed_time());

  return 0;
}

double calculate_pi (int num_threads, unsigned long int samples) {
  double pi = 0;
  double x,y = 0;
  unsigned long int inside = 0;
  omp_set_num_threads(num_threads);

  #pragma omp parallel private(x,y) reduction(+:inside)
  {
    rand_gen gen = init_rand();
    for (unsigned long int i = 0; i < samples/num_threads; i++)
    {
      x = next_rand(gen);
      y = next_rand(gen);
      if(x*x+y*y <= 1){
        inside += 1;
      }
    }
  }

  pi = 4.0*inside/samples;
  return pi;
}
