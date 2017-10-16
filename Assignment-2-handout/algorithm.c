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
            // printf("Thread %d, on element (%ld,%ld), center square\n", omp_get_thread_num(), row, col);
            return src(row, col);
        } else {
            //Diagonal, consider Diagonal elements once, others twice
            // printf("Thread %d, on element (%ld,%ld), diagonal element\n", omp_get_thread_num(), row, col);
            sum =
            src(row, col) + src(row-1, col-1)+  src(col+1, row+1) +
            2*(src(row, col-1)+ src(row+1, col-1) + src(row+1, col));
        }
    } else {
        if (row == col+1) {
            if(row == length/2-1){
                // printf("Thread %d, on element (%ld,%ld), that special fucker\n", omp_get_thread_num(), row, col);
                sum =
                    src(row-1, col-1)+src(row-1, col)+
                    2*src(row, col-1)+3*src(row, col) + 2*src(row, col+1);
            } else {
                //line below diagonal, one special element to consider
                // printf("Thread %d, on element (%ld,%ld), below diagonal\n", omp_get_thread_num(), row, col);
                sum =
                src(row-1,col-1) + src(row-1,col) /*+src(row-1,col+1)*/+
                src(row,col-1) +src(row,col) + src(row,col+1) +
                2*src(row+1,col-1) + src(row+1,col) + src(row+1,col+1);
            }
        } else {
            if(row == length/2-1){ //the bottom line
                // printf("Thread %d, on element (%ld,%ld), bottom line\n", omp_get_thread_num(), row, col);
                sum =
                    src(row-1, col-1)+src(row-1, col)+src(row-1,col+1)+
                    2*(src(row, col-1)+src(row, col) + src(row, col+1));
            } else {
                // printf("Thread %d, on element (%ld,%ld), about any other element\n", omp_get_thread_num(), row, col);
                sum =
                src(row-1,col-1)+src(row-1,col)+src(row-1,col+1)+
                src(row,col-1) + src(row,col) + src(row,col+1)+
                src(row+1,col-1)+src(row+1,col)+src(row+1,col+1);
            }
        }
    }
    return sum/9;
}

void simulate_(double *input, double *output, int threads, int length, int iterations)
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

void simulate(double *input, double *output, int threads, int length, int iterations)
{
    omp_set_num_threads(threads);
    for (size_t i = 0; i < iterations; i++) {
        #pragma omp parallel for
        for (size_t row = 1; row < length-1; row++) {
            for (size_t col = 1; col < length-1; col++) {
                if(
                    ((row == length/2 || row == length/2-1) && col == length/2) ||
                    ((row == length/2 || row == length/2-1) && col == length/2-1)
                ){
                    continue;
                } else {
                    output(row, col) =(
                        input(row-1, col-1) +input(row-1, col) +input(row-1, col+1) +
                        input(row, col-1) +input(row, col) +input(row, col+1) +
                        input(row+1, col-1) +input(row+1, col) +input(row+1, col+1))/9;
                }
            }
        }
        memcpy(input, output, sizeof(double)*length*length);
    }

}
