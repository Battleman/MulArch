/*
============================================================================
Filename    : integral.c
Author      : Your name goes here
SCIPER		 : Your SCIPER number
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "function.c"

double integrate (int num_threads, int samples, int a, int b, double (*f)(double));

int main (int argc, const char *argv[]) {

  int num_threads, num_samples, a, b;
  double integral;

  if (argc != 5) {
    printf("Invalid input! Usage: ./integral <num_threads> <num_samples> <a> <b>\n");
    return 1;
  } else {
    num_threads = atoi(argv[1]);
    num_samples = atoi(argv[2]);
    a = atoi(argv[3]);
    b = atoi(argv[4]);
  }

  set_clock();

  /* You can use your self-defined funtions by replacing identity_f. */
  integral = integrate (num_threads, num_samples, a, b, identity_f);

  printf("- Using %d threads: integral on [%d,%d] = %.15g computed in %.4gs.\n", num_threads, a, b, integral, elapsed_time());

  return 0;
}

double integrate (int num_threads, int samples, int a, int b, double (*f)(double)) {
  //Initialization
  double integral = 0.0;
  double x = 0.0;
  omp_set_num_threads(num_threads);

  //computation
  #pragma omp parallel private(x) reduction(+:integral)
  {
    rand_gen gen = init_rand();
    for (long int i = 0; i < samples/num_threads; i++) {
      x = next_rand(gen)*(b-a) + a; //map to the correct intervall
      integral += (b-a)*f(x);
    }
  }

  //adjustment
  return integral/samples;
}
