#ifndef BASELINE_H
#define BASELINE_H
#include <stdint.h>

void SequentialSortUniqueWords(char*** words, uint32_t* count);
void WordListBaseLine(char** doc, uint32_t doc_len, char*** words,
                      uint32_t* count);

#endif  // BASELINE_H
