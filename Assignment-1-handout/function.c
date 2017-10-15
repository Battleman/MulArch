/*
============================================================================
Filename    : function.c
Author      : Olivier Cloux
SCIPER		 : 236079
============================================================================
*/

double identity_f (double x){
    return x;
}

double square_f (double x){
  return x*x;
}

double inverse_f (double x){
  return 1/x;
}

double complicated_f (double x){
  return (3*x*x*x + 2*x)/(5*x*x);
}
