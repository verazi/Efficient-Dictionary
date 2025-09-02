#include "driver/free_res.h"
#include "driver/load_file.h"
#ifdef BASELINE
#include "baseline.h"
#else
#include "omp.h"
#include "sort_unique_words.h"
#endif
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <result_filename_prefix>\n", argv[0]);
    return 1;
  }

  struct timespec start_time;
  struct timespec end_time;
  int ret;

  // Load file
  char* doc = NULL;
  uint32_t doc_len = 0;
  fprintf(stdout, "[-] Loading file: %s\n", argv[1]);
  ret = LoadFile(argv[1], &doc, &doc_len);
  if (ret != 0) {
    fprintf(stderr, "[!] Loading file error: %d\n", ret);
    return ret;
  }

  // Find words
  char** words = NULL;
  uint32_t count = 0;
#ifdef BASELINE
  int8_t max_threads = 1;
  fprintf(stdout, "[+] Sequential baseline implementation.\n");
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  WordListBaseLine(&doc, doc_len, &words, &count);
#else
  int8_t max_threads = omp_get_max_threads();
  fprintf(stdout, "[+] Parallel implementation with maximum threads: %d\n",
          max_threads);
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  WordList(&doc, doc_len, &words, &count);
#endif
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  // Save words
  size_t hash = 0;
  ret = SaveWords(&words, count, argv[2], &hash);
  if (ret != 0) {
    fprintf(stderr, "[!] Save words error: %d\n", ret);
    return ret;
  }

  // Save time
  ret = SaveTime(start_time, end_time, max_threads, hash, argv[2]);
  if (ret != 0) {
    fprintf(stderr, "[!] Save time error: %d\n", ret);
    return ret;
  }

  free(doc);
  free(words);
  return 0;
}