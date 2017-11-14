/*
 ============================================================================
 Filename    : assignment2.c
 Author      : Arash Pourhabibi, Hussein Kassir
 ============================================================================
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "algorithm.c"

int main (int argc, const char *argv[]) {

    int threads, length, iterations;
    double time;

    if (argc != 5) {
		printf("Invalid input! \nUsage: ./assignment2 <threads> <length> <iterations> <output_filname>\n");
		return 1;
	} else {
        threads     = atoi(argv[1]);
        length      = atoi(argv[2]);
        iterations  = atoi(argv[3]);
        if(length%2!=0)
        {
            printf("Invalid input! Array length must be even\n");
            return 1;
        }
	}
  // printf("My parameters are %d,%d,%d\n",threads,length,iterations);

    //Allocate a two-dimensional array
    double *input  = malloc(sizeof(double)*length*length);
    double *output = malloc(sizeof(double)*length*length);
    if(NULL == input || NULL == output){
      printf("No memory could be allocated");
      return -1;
    }


    // /*##################
    // *####  eigth_switch #####
    // *##################*/
    // //Initialize the array
    // init(input, length);
    // init(output, length);
    // //Start timer
    // set_clock();
    // //Optimize the following function
    // eighth_switch(input, output, threads, length, iterations);
    // //Stop timer
    // time = elapsed_time();
    // //Report time required for n iterations
    // printf("Running the algorithm EIGTH_SWITCH with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    // //Save array in filelength
    // save(output, length, "output_eigth_switch.csv");


    // /*##################
    // *####  half_vertical_switch #####
    // *##################*/
    // //Initialize the array
    // init(input, length);
    // init(output, length);
    // //Start timer
    // set_clock();
    // //Optimize the following function
    // half_vertical_switch(input, output, threads, length, iterations);
    // //Stop timer
    // time = elapsed_time();
    // //Report time required for n iterations
    // printf("Running the algorithm HALF_VERTICAL_SWITCH with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    // //Save array in filelength
    // save(output, length, "output_half_vertical_switch.csv");
    //
    //
    // /*##################
    // *####  half_horizontal_switch #####
    // *##################*/
    // //Initialize the array
    // init(input, length);
    // init(output, length);
    // //Start timer
    // set_clock();
    // //Optimize the following function
    // half_horizontal_switch(input, output, threads, length, iterations);
    // //Stop timer
    // time = elapsed_time();
    // //Report time required for n iterations
    // printf("Running the algorithm HALF_HORIZONTAL_SWITCH with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    // //Save array in filelength
    // save(output, length, "output_half_horizontal_switch.csv");
    //
    //
    // /*##################
    // *####  whole_switch #####
    // *##################*/
    // //Initialize the array
    // init(input, length);
    // init(output, length);
    // //Start timer
    // set_clock();
    // //Optimize the following function
    // whole_switch(input, output, threads, length, iterations);
    // //Stop timer
    // time = elapsed_time();
    // //Report time required for n iterations
    // printf("Running the algorithm WHOLE_SWITCH with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    // //Save array in filelength
    // save(output, length, "output_whole_switch.csv");
    //
    //
    // /*#########################
    // *####  whole_noSwitch #####
    // *##########################*/
    // //Initialize the array
    // init(input, length);
    // init(output, length);
    // //Start timer
    // set_clock();
    // //Optimize the following function
    // whole_noSwitch(input, output, threads, length, iterations);
    // //Stop timer
    // time = elapsed_time();
    // //Report time required for n iterations
    // printf("Running the algorithm WHOLE_NOSWITCH with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    // //Save array in filelength
    // save(output, length, "output_whole_noSwitch.csv");
    //
    // /*##################
    // *####  QUARTER #####
    // *##################*/
    // //Initialize the array
    // init(input, length);
    // init(output, length);
    // //Start timer
    // set_clock();
    // //Optimize the following function
    // whole_noSwitch(input, output, threads, length, iterations);
    // //Stop timer
    // time = elapsed_time();
    // //Report time required for n iterations
    // printf("Running the algorithm QUARTER_SWITCH with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    // //Save array in filelength
    // save(output, length, "output_quarter_switch.csv");


    /*###################
    *####  simulate #####
    *###################*/
    //Initialize the array
    init(input, length);
    init(output, length);
    //Start timer
    set_clock();
    //Optimize the following function
    simulate(input, output, threads, length, iterations);
    //Stop timer
    //Report time required for n iterations
    time = elapsed_time();
    printf("Running the algorithm SIMULATE with %d threads on %d by %d array for %d iteration takes %.4gs seconds \n", threads, length, length, iterations, time);
    //Save array in filelength
    save(output, length, argv[4]);



    //Free allocated memory
    free(input);
    free(output);


    return 0;
}
