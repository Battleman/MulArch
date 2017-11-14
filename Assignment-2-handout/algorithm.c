/*
============================================================================
Filename    : algorithm.c
Author      : Olivier Cloux
SCIPER		: 236079
============================================================================
*/

#define input(i,j) input[(i)*length+(j)]
#define output(i,j) output[(i)*length+(j)]

#define small_input(i,j) small_input[((j) + ((i)*(i)) - ((i)*((i)-1)/2))]
#define small_output(i,j) small_output[((j) + ((i)*(i)) - ((i)*((i)-1)/2))]
#define src(i,j) src[((j) + ((i)*(i)) - ((i)*((i)-1)/2))]


double meanify(size_t row, size_t col, double* src, int length){
    double mean = 0.0;
    if (row == col) {
        if(row == length/2-1) {
            //center square, pass
            mean = src(row, col);
            return mean;
        } else {
            //diag element
            mean =  (src(row, col) + src(row-1, col-1) + src(row+1, col+1) +
            2*(src(row+1, col) + src(row, col-1) + src(row+1, col-1)))/9;
            return mean;
        }
    }
    if(row == col+1){
        if(row == length/2-1) {
            //below diag, bottom line
            mean = (3*src(row, col) + 2*src(row, col-1) +
            2*src(row, col+1) + src(row-1, col-1) + src(row-1, col))/9;
            return mean;
        } else {
            //any other below diag
            mean =(
                2*src(row, col) + src(row-1, col-1) + src(row-1, col) +
                src(row, col-1) + src(row, col+1) + src(row+1, col-1) +
                src(row+1, col) + src(row+1, col+1))/9;
            return mean;
        }
    }
    if(row == length/2-1){
        //normal bottom line
        mean = (src(row-1, col-1) + src(row-1, col) + src(row-1, col+1) +
        2*(src(row, col-1) + src(row, col) + src(row, col+1)))/9;
        return mean;
    }
    //any other case
    mean = (src(row-1, col-1) + src(row-1, col) + src(row-1, col+1) +
            src(row, col-1) + src(row, col) + src(row, col+1) +
            src(row+1, col-1) + src(row+1, col) + src(row+1, col+1))/9;
    return mean;
}

void simulate(double *input, double *output, int threads, int length, int iterations){
    //Work in a smaller tab (around 1/8 of cases), and only at the end
    int numberOfCases = (length*length/8.0 + length/4.0);
    double* small_input = malloc(sizeof(double)*numberOfCases);
    double* small_output = malloc(sizeof(double)*numberOfCases);
    if(small_input == NULL || small_output == NULL){
        printf("Impossible to allocate memory, aborting");
        return;
    }
    memset(small_input, 0, sizeof(double)*numberOfCases);
    memset(small_output, 0, sizeof(double)*numberOfCases);
    small_input(length/2-1, length/2-1) = input(length/2-1, length/2-1);

    omp_set_num_threads(threads);
    double mean = 0.0;
    for (size_t i = 0; i < iterations; i++) {
        #pragma omp parallel for private(mean)
        for (size_t row = 1; row < length/2; row++) {
            for (size_t col = 1; col <= row; col++) {
                if(i%2 == 0){ //get mean from correct source
                    mean =  meanify(row, col, small_input, length);;
                } else {
                    mean = meanify(row, col, small_output, length);
                }
                if(i == iterations-1){
                    //If at the last iteration, write to output
                    output(row, col) = mean;
                    output(length-1-row, col) = mean;
                    output(row, length-1-col) = mean;
                    output(length-1-row, length-1-col) = mean;

                    output(col, row) = mean;
                    output(length-1-col, row) = mean;
                    output(col, length-1-row) = mean;
                    output(length-1-col, length-1-row) = mean;
                } else {
                    //else, keep computing
                    if(i%2 == 0){
                        small_output(row, col) = mean;
                    } else {
                        small_input(row, col) = mean;
                    }
                }
            }
        }
    }

    //free the temporary tables
    free(small_input);
    free(small_output);
}

void eighth_switch(double *input, double *output, int threads, int length, int iterations)
{
    /*Only read 1/8 of the square (bottom half of the top left quarter) and do 8 assignments
    * per iteration. Also switch the roles of source-destination between input and output
    * at each iteration, to avoid memory overhead.
    */
    double mean = 0.0;
    omp_set_num_threads(threads);
    for(int i = 0; i < iterations; i++)
    {
        #pragma omp parallel for shared(output, input) private(mean)
        for(size_t row = 1; row <= length/2-1; row++)
        {
            for(size_t col = 1; col <= row; col++)
            {
                if(row == col && row == length/2-1){
                    continue;
                }
                if(i%2 == 0){
                    mean =  (input(row-1,col-1)+input(row-1,col)+input(row-1,col+1)+
                            input(row,col-1)   +input(row,col)  +input(row,col+1)+
                            input(row+1,col-1) +input(row+1,col)+input(row+1,col+1))/9;

                    output(row, col) = mean;
                    output(length-1-row, col) = mean;
                    output(row, length-1-col) = mean;
                    output(length-1-row, length-1-col) = mean;

                    output(col, row) = mean;
                    output(length-1-col, row) = mean;
                    output(col, length-1-row) = mean;
                    output(length-1-col, length-1-row) = mean;
                } else {
                    mean = (output(row-1,col-1)+output(row-1,col)+output(row-1,col+1)+
                            output(row,col-1) + output(row,col) + output(row,col+1)+
                            output(row+1,col-1)+output(row+1,col)+output(row+1,col+1))/9;

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

void half_vertical_switch(double *input, double *output, int threads, int length, int iterations)
{
    /*Split square in half vertically, and use symmetry to complete the other half.
    * Use switching.
    */
    double mean = 0.0;
    omp_set_num_threads(threads);
    for(int i = 0; i < iterations; i++)
    {
        #pragma omp parallel for shared(output, input) private(mean)
        for(size_t row = 1; row < length-1; row++)
        {
            for(size_t col = 1; col <= length/2-1; col++)
            {
                if(col == length/2-1 && (row == length/2 || row == length/2-1)){
                    //the 2 center squares
                    continue;
                }
                if(i%2 == 0){
                    mean =  (input(row-1,col-1)+input(row-1,col)+input(row-1,col+1)+
                            input(row,col-1) + input(row,col) + input(row,col+1)+
                            input(row+1,col-1)+input(row+1,col)+input(row+1,col+1))/9;

                    output(row, col) = mean;
                    output(row, length-1-col) = mean;
                } else {
                    mean = (output(row-1,col-1)+output(row-1,col)+output(row-1,col+1)+
                            output(row,col-1) + output(row,col) + output(row,col+1)+
                            output(row+1,col-1)+output(row+1,col)+output(row+1,col+1))/9;

                    input(row, col) = mean;
                    input(row, length-1-col) = mean;
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

void quarter_switch(double *input, double *output, int threads, int length, int iterations)
{
    /*Split square in half vertically, and use symmetry to complete the other half.
    * Use switching.
    */
    double mean = 0.0;
    omp_set_num_threads(threads);
    for(int i = 0; i < iterations; i++)
    {
        #pragma omp parallel for shared(output, input) private(mean)
        for(size_t row = 1; row < length/2; row++)
        {
            for(size_t col = 1; col < length/2; col++)
            {
                if(row == col && row == length/2-1){
                    //the center square
                    continue;
                }
                if(i%2 == 0){
                    mean =  (input(row-1,col-1)+input(row-1,col)+input(row-1,col+1)+
                            input(row,col-1) + input(row,col) + input(row,col+1)+
                            input(row+1,col-1)+input(row+1,col)+input(row+1,col+1))/9;

                    output(row, col) = mean;
                    output(row, length-1-col) = mean;
                    output(length-1-row, col) = mean;
                    output(length-1-row, length-1-col) = mean;
                } else {
                    mean = (output(row-1,col-1)+output(row-1,col)+output(row-1,col+1)+
                            output(row,col-1) + output(row,col) + output(row,col+1)+
                            output(row+1,col-1)+output(row+1,col)+output(row+1,col+1))/9;

                    input(row, col) = mean;
                    input(row, length-1-col) = mean;
                    input(length-1-row, col) = mean;
                    input(length-1-row, length-1-col) = mean;
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

void half_horizontal_switch(double *input, double *output, int threads, int length, int iterations)
{
    /*Same as half_vertical_switch but split horizontally
    */
    double mean = 0.0;
    omp_set_num_threads(threads);
    for(int i = 0; i < iterations; i++)
    {
        #pragma omp parallel for shared(output, input) private(mean)
        for(size_t row = 1; row <= length/2-1; row++)
        {
            for(size_t col = 1; col < length-1; col++)
            {
                if(row == length/2-1 && (col == length/2 || col == length/2-1)){
                    continue;
                }
                if(i%2 == 0){
                    mean =  (input(row-1,col-1)+input(row-1,col)+input(row-1,col+1)+
                            input(row,col-1) + input(row,col) + input(row,col+1)+
                            input(row+1,col-1)+input(row+1,col)+input(row+1,col+1))/9;

                    output(row, col) = mean;
                    output(length-1-row, col) = mean;
                } else {
                    mean = (output(row-1,col-1)+output(row-1,col)+output(row-1,col+1)+
                            output(row,col-1) + output(row,col) + output(row,col+1)+
                            output(row+1,col-1)+output(row+1,col)+output(row+1,col+1))/9;

                    input(row, col) = mean;
                    input(length-1-row, col) = mean;
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

void whole_switch(double *input, double *output, int threads, int length, int iterations)
{
    /*Iterate through whole square but switch between
     +input -mean-> output
      and
     +output -mean-> input
    so no lost memory action*/
    // double mean = 0.0;
    omp_set_num_threads(threads);
    for(int i = 0; i < iterations; i++)
    {
        #pragma omp parallel for shared(output, input)
        for(size_t row = 1; row < length-1; row++)
        {
            for(size_t col = 1; col < length-1; col++)
            {
                if((row == length/2-1 && col == length/2-1) ||
                   (row == length/2-1 && col == length/2) ||
                   (row == length/2 && col == length/2-1) ||
                   (row == length/2 && col == length/2)){
                     //One of the center square
                    continue;
                }
                if(i%2 == 0){
                    output(row, col) = (input(row-1,col-1)+input(row-1,col)+input(row-1,col+1)+
                            input(row,col-1) + input(row,col) + input(row,col+1)+
                            input(row+1,col-1)+input(row+1,col)+input(row+1,col+1))/9;;
                } else {
                    input(row, col) = (output(row-1,col-1)+output(row-1,col)+output(row-1,col+1)+
                            output(row,col-1) + output(row,col) + output(row,col+1)+
                            output(row+1,col-1)+output(row+1,col)+output(row+1,col+1))/9;;
                }
            }//END OF COL ITERATION
        } //END OF PARALLEL FOR
    } //END OF ITERATION
    if(iterations%2 == 0){
        //if even number of iterations, need to ensure
        //'output' is the final placeholder
        memcpy(output, input, sizeof(double)*length*length);
    }
}

void whole_noSwitch(double *input, double *output, int threads, int length, int iterations)
{
    /*Iterate through whole square, and always read input and write to output. Copy results
    * from output to input at each iteration*/
    omp_set_num_threads(threads);
    for (size_t i = 0; i < iterations; i++)
    {
        #pragma omp parallel for
        for (size_t row = 1; row < length-1; row++)
        {
            for (size_t col = 1; col < length-1; col++)
            {
                if(
                    ((row == length/2 || row == length/2-1) && col == length/2) ||
                    ((row == length/2 || row == length/2-1) && col == length/2-1)){
                    continue;
                } else {
                    output(row, col) =(
                        input(row-1, col-1) +input(row-1, col) +input(row-1, col+1) +
                        input(row, col-1) +input(row, col) +input(row, col+1) +
                        input(row+1, col-1) +input(row+1, col) +input(row+1, col+1))/9;
                }
            } //END OF COL
        } //END OF PARALLEL ROW
        memcpy(input, output, sizeof(double)*length*length);
    } //END OF ITERATIONS

}
