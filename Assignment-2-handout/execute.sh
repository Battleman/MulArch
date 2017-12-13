#!/bin/bash
#SBATCH --workdir /scratch/cloux/A2
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --reservation cs307cpu

threads=8
length=1100
iterations=10000
output="output.csv"

./assignment2 $threads $length $iterations $output
