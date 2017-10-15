#!/bin/bash
#SBATCH --workdir /scratch/cloux
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --reservation cs307cpu


echo STARTING AT `date`
for i in 1 2 4 8
do

	./pi $i 5000000000
	./integral $i  2000 5 9
done
echo END INTEGRAL at `date`
