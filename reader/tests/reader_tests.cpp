#include "../reader.h"
#include <gtest/gtest.h>

#include <vector>


TEST(Reader, ReadBinaryFile) {
  const std::vector<unsigned char> expected_data = {
    0xAA, 0xAA, 0xAA, 0xAA,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xCC, 0xCC, 0xCC, 0xCC
  };

  Reader reader("mock/test_1.bin");

  for (size_t i = 0; i < expected_data.size(); ++i) {
    ASSERT_TRUE(reader.has_next_byte());
    auto read_byte = reader.read_next_byte();
    ASSERT_EQ(read_byte, expected_data[i]);
  }
}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

