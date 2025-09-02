# COMP90025 Code Skeleton for Project 1

This is the skeleton code, which includes a `driver` to load a text file. We hope this skeleton can help you focus on the implementation of `src/sort_unique_words.c:WordList`. Feel free to add any files, functions, or tests; but do not alter the skeleton.

**NOTE**: You are not allowed to use any C++ container class libraries, such as `std::vector`, `std::list`, `std::map`, `std::qsort`, `std::sort`, etc.

## Build

```bash
$ make clean && make run
rm -rf build/*
gcc -Iinclude -O3 -fopenmp -lm -c src/driver/free_res.c -o build/driver/free_res.o
gcc -Iinclude -O3 -fopenmp -lm -c src/driver/load_file.c -o build/driver/load_file.o
gcc -Iinclude -O3 -fopenmp -lm -c src/sort_unique_words.c -o build/sort_unique_words.o
gcc -Iinclude -O3 -fopenmp -lm run_sort_unique_words.c build/driver/free_res.o build/driver/load_file.o build/sort_unique_words.o -lpthread -lstdc++ -fopenmp -lm -o build/run
```

### Build with debug

```bash
$ make clean && make run DEBUG=1
```

## Usage

```bash
$ ./build/run 
Usage: ./build/run <filename> <result_filename_prefix>
$ ./build/run files/simple.txt simple.txt.v1
[-] Loading file: files/simple.txt
[+] Parallel implementation with maximum threads: 8
[-] Words saved: results/simple.txt.v1.words
    Unique words count: 7
[+] Wall time saved: results/simple.txt.v1.time
    result_filename, result_hash, max_threads, wall_time
    simple.txt.v1, 7942845722386907539, 8, 0.000000
$ hexdump -C results/simple.txt.v1.words
00000000  61 0a 62 69 67 0a 63 61  74 0a 64 61 6e 63 65 64  |a.big.cat.danced|
00000010  0a 65 6c 65 67 61 6e 74  6c 79 0a 66 69 6e 64 69  |.elegantly.findi|
00000020  6e 67 0a 67 72 61 70 65  73 0a                    |ng.grapes.|
0000002a
$ hexdump -C files/simple.txt
00000000  66 69 6e 64 69 6e 67 20  67 72 61 70 65 73 40 00  |finding grapes@.|
00000010  2c 20 61 20 62 69 67 20  63 61 74 20 3c 5b 60 60  |, a big cat <[``|
00000020  5d 3e 20 64 61 6e 63 65  64 20 65 6c 65 67 61 6e  |]> danced elegan|
00000030  74 6c 79 21 61                                    |tly!a|
00000035
```

## Test
```bash
$ make clean && make test
rm -rf build/*
gcc-11 -std=c++11 -Iinclude -Igtest/include -Igtest -O3 -fopenmp -lm -c gtest/src/gtest-all.cc -o build/gtest/gtest-all.o
gcc-11 -Iinclude -O3 -fopenmp -lm -c src/driver/load_file.c -o build/driver/load_file.o
gcc-11 -Iinclude -O3 -fopenmp -lm -c src/sort_unique_words.c -o build/sort_unique_words.o
gcc-11 -Iinclude -O3 -fopenmp -lm -c src/baseline.c -o build/baseline.o
gcc-11 -std=c++11 -Iinclude -Igtest/include -Igtest -O3 -fopenmp -lm test/test_main.cc build/gtest/gtest-all.o build/driver/load_file.o build/sort_unique_words.o build/baseline.o -lpthread -lstdc++ -fopenmp -lm -o build/test
[==========] Running 3 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 2 tests from Baseline
[ RUN      ] Baseline.SequentialSortUniqueWords
[       OK ] Baseline.SequentialSortUniqueWords (0 ms)
[ RUN      ] Baseline.WordListBaseLine
[       OK ] Baseline.WordListBaseLine (0 ms)
[----------] 2 tests from Baseline (0 ms total)

[----------] 1 test from SortUniqueWords
[ RUN      ] SortUniqueWords.WordList
[       OK ] SortUniqueWords.WordList (0 ms)
[----------] 1 test from SortUniqueWords (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 2 test suites ran. (0 ms total)
[  PASSED  ] 3 tests.
```
## Sequential Baseline
This is a very simple sequential baseline implementation. You can use it in your report but you have the choice to implement your own better sequential implementation.
### Usage
```
$ make clean && make run_baseline && ./build/run_baseline files/simple.txt simple
rm -rf build/*
gcc-11 -Iinclude -O3 -fopenmp -lm -c src/driver/load_file.c -o build/driver/load_file.o
gcc-11 -Iinclude -O3 -fopenmp -lm -c src/sort_unique_words.c -o build/sort_unique_words.o
gcc-11 -Iinclude -O3 -fopenmp -lm -c src/baseline.c -o build/baseline.o
gcc-11 -Iinclude -O3 -fopenmp -lm run_sort_unique_words.c build/driver/load_file.o build/sort_unique_words.o build/baseline.o -lpthread -lstdc++ -fopenmp -lm -o build/run_baseline -DBASELINE
[-] Loading file: files/simple.txt
[+] Sequential baseline implementation.
[-] Words saved: results/simple.words
    Unique words count: 7
[+] Wall time saved: results/simple.time
    result_filename, result_hash, max_threads, wall_time
    simple, 7942845722386907539, 1, 0.000002
$ hexdump -C results/simple.baseline.words 
00000000  61 0a 62 69 67 0a 63 61  74 0a 64 61 6e 63 65 64  |a.big.cat.danced|
00000010  0a 65 6c 65 67 61 6e 74  6c 79 0a 66 69 6e 64 69  |.elegantly.findi|
00000020  6e 67 0a 67 72 61 70 65  73 0a                    |ng.grapes.|
0000002a
$ ./build/run_baseline files/text1-ASCII.txt sequential.baseline.text1-ASCII.txt 
[-] Loading file: files/text1-ASCII.txt
[+] Sequential baseline implementation.
[-] Words saved: results/sequential.baseline.text1-ASCII.txt.words
    Unique words count: 10997
[+] Wall time saved: results/sequential.baseline.text1-ASCII.txt.time
    result_filename, result_hash, max_threads, wall_time
    sequential.baseline.text1-ASCII.txt, 1909418373055651679, 1, 0.015758
```
## Release

- **Version: 1.0.0**
    - Date: Friday, Aug 15, 2025, 5:05 PM (UTC+10)
    - Notes:
        - Example cases
        - Sequential baseline

## Tips

- [ ] Optimize the  idea with your theory as much as possible, then start parallelizing. This will save you a lot of time.
- [ ] Commit frequently.
- [ ] Make sure your code can run on Spartan.
- [ ] Do not waste time too much on implementing anything similar to the container classes. If you have this idea, you may be on the wrong track.

### Spartan
#### TODO
- [ ] complete `sequential-baseline.slurm`
- [ ] complete `sort-unique-words-test.slurm`
- [ ] complete `sort-unique-words-experiment.slurm`
#### Usage
```
$ sbatch sort-unique-words-test.slurm
```
Use `watch squeue --me` to see the status of your submitted task. If it is running, use `tail -f sort-unique-words-test.out` to see realtime output or `less sort-unique-words-test.err` for errors.

If you completed todos above, you should be able to use them collecting results easier as below:
```
$ sbatch sort-unique-words-experiment.slurm files/text1-ASCII.txt 4
sbatch: You are submitting jobs under your home directory.
sbatch: We recommend to submit jobs under your project directory or scratch directory.
Submitted batch job 14216628
$ echo "use `watch squeue --me` to check job status"
$ cat sort-unique-words-experiment.out 
██████████████ <- redacted
██████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████
████████████████████████████████████████████████████████████████████
███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
==========================
[*] Using provided file: files/text1-ASCII.txt
[*] Using provided threads: 4
---------------------------
[-] Loading file: files/text1-ASCII.txt
[+] Parallel implementation with maximum threads: 1
[-] Words saved: results/text1-ASCII.txt.parallel.1.words
    Unique words count: 7
[+] Wall time saved: results/text1-ASCII.txt.parallel.1.time
    result_filename, result_hash, max_threads, wall_time
    text1-ASCII.txt.parallel.1, 7847069655474528065, 1, 0.000000
        Command being timed: "./build/run files/text1-ASCII.txt text1-ASCII.txt.parallel.1"
        User time (seconds): 0.00
        System time (seconds): 0.00
        Percent of CPU this job got: 20%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.01
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 4096
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 346
        Voluntary context switches: 32
        Involuntary context switches: 0
        Swaps: 0
        File system inputs: 1584
        File system outputs: 16
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
---------------------------
[-] Loading file: files/text1-ASCII.txt
[+] Parallel implementation with maximum threads: 2
[-] Words saved: results/text1-ASCII.txt.parallel.2.words
    Unique words count: 7
[+] Wall time saved: results/text1-ASCII.txt.parallel.2.time
    result_filename, result_hash, max_threads, wall_time
    text1-ASCII.txt.parallel.2, 7847069655474528065, 2, 0.000000
        Command being timed: "./build/run files/text1-ASCII.txt text1-ASCII.txt.parallel.2"
        User time (seconds): 0.00
        System time (seconds): 0.00
        Percent of CPU this job got: 20%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 4096
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 347
        Voluntary context switches: 14
        Involuntary context switches: 0
        Swaps: 0
        File system inputs: 0
        File system outputs: 16
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
---------------------------
[-] Loading file: files/text1-ASCII.txt
[+] Parallel implementation with maximum threads: 3
[-] Words saved: results/text1-ASCII.txt.parallel.3.words
    Unique words count: 7
[+] Wall time saved: results/text1-ASCII.txt.parallel.3.time
    result_filename, result_hash, max_threads, wall_time
    text1-ASCII.txt.parallel.3, 7847069655474528065, 3, 0.000000
        Command being timed: "./build/run files/text1-ASCII.txt text1-ASCII.txt.parallel.3"
        User time (seconds): 0.00
        System time (seconds): 0.00
        Percent of CPU this job got: 20%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 4096
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 347
        Voluntary context switches: 13
        Involuntary context switches: 0
        Swaps: 0
        File system inputs: 0
        File system outputs: 16
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
---------------------------
[-] Loading file: files/text1-ASCII.txt
[+] Parallel implementation with maximum threads: 4
[-] Words saved: results/text1-ASCII.txt.parallel.4.words
    Unique words count: 7
[+] Wall time saved: results/text1-ASCII.txt.parallel.4.time
    result_filename, result_hash, max_threads, wall_time
    text1-ASCII.txt.parallel.4, 7847069655474528065, 4, 0.000000
        Command being timed: "./build/run files/text1-ASCII.txt text1-ASCII.txt.parallel.4"
        User time (seconds): 0.00
        System time (seconds): 0.00
        Percent of CPU this job got: 25%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 4096
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 347
        Voluntary context switches: 14
        Involuntary context switches: 0
        Swaps: 0
        File system inputs: 0
        File system outputs: 16
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
---------------------------

[*] Experiment completed. Results saved in results/experiment.parallel.4threads.text1-ASCII.txt.csv

result_filename, result_hash, max_threads, wall_time, peak_memory, partition
text1-ASCII.txt.parallel.1, 7847069655474528065, 1, 0.000000, 4096, sapphire
text1-ASCII.txt.parallel.2, 7847069655474528065, 2, 0.000000, 4096, sapphire
text1-ASCII.txt.parallel.3, 7847069655474528065, 3, 0.000000, 4096, sapphire
text1-ASCII.txt.parallel.4, 7847069655474528065, 4, 0.000000, 4096, sapphire

==========================
```

### Disclaimer

These tips come from one teaching staff, but they may not work for you, as everyone has their own programming skills, thought processes, and working styles.

Happy coding and good luck! 🚀
