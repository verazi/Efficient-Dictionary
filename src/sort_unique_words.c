#include <error_codes.h>
#include <sort_unique_words.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for isalnum

// Insertion sort implementation
static void insertion_sort(char **a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        char *key = a[i];
        size_t j = i;
        while (j > 0 && strcmp(a[j-1], key) > 0) {
            a[j] = a[j-1];
            --j;
        }
        a[j] = key;
    }
}

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

  // (a) take a pointer doc to a nul-terminated ASCII string and its length, doc len;
  if (words) *words = NULL;
  if (count) *count = 0;
      
  if (*doc == NULL || doc_len == 0) {
    return;
  }

  // (b) parse the string into a list of words, where a word is defined as a
  // maximal string of characters that are ASCII letter type, as determined by isalnum.
  // That is, it is a string of ASCII letters that
  //    • is either preceded by a non-letter or starts at the start of the string; and
  //    • is either followed by a non-letter or ends at the end of the string;
  // The function should malloc two regions of memory:
  //    one containing all of the ('\0'-terminated) words contiguously, and
  //    the other containing a list of pointers to the starts of words within the first region.
  // A pointer to the second should be returned in words.
  // The memory should be able to be freed by free (*words); The original string doc may be modified.

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

  // (c) set *count to the number of distinct words.
  // Sort the words using insertion sort
  insertion_sort(word_array, word_count);

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


  // Input: *doc = "finding grapes@\0, a big cat <[``]> danced elegantly!a"
  // There are 7 unique words in the doc
  // *count = 7;
  // *words = (char**)malloc((*count) * sizeof(char*));
  // if (*words == NULL) {
  //   fprintf(stderr, "Memory allocation failed for words pointers.\n");
  //   exit(ERR_SAFE_ALLOC_MEM);
  // }

  // doc: "finding grapes@\0, a big cat <[``]> danced elegantly!a"
  // -> : "finding\0grapes\0\0, a\0big\0cat\0<[``]>\0danced\0elegantly\0a";
  // (*doc)[7] = '\0';
  // (*doc)[14] = '\0';
  // (*doc)[19] = '\0';
  // (*doc)[23] = '\0';
  // (*doc)[27] = '\0';
  // (*doc)[41] = '\0';
  // (*doc)[51] = '\0';
  // (*words)[0] = *doc + 18;  // point to first word `a`
  // (*words)[1] = *doc + 20;  // point to second word `big`
  // (*words)[2] = *doc + 24;  // point to third word `cat`
  // (*words)[3] = *doc + 35;  // point to fourth word `danced`
  // (*words)[4] = *doc + 42;  // point to fifth word `elegantly`
  // (*words)[5] = *doc + 0;   // point to sixth word `finding`
  // (*words)[6] = *doc + 8;   // point to seventh word `grapes`
  //  *  doc: "finding\0grapes\0\0, a\0big\0cat\0<[``]> danced\0elegantly\0a"
  //  *         \        \         /  /    /           /       /
  //  *            \        \     /  /    /           /       /
  //  *              \        \  /  /    /           /       /
  //  *                \       \/  /    /           /       /
  //  *                  \     / \/    /           /       /
  //  *                     \ /  / \  /           /       /
  //  *                      /\ /   \/           /       /
  //  *                     /  /\   /  \        /       /
  //  *                    /  /  \ /     \     /       /
  //  *                   /  /    /\       \  /       /
  //  *                  /  /    /   \      \/       /
  //  *                 /  /    /      \    / \     /
  //  *                /  /    /         \ /    \  /
  //  *               /  /    /           /\     \/
  //  *              /  /    /           /  \    / \
  //  *             /  /    /           /     \ /   \
  //  *            /  /    /           /       /\     \
  //  *           /  /    /           /       /  \     \
  //   words:    [0][1]  [2]         [3]     [4] [5]   [6]
  //  *           a  big  cat         danced  elegantly finding grapes

  // Note that *words will be freed outside this function.
}