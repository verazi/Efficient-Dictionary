#include <gtest/gtest.h>
#include <string>

extern "C" {
#include "driver/free_res.h"
#include "sort_unique_words.h"
}

TEST(SortUniqueWords, WordList) {
  char*** words = nullptr;
  words = (char***)malloc(sizeof(char**));
  if (words == nullptr) {
    ASSERT_TRUE(false) << "Memory allocation failed for words.";
    return;
  }
  uint32_t* count = nullptr;
  count = (uint32_t*)malloc(sizeof(uint32_t));
  if (count == nullptr) {
    ASSERT_TRUE(false) << "Memory allocation failed for count.";
    free(words);
    return;
  }
  *count = 0;

  char** p_doc = nullptr;
  p_doc = (char**)malloc(sizeof(char*));
  if (p_doc == nullptr) {
    ASSERT_TRUE(false) << "Memory allocation failed for p_doc.";
    free(count);
    free(words);
    return;
  }

  size_t doc_len = 53;
  *p_doc = (char*)malloc(doc_len * sizeof(char));
  if (*p_doc == nullptr) {
    ASSERT_TRUE(false) << "Memory allocation failed for p_doc.";
    free(count);
    free(words);
    free(p_doc);
    return;
  }

  char doc[] = "finding grapes@\0, a big cat <[``]> danced elegantly!a";
  memcpy(*p_doc, doc, doc_len);
  WordList(p_doc, doc_len, words, count);

  ASSERT_TRUE(*words != nullptr);
  ASSERT_EQ(*count, 7);

  char const* expected_words[] = {"a",         "big",     "cat",   "danced",
                                  "elegantly", "finding", "grapes"};
  for (int i = 0; i < 7; ++i) {
    ASSERT_TRUE(std::string(expected_words[i]) == std::string((*words)[i]));
  }

  free(*p_doc);
  free(p_doc);
  free(words);
  free(count);
}