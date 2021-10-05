#include "../writer.h"
#include <gtest/gtest.h>

#include <vector>

#include "../../reader/reader.h"

void ByteWritingTest(const std::vector<unsigned char>& data) {
    const std::string test_dir = "mock/";
    const std::string test_name = "test.bin";

    Writer writer(test_dir);

    writer.OpenFile(test_name);

    for (auto byte : data) {
        writer.WriteByte(byte);
    }

    writer.CloseFile();

    Reader reader(test_dir + test_name);

    for (auto byte : data) {
        ASSERT_EQ(reader.ReadNextByte(), byte);
    }
}

void BitWritingTest(const std::vector<unsigned char>& data) {
    const std::string test_dir = "mock/";
    const std::string test_name = "test.bin";

    Writer writer(test_dir);

    writer.OpenFile(test_name);

    for (auto byte : data) {
        for (size_t i = 0; i < 8; ++i) {
            writer.WriteBit((byte >> (7 - i)) & 1);
        }
    }

    writer.CloseFile();

    Reader reader(test_dir + test_name);

    for (auto byte : data) {
        ASSERT_EQ(reader.ReadNextByte(), byte);
    }
}

TEST(Reader, WriteBinaryFile1) {
    const std::vector<unsigned char> test_data = {0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB,
                                                  0xBB, 0xBB, 0xCC, 0xCC, 0xCC, 0xCC};
    ByteWritingTest(test_data);
    BitWritingTest(test_data);
}

TEST(Reader, WriteBinaryFile2) {
    const std::vector<unsigned char> test_data = {0xFF, 0xAF, 0xFA, 0xF1, 0xF2, 0xF4, 0xF5,
                                                  0xF6, 0xBC, 0xDD, 0x30, 0x00, 0x40, 0xFF};
    ByteWritingTest(test_data);
    BitWritingTest(test_data);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
