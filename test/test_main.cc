#include "src/test_baseline.cc"
#include "src/test_sort_unique_words.cc"
#include <gtest/gtest.h>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}