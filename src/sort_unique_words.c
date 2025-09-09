#include <error_codes.h>
#include <sort_unique_words.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // isalnum
#include <omp.h>
#include <assert.h>
#include <math.h>

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
 * Sequential Experiment Result: 0.30s
 * Parallel Experiment Result:
 *    (1) Sequential partition: 0.15s
 *    (2) Parallel partition
 */


// Basic quick sort
static void swap_words(char** a, char** b) {
  char* temp = *a;
  *a = *b;
  *b = temp;
}
static int partition_words(char** words, int left, int right) {
  char* pivot = words[right];
  int idx = left;
  for (int i = left; i < right; i++) {
    int ret = strcmp(words[i], pivot);
    if (ret < 0) {
      swap_words(&words[idx], &words[i]);
      ++idx;
      continue;
    }
  }
  swap_words(&words[idx], &words[right]);
  return idx;
}
static void quick_sort_recursive(char **words, int left, int right) {
  if (left < right) {
    int partition_index = partition_words(words, left, right);
    quick_sort_recursive(words, left, partition_index - 1);
    quick_sort_recursive(words, partition_index + 1, right);
  }
}
static void quick_sort_words(char** array, size_t count) {
  if (count <= 1) return;
  quick_sort_recursive(array, 0, count - 1);
}

// Parallel quick sort
// with sequential partition
static int seq_pivot_select(char** words, int left, int right) {
  int mid = left + ((right - left) / 2);

  if (strcmp(words[left], words[mid]) > 0) {
    swap_words(&words[left], &words[mid]);
  }
  if (strcmp(words[left], words[right]) > 0) {
    swap_words(&words[left], &words[right]);
  }
  if (strcmp(words[mid], words[right]) > 0) {
    swap_words(&words[mid], &words[right]);
  }

  swap_words(&words[mid], &words[right - 1]);
  char* pivot = words[right - 1];
  
  int idx = left;
  for (int i = left; i < right - 1; i++) {
    int ret = strcmp(words[i], pivot);
    if (ret < 0) {
      swap_words(&words[idx], &words[i]);
      ++idx;
      continue;
    }
  }
  swap_words(&words[idx], &words[right - 1]);
  return idx;
}
static void quick_sort_parallel(char** array, int left, int right, int threshold) {
  if (left < right) {
    if (right - left < threshold) {
      quick_sort_recursive(array, left, right);
    } else { // n + n/2 + n/4 + ... = 2n => O(n)
      int partition_index = seq_pivot_select(array, left, right);
      #pragma omp task shared(array)
      quick_sort_parallel(array, left, partition_index - 1, threshold);
      #pragma omp task shared(array)
      quick_sort_parallel(array, partition_index + 1, right, threshold);
      #pragma omp taskwait
    }
  }
}
static void quick_sort_words_parallel(char** array, size_t count) {
  if (count <= 1) return;
  int threshold = 5000;
  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      quick_sort_parallel(array, 0, count - 1, threshold);
    }
  }
}

// Parallel partitioning
static char* par_pivot_select(char** words, int left, int right) {
  int mid = left + ((right - left) >> 1);

  char* left_val = words[left];
  char* mid_val = words[mid];
  char* right_val = words[right - 1];

  if ((strcmp(left_val, mid_val) <= 0 && strcmp(mid_val, right_val) <= 0) ||
      (strcmp(right_val, mid_val) <= 0 && strcmp(mid_val, left_val) <= 0))
    return mid_val;
  if ((strcmp(mid_val, right_val) <= 0 && strcmp(right_val, left_val) <= 0) ||
      (strcmp(left_val, right_val) <= 0 && strcmp(right_val, mid_val) <= 0))
    return right_val;
  return left_val;
}
// 2. Partition
static void par_sort_partition(char** words, int left, int right, int* m1, int* m2) {
  int n = right - left;
  if (n <= 1) {
    *m1 = left;
    *m2 = right;
    return;
  }

  const char* pivot = par_pivot_select(words, left, right);

  int p = omp_get_max_threads();
  if (p > n) p = n;
  int const ws = (n + p - 1) / p;

  // Dijkstra Partitioning
  int* nL = (int*)calloc((size_t)p, sizeof(int));
  int* nE = (int*)calloc((size_t)p, sizeof(int));
  int* nG = (int*)calloc((size_t)p, sizeof(int));

  #pragma omp parallel for schedule(static)
  for (int id = 0; id < p; ++id) {
    int b = left + (id * ws);
    int e = left + ((id + 1) * ws);
    if (e > right) e = right;
    if (b >= right) {
      nL[id] = nE[id] = nG[id] = 0;
      continue;
    }

    int l = 0;
    int eq = 0;
    int g = 0;
    for (int i = b; i < e; ++i) {
      int cmp = strcmp(words[i], pivot);
      if (cmp < 0) {
        ++l;
      } else if (cmp == 0) {
        ++eq;
      } else {
        ++g;
      }
    }
    nL[id] = l;
    nE[id] = eq;
    nG[id] = g;
  }

  int *psL=(int*)malloc((size_t)p*sizeof(int));
  int *psE=(int*)malloc((size_t)p*sizeof(int));
  int *psG=(int*)malloc((size_t)p*sizeof(int));

  int totL = 0;
  int totE = 0;
  int totG = 0;
  for (int i = 0; i < p; ++i) {
    psL[i] = totL;
    totL += nL[i];
    }
  for (int i=0;i<p;++i) {
    psE[i] = totE;
    totE += nE[i];
  }
  for (int i = 0; i < p; ++i) {
    psG[i] = totG;
    totG += nG[i];
  }

  const int baseL = 0;
  const int baseE = totL;
  const int baseG = totL + totE;

  char** temp = (char**)malloc(sizeof(char*) * (size_t)n);

  #pragma omp parallel for schedule(static)
  for (int id = 0; id < p; ++id) {
    int b = left + (id * ws);
    int e = left + ((id + 1) * ws);
    if (e > right) e = right;

    int wL = psL[id];
    int wE = psE[id];
    int wG = psG[id];

    for (int i = b; i < e; ++i) {
      int cmp = strcmp(words[i], pivot);
      if (cmp < 0) {
        temp[baseL + wL++] = words[i];
      } else if (cmp == 0) {
        temp[baseE + wE++] = words[i];
      } else {
        temp[baseG + wG++] = words[i];
      }
    }
  }

  for (int k = 0; k < n; ++k) {
    words[left + k] = temp[k];
  }

  free(temp);
  free(nL); free(nE); free(nG);
  free(psL); free(psE); free(psG);

  *m1 = left + totL;
  *m2 = left + totL + totE;
}
// 3. Recursion
static void quicksort_parallel_recursive(char*** words, int left, int right, int threshold) {
  int n = right - left;
  if (n <= 1) return;
  
  if (n <= threshold) {
    quick_sort_recursive(*words, left, right - 1);
    return;
  }

  int partition_index_1;
  int partition_index_2;
  par_sort_partition(*words, left, right, &partition_index_1, &partition_index_2);

  #pragma omp task shared(words)
  quicksort_parallel_recursive(words, left, partition_index_1, threshold);
  #pragma omp task shared(words)
  quicksort_parallel_recursive(words, partition_index_2, right, threshold);
  #pragma omp taskwait
}
static void quicksort_parallel(char*** words, int n) {
  if (n <= 1) return;
  int threshold = 3000;
  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      quicksort_parallel_recursive(words, 0, n, threshold);
    }
  }
}

// Main function
void WordList(char** doc, uint32_t doc_len, char*** words, uint32_t* count) {
  if (words) *words = NULL;
  if (count) *count = 0;
  if (*doc == NULL || doc_len == 0) {
    return;
  }

  // Tokenize
  double t0 = omp_get_wtime();

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

  // Sort
  double t1 = omp_get_wtime();

  // insertion_sort(word_array, word_count);
  // quick_sort_words(word_array, word_count);
  // quick_sort_words_parallel(word_array, word_count);
  quicksort_parallel(&word_array, word_count);

  // Unique
  double t2 = omp_get_wtime();

  uint32_t unique_count = 1;
  for (uint32_t i = 1; i < word_count; i++) {
    if (strcmp(word_array[i], word_array[i - 1]) != 0) {
      word_array[unique_count++] = word_array[i];
    }
  }

  // Pack
  double t3 = omp_get_wtime();

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

  double t4 = omp_get_wtime();

  fprintf(stderr,
          "timing: tokenize=%.6f sort=%.6f unique=%.6f pack=%.6f total=%.6f\n",
          t1 - t0, t2 - t1, t3 - t2, t4 - t3, t4 - t0);
  
  *words = out;
  *count = unique_count;
}