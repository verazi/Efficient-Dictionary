#ifndef LOAD_FILE_H
#define LOAD_FILE_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

int LoadFile(char const* filename, char** doc, uint32_t* doc_len);

int SaveWords(char*** words, uint32_t count, char* filename, size_t* hash);

int SaveTime(struct timespec start_time, struct timespec end_time,
             int8_t max_threads, size_t hash, char* filename);

#endif  // LOAD_FILE_H
