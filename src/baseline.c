#include <ctype.h>
#include <error_codes.h>
#include <sort_unique_words.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(char** a, char** b) {
  char* temp = *a;
  *a = *b;
  *b = temp;
}

int partition(char*** words, int left, int right) {
  char* pivot = (*words)[right];
  if (pivot == NULL) {
    swap(&(*words)[left], &(*words)[right]);
    return left;
  }

  int idx = left;
  for (int i = left; i < right; i++) {
    if ((*words)[i] == NULL) {
      // NULL is always bigger than any valid word
      swap(&(*words)[idx], &(*words)[i]);
      ++idx;
      continue;
    }
    int ret = strcmp((*words)[i], pivot);
    if (ret < 0) {
      swap(&(*words)[idx], &(*words)[i]);
      ++idx;
      continue;
    }
    if (ret == 0) {
      // mark duplicates as NULL
      (*words)[i] = NULL;
      // NULL is always bigger than any valid word
      swap(&(*words)[idx], &(*words)[i]);
      ++idx;
    }
  }

  swap(&(*words)[idx], &(*words)[right]);
  return idx;
}

void quicksort_recursive(char*** words, int left, int right) {
  if (left < right) {
    int partition_index = partition(words, left, right);
    quicksort_recursive(words, left, partition_index - 1);
    quicksort_recursive(words, partition_index + 1, right);
  }
}

void SequentialSortUniqueWords(char*** words, uint32_t* count) {
  if (*count <= 1) {
    return;
  }
  quicksort_recursive(words, 0, *count - 1);
  int j = 0;
  for (int i = 0; i < *count; i++) {
    if ((*words)[i] != NULL) {
      (*words)[j++] = (*words)[i];
    }
  }
  *count = j;
}

void WordListBaseLine(char** doc, uint32_t doc_len, char*** words,
                      uint32_t* count) {
  /**
   * sequential implementation of WordList
   * **/
  uint32_t max_words = doc_len / 2;  // minimum chars: a letter + '\0'
  *words = (char**)malloc(max_words * sizeof(char*));
  *count = 0;
  int is_prev_valid = 0;
  for (size_t i = 0; i < doc_len; ++i) {
    if (is_prev_valid == 0) {
      // prev char is not valid
      if (isalnum((*doc)[i]) != 0) {
        // curr char is a valid
        is_prev_valid = 1;
        // this is the start of the word
        (*words)[*count] = (*doc) + i;  // point to this word
        *count += 1;
      } else {
        // curr char is not valid, do nothing and continue
        continue;
      }
    } else {
      // prev char is valid
      if (isalnum((*doc)[i]) != 0) {
        // curr char is a valid, do nothing and continue
        continue;
      } else {
        // curr char is not valid
        is_prev_valid = 0;
        (*doc)[i] = '\0';  // terminate this word
      }
    }
  }
  SequentialSortUniqueWords(words, count);
}