#include <gtest/gtest.h>
#include <string>

extern "C" {
#include "baseline.h"
#include "driver/free_res.h"
}

TEST(Baseline, SequentialSortUniqueWords) {
  char** words = (char**)malloc(5 * sizeof(char*));
  if (words == nullptr) {
    ASSERT_TRUE(false) << "Memory allocation failed for words.";
    return;
  }
  words[0] = (char*)"banana";
  words[1] = (char*)"apple";
  words[2] = (char*)"apple";
  words[3] = (char*)"banana";
  words[4] = (char*)"cherry";
  uint32_t count = 5;

  SequentialSortUniqueWords(&words, &count);

  ASSERT_EQ(count, 3);
  ASSERT_STREQ(words[0], "apple");
  ASSERT_STREQ(words[1], "banana");
  ASSERT_STREQ(words[2], "cherry");

  free(words);
}

TEST(Baseline, WordListBaseLine) {
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
    ASSERT_TRUE(false) << "Memory allocation failed for *p_doc.";
    free(count);
    free(words);
    free(p_doc);
    return;
  }

  char doc[] = "finding grapes@\0, a big cat <[``]> danced elegantly!a";
  memcpy(*p_doc, doc, doc_len);
  WordListBaseLine(p_doc, doc_len, words, count);

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
