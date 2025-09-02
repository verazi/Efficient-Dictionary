#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sort_unique_words.h>

static char* slurp(const char* path, uint32_t* len) {
    FILE* f = fopen(path, "rb");
    if (!f) { perror("fopen"); exit(1); }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)n + 1);
    if (!buf) { perror("malloc"); exit(1); }
    size_t r = fread(buf, 1, (size_t)n, f);
    fclose(f);
    if (r != (size_t)n) { fprintf(stderr, "short read\n"); exit(1); }
    buf[n] = '\0';
    *len = (uint32_t)(n + 1);
    return buf;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <input.txt>\n", argv[0]); return 2; }

    uint32_t len = 0;
    char* doc = slurp(argv[1], &len);

    char** words = NULL;
    uint32_t count = 0;
    WordList(&doc, len, &words, &count);

    for (uint32_t i = 1; i < count; ++i) {
        if (!(strcmp(words[i-1], words[i]) < 0)) {
            fprintf(stderr, "[ERR] not strictly sorted at %u: '%s' vs '%s'\n",
                    i, words[i-1], words[i]);
            free(words); free(doc);
            return 3;
        }
    }

    for (uint32_t i = 0; i < count; ++i) puts(words[i]);

    free(words);
    free(doc);
    return 0;
}