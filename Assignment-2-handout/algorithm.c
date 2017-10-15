/*
============================================================================
Filename    : algorithm.c
Author      : Olivier Cloux
SCIPER		: 236079
============================================================================
*/

#define input(i,j) input[(i)*length+(j)]
#define output(i,j) output[(i)*length+(j)]
#define temp_square(i,j) temp_square[(i)*length+(j)]
#define mean(i,j)  (input(i-1,j-1)+input(i-1,j)+input(i-1,j+1)+input(i,j-1)+input(i,j)+input(i,j+1)+input(i+1,j-1)+input(i+1,j)+input(i+1,j+1))/9
// double mean(double* input, int row, int col){
//   double total = 0.0;
//   for(int i = row-1, i <= row+1;row++){
//     for (int j = col-1; j <= col+1; j++) {
//       total += input(i,j);
//     }
//   }
//   return total/9;
// }

void simulate(double *input, double *output, int threads, int length, int iterations)
{
  omp_set_num_threads(threads);
  int inMiddleRow = 0;
  for(int i = 0; i < iterations; i++)
  {
    #pragma omp parallel for private(inMiddleRow)
    for(int row = 1; row < length-1; row++)
    {
          if(row == length/2-1  || row == length/2){
        inMiddleRow=1;
      } else {
        inMiddleRow=0;
      }
      for(int col = 1; col < length-1; col++)
      {
        if(((row == length/2-1  || row == length/2) && col == length/2-1) ||
           ((row == length/2-1  || row == length/2) && col == length/2)) {
          continue;
        } else {
          output(row, col) = mean(row, col);
        }
      }
    } //END OF PARALLEL FOR
    // memcpy(input, output, sizeof(double)*length*length);
  }


}
