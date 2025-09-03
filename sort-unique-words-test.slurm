#!/usr/bin/bash

#SBATCH --job-name=sort-unique-words-test
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --account=punim0520
#SBATCH --partition=sapphire
#SBATCH --output=logs/sort-unique-words-test.%j.out
#SBATCH --error=logs/sort-unique-words-test.%j.err

#### You only need to edit the above redacted lines to use this slurm file ####
#### However, feel free to use your own slurm file if you prefer ##############

cd "$SLURM_SUBMIT_DIR"

module purge
module load GCC/11.3.0

make clean
make run CC=gcc CXX=g++

OMP_NUM_THREADS=2 ./build/run files/text1-ASCII.txt text1-ASCII.txt.test.01
