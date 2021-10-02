#include "../reader.h"
#include <gtest/gtest.h>

#include <vector>

TEST(Reader, ReadBinaryFile1) {
    const std::vector<unsigned char> expected_data = {0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB,
                                                      0xBB, 0xBB, 0xCC, 0xCC, 0xCC, 0xCC};

    Reader reader("mock/test_1.bin");

    for (auto byte : expected_data) {
        ASSERT_TRUE(reader.HasNextByte());
        auto read_byte = reader.ReadNextByte();
        ASSERT_EQ(read_byte, byte);
    }

    ASSERT_FALSE(reader.HasNextByte());
}

TEST(Reader, ReadBinaryFile2) {
    const std::vector<unsigned char> expected_data = {0xFF, 0xAF, 0xFA, 0xF1, 0xF2, 0xF4, 0xF5,
                                                      0xF6, 0xBC, 0xDD, 0x30, 0x00, 0x40, 0xFF};

    Reader reader("mock/test_2.bin");

    for (auto byte : expected_data) {
        ASSERT_TRUE(reader.HasNextByte());
        auto read_byte = reader.ReadNextByte();
        ASSERT_EQ(read_byte, byte);
    }

    ASSERT_FALSE(reader.HasNextByte());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
