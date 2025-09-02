#include "driver/load_file.h"
#include "error_codes.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int LoadFile(char const* filename, char** doc, uint32_t* doc_len) {
  if (strlen(filename) > MAX_FILENAME_LEN) {
    return ERR_FILENAME_LEN_EXCEED;
  }

  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    return ERR_OPEN_FILE;
  }

  // Get the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory for the buffer
  *doc = (uint8_t*)malloc(file_size);
  if (*doc == NULL) {
    fclose(file);
    return ERR_MALLOC;
  }

  // Read the file content to the buffer
  size_t bytes_read = fread(*doc, 1, file_size, file);
  if (bytes_read != (size_t)file_size) {
    free(*doc);
    fclose(file);
    return ERR_READ_FILE;
  }
  *doc_len = bytes_read;

  // Check for UTF-8 BOM
  if (*doc_len >= 3 && (*doc)[0] == 0xEF && (*doc)[1] == 0xBB &&
      (*doc)[2] == 0xBF) {
    *doc += 3;
    *doc_len -= 3;
  }

  fclose(file);
  return SUCCESS_CODE;
}

#define FNV_OFFSET_BASIS 2166136261u
#define FNV_PRIME 16777619u

size_t Hash(size_t prev_hash, char const* letter) {
  while (*letter) {
    prev_hash ^= *letter++;
    prev_hash *= FNV_PRIME;
  }
  return prev_hash % (UINT64_MAX / 2);
}

int SaveWords(char*** words, uint32_t count, char* filename, size_t* hash) {
  if (count == 0) {
    return SUCCESS_CODE;
  }
  char out_filename[FILENAME_MAX];
  snprintf(out_filename, FILENAME_MAX, "results/%s.words", filename);
  FILE* file = fopen(out_filename, "wb");
  if (file == NULL) {
    return ERR_SAVE_FILE_OPEN;
  }

  char lf = '\n';
  size_t max_len = 1024 * 1024;
  char* temp = (char*)malloc(sizeof(char) * max_len);
  if (temp == NULL) {
    exit(ERR_MALLOC);
  }
  size_t curr_len = 0;
  *hash = FNV_OFFSET_BASIS;
  for (size_t i = 0; i < count; i++) {
    size_t j = 0;
    while ((*words)[i][j] != '\0') {
      ++j;
    }
    if (curr_len + j > max_len) {
      fwrite(temp, sizeof(char), curr_len, file);
      curr_len = 0;
    }
    memcpy(temp + curr_len, &(*words)[i][0], sizeof(char) * j);
    temp[curr_len + j] = lf;
    curr_len += j + 1;
    *hash = Hash(*hash, &(*words)[i][0]);
  }
  fwrite(temp, sizeof(char), curr_len, file);
  fclose(file);
  free(temp);

  fprintf(stdout, "[-] Words saved: %s\n", out_filename);
  fprintf(stdout, "    Unique words count: %u\n", count);
  return SUCCESS_CODE;
}

float CalcTime(struct timespec start_time, struct timespec end_time) {
  long seconds = end_time.tv_sec - start_time.tv_sec;
  long nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
  if (nanoseconds < 0) {
    seconds -= 1;
    nanoseconds += 1000000000L;
  }
  return seconds + nanoseconds / 1000000000.0;
}

int SaveTime(struct timespec start_time, struct timespec end_time,
             int8_t max_threads, size_t hash, char* filename) {
  char out_filename[FILENAME_MAX];
  snprintf(out_filename, FILENAME_MAX, "results/%s.time", filename);
  FILE* file = fopen(out_filename, "w");
  if (file == NULL) {
    return ERR_SAVE_FILE_OPEN;
  }

  float wall_time_sec = CalcTime(start_time, end_time);

  char* header = "result_filename, result_hash, max_threads, wall_time\n";
  char line[2 * FILENAME_MAX];
  snprintf(line, sizeof(line), "%s, %zu, %d, %.6f\n", filename, hash,
           max_threads, wall_time_sec);
  fprintf(file, "%s", header);
  fprintf(file, "%s", line);
  fclose(file);

  fprintf(stdout, "[+] Wall time saved: %s\n", out_filename);
  fprintf(stdout, "    %s", header);
  fprintf(stdout, "    %s", line);

  return SUCCESS_CODE;
}