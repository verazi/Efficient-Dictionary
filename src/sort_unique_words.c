#include <error_codes.h>
#include <sort_unique_words.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // for isalnum
#include <omp.h>

// baseline 0.019s

/* Insertion Sort
 * O(n^2) time complexity -- 18s
 */
static void insertion_sort(char **array, size_t count) {
    for (size_t i = 1; i < count; ++i) {
        char *key = array[i];
        size_t j = i;
        while (j > 0 && strcmp(array[j-1], key) > 0) {
            array[j] = array[j-1];
            --j;
        }
        array[j] = key;
    }
}

/* Quick Sort
 * Average O(n log n) time complexity
 * Sequential Experiment Result: 0.3s
 * Parallel Experiment Result: 0.15s
 */

// TODO(tzuyu): pivot -- mdedian of three
// TODO(tzuyu): insertion sort for small arrays

// Sequential quick sort
static void swap_a_b(char** a, char** b) {
  char* temp = *a;
  *a = *b;
  *b = temp;
}
static int partition_left_right(char** words, int left, int right) {
  char* pivot = words[right];

  int idx = left;
  for (int i = left; i < right; i++) {
    int ret = strcmp(words[i], pivot);
    if (ret < 0) {
      swap_a_b(&words[idx], &words[i]);
      ++idx;
      continue;
    }
  }

  swap_a_b(&words[idx], &words[right]);
  return idx;
}
// quick sort
static void quick_sort(char **words, int left, int right) {
  if (left < right) {
    int partition_index = partition_left_right(words, left, right);
    quick_sort(words, left, partition_index - 1);
    quick_sort(words, partition_index + 1, right);
  }
}
// quick sort word array
static void quick_sort_words(char** array, size_t count) {
  if (count <= 1) return;
  quick_sort(array, 0, count - 1);
}

// quick sort in parallel
static void parallel_quick_sort(char** array, int left, int right, int threadhold) {
  if (left < right) {
    if (right - left < threadhold) {
      quick_sort(array, left, right);
    } else {
      int partition_index = partition_left_right(array, left, right);
      #pragma omp task shared(array)
      parallel_quick_sort(array, left, partition_index - 1, threadhold);
      #pragma omp task shared(array)
      parallel_quick_sort(array, partition_index + 1, right, threadhold);
      #pragma omp taskwait
    }
  }
}
static void parallel_quick_sort_words(char** array, size_t count) {
  if (count <= 1) return;
  int threadhold = 50000; // sequential threshold (consider dynamically adjust based on count number)
  // Start()
  #pragma omp parallel  // ThreadPoolInit(pool)
  {
    #pragma omp single nowait
    {
      parallel_quick_sort(array, 0, count - 1, threadhold);
    }
  }
}

//TODO(tzuyu): try merge sort in parallel

// TODO(tzuyu): [prefix sum] remove duplicates in parallel
// TODO(tzuyu): [prefix sum] malloc memory
void WordList(char** doc, uint32_t doc_len, char*** words, uint32_t* count) {
  /**
   * Version: 1.0.0
   * Date: Friday 15 Aug 2025 UTC+10
   * - Always use the latest skeleton code
   * - Keep an eye on announcements for updates and download the latest version
   * from Ed: https://edstem.org/au/courses/25948/discussion/2845998
   * **/

  //////////////////////////////////////////////////////////////
  // Implement your code below.
  // Feel free to remove the dummy implementation below.
  //////////////////////////////////////////////////////////////

  if (words) *words = NULL;
  if (count) *count = 0;
      
  if (*doc == NULL || doc_len == 0) {
    return;
  }

  size_t word_count = 0;
  int in_word = 0;

  for (uint32_t i = 0; i < doc_len; i++) { // memory allocation
    int is_letter = isalnum((unsigned char)(*doc)[i]) != 0;
    if (is_letter && !in_word) { // Start of a word
      word_count++;
      in_word = 1;
    } else if (!is_letter && in_word) { // End of a word
      (*doc)[i] = '\0';
      in_word = 0;
    }
  }

  if (word_count == 0) {
    return;
  }

  char** word_array = (char**)malloc(word_count * sizeof(char*));
  if (!word_array) {
    fprintf(stderr, "Memory allocation failed for words pointers.\n");
    exit(ERR_SAFE_ALLOC_MEM);
  }

  size_t word_start = 0;
  in_word = 0;
  for (uint32_t i = 0; i < doc_len; i++) {
    int is_letter = isalnum((unsigned char)(*doc)[i]) != 0;
    if (is_letter && !in_word) { // Start of a word
      word_array[word_start++] = &(*doc)[i];
      in_word = 1;
    } else if (!is_letter && in_word) { // End of a word
      in_word = 0;
    }
  }

  // insertion_sort(word_array, word_count);
  // quick_sort_words(word_array, word_count);
  parallel_quick_sort_words(word_array, word_count);

  // Remove duplicates
  uint32_t unique_count = 1;  // At least one word exists
  for (uint32_t i = 1; i < word_count; i++) {
    if (strcmp(word_array[i], word_array[i - 1]) != 0) {
      word_array[unique_count++] = word_array[i];
    }
  }

  // malloc
  size_t ptr_size = unique_count * sizeof(char*);
  size_t str_size = 0;
  for (uint32_t i = 0; i < unique_count; i++) {
    str_size += strlen(word_array[i]) + 1; // +1 for '\0'
  }

  char* str_block = (char*)malloc(ptr_size + str_size);
  if (!str_block) {
    free(word_array);
    fprintf(stderr, "Memory allocation failed for words strings.\n");
    exit(ERR_SAFE_ALLOC_MEM);
  }

  char** out = (char**)str_block;
  char* pool = str_block + ptr_size;
  for(uint32_t i = 0; i < unique_count; i++) {
    size_t len = strlen(word_array[i]) + 1;
    memcpy(pool, word_array[i], len);
    out[i] = pool;
    pool += len;
  }

  free(word_array);
  *words = out;
  *count = unique_count;
}