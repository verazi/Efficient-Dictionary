#!/usr/bin/bash

#SBATCH --job-name=sequential-baseline
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --account=punim0520
#SBATCH --partition=sapphire
#SBATCH --output=logs/sequential-baseline.%j.out
#SBATCH --error=logs/sequential-baseline.%j.err

#### You only need to edit the above redacted lines to use this slurm file ####
#### However, feel free to use your own slurm file if you prefer ##############

cd "$SLURM_SUBMIT_DIR"

module purge
module load GCC/11.3.0

make clean
make run_baseline CC=gcc CXX=g++

echo "=========================="
if [ -z "$1" ]; then
    echo -e "\nUsage: sbatch $0 <test_filename>\n"
    file="files/text1-ASCII.txt"
    echo "[*] Using default file: $file"
else
    file=$1
    echo "[*] Using provided file: $file"
fi
echo "---------------------------"

file_basename=$(basename "$file")
res_path="results"

if [ ! -d "$res_path" ]; then
    mkdir "$res_path"
fi

out_arg="$file_basename.baseline.1"
out_file="$res_path/$out_arg.time"

stdout=$(OMP_NUM_THREADS=1 /usr/bin/time -v ./build/run_baseline $file $out_arg 2>&1)
echo -e "$stdout"
peak_mem=$( echo "$stdout" | grep "Maximum resident set size" | awk '{print $NF}' )

header=$(head -n 1 $out_file | tr -d '\n' )
content=$(tail -n 1 $out_file | tr -d '\n' )

header="$header, peak_memory, partition\n"
content="$content, $peak_mem, sapphire"

out_content="$header$content"

final_results="$res_path/experiment.sequential.baseline.$file_basename.csv"
echo -e "$out_content" > "$final_results"

echo -e "\n[*] Experiment completed. Results saved in $final_results\n"
echo -e "$out_content"
echo "=========================="
