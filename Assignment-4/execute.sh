#!/bin/bash -l
#SBATCH --workdir /scratch/cloux/A4
#SBATCH --reservation cs307gpu
#SBATCH --time 01:00:00
#SBATCH --partition=gpu
#SBATCH --qos=gpu_free
#SBATCH --gres=gpu:1

length=1026
iterations=10000
# echo "Starting..."
module load gcc cuda
# echo "Done loading. Making"
make
# echo "Done making. Starting..."
./assignment4 $length $iterations
# echo "Completely done."
