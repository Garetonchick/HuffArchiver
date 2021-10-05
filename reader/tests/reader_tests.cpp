#include "../reader.h"
#include <gtest/gtest.h>

#include <vector>

void TestByteReading(const std::string& file_path, const std::vector<unsigned char>& expected_data) {
    Reader reader(file_path);

    for (size_t i = 0; i < 4; ++i) {
        for (auto byte : expected_data) {
            ASSERT_TRUE(reader.HasNextByte());
            auto read_byte = reader.ReadNextByte();
            ASSERT_EQ(read_byte, byte);
        }

        ASSERT_FALSE(reader.HasNextByte());
        reader.Reset();
    }
}

void TestBitReading(const std::string& file_path, const std::vector<unsigned char>& expected_data) {
    Reader reader(file_path);

    for (size_t i = 0; i < 4; ++i) {
        for (auto byte : expected_data) {
            for (size_t j = 0; j < 8; ++j) {
                ASSERT_TRUE(reader.HasNextBit());
                bool read_bit = reader.ReadNextBit();
                ASSERT_EQ(read_bit, (1 & (byte >> (7 - j))));
            }
        }

        ASSERT_FALSE(reader.HasNextBit());
        reader.Reset();
    }
}

TEST(Reader, ReadBinaryFile1) {
    const std::vector<unsigned char> expected_data = {0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB,
                                                      0xBB, 0xBB, 0xCC, 0xCC, 0xCC, 0xCC};

    TestByteReading("mock/test_1.bin", expected_data);
    TestBitReading("mock/test_1.bin", expected_data);
}

TEST(Reader, ReadBinaryFile2) {
    const std::vector<unsigned char> expected_data = {0xFF, 0xAF, 0xFA, 0xF1, 0xF2, 0xF4, 0xF5,
                                                      0xF6, 0xBC, 0xDD, 0x30, 0x00, 0x40, 0xFF};

    TestByteReading("mock/test_2.bin", expected_data);
    TestBitReading("mock/test_2.bin", expected_data);
}

TEST(Reader, NameGettingTest) {
    Reader reader("mock/test_1.bin");

    ASSERT_EQ(reader.GetFileName(), "test_1.bin");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
