#include <stdio.h>
#include "utility.h"

int main(int argc, const char* *argv[]){
	rand_gen gen = init_rand();
	int a = 4;
	int b = 9;
	double x = next_rand(gen);
	double y = (x*(b-a))+a;
	printf("x value : %f, adapted value in [5,9] : %f",x,y);

	return 0;
}
