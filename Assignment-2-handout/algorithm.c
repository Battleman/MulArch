/*
============================================================================
Filename    : algorithm.c
Author      : Olivier Cloux
SCIPER		: 236079
============================================================================
*/

#define input(i,j) input[(i)*length+(j)]
#define output(i,j) output[(i)*length+(j)]
#define src(i,j) src[(i)*length+(j)]

double mean_(double* src, size_t row, size_t col, int length){
    double sum = 0.0;
    if(row == col){
        if(row == length/2-1){ //center square, don't modify
            return src(row, col);
        } else {
            //Diagonal, consider Diagonal elements once, others twice
            sum =
            src(row, col) + src(row-1, col-1)+  src(col+1, row+1) +
            2*(src(row, col-1)+ src(row+1, col-1) + src(row+1, col));
        }
    } else {
        if (row == col+1) {
            //line below diagonal, one special case to consider
            sum =
            src(row-1,col-1) + src(row-1,col) + /*src(row-1,col+1)*/+
            src(row,col-1) +src(row,col) + src(row,col+1) +
            2*src(row+1,col-1) + src(row+1,col) + src(row+1,col+1);
        } else {
            if(row == length/2-1){ //the bottom line
                sum =
                    2*(src(row-1, col-1)+src(row-1, col)+src(row-1,col+1))+
                    src(row, col-1)+src(row, col) + src(row, col+1);
            } else {
                sum =
                src(row-1,col-1)+
                src(row-1,col)+
                src(row-1,col+1)+
                src(row,col-1)+
                src(row,col)+
                src(row,col+1)+
                src(row+1,col-1)+
                src(row+1,col)+
                src(row+1,col+1);
            }
        }
    }
    return sum/9;
}

void simulate(double *input, double *output, int threads, int length, int iterations)
{
    double mean = 0.0;
    omp_set_num_threads(threads);
    for(int i = 0; i < iterations; i++)
    {
        #pragma omp parallel for shared(output, input) private(mean)
        for(size_t row = 1; row <= length/2-1; row++)
        {
            for(size_t col = 1; col <= row; col++)
            {
                if(i%2 == 0){
                    mean = mean_(input, row, col, length);
                    output(row, col) = mean;
                    output(length-1-row, col) = mean;
                    output(row, length-1-col) = mean;
                    output(length-1-row, length-1-col) = mean;

                    output(col, row) = mean;
                    output(length-1-col, row) = mean;
                    output(col, length-1-row) = mean;
                    output(length-1-col, length-1-row) = mean;
                } else {
                    mean = mean_(output, row, col, length);
                    input(row, col) = mean;
                    input(length-1-row, col) = mean;
                    input(row, length-1-col) = mean;
                    input(length-1-row, length-1-col) = mean;

                    input(col, row) = mean;
                    input(length-1-col, row) = mean;
                    input(col, length-1-row) = mean;
                    input(length-1-col, length-1-row) = mean;
                }
            }//END OF column iteration
        } //END OF PARALLEL FOR
    } //END OF ITERATION
    if(iterations%2 == 0){
        //if even number of iterations, need to ensure
        //'output' is the final placeholder
        memcpy(output, input, sizeof(double)*length*length);
    }
}
