#include "archiver/archiver.h"
#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <filesystem>

#include "reader/file_reader.h"
#include "writer/file_writer.h"

bool AreFilesEqual(const std::string& file_path1, const std::string& file_path2) {
    FileReader reader1(file_path1);
    FileReader reader2(file_path2);

    while (reader1.HasNextByte()) {
        if (reader1.ReadNextByte() != reader2.ReadNextByte()) {
            return false;
        }
    }

    if (reader1.HasNextByte() != reader2.HasNextByte()) {
        return false;
    }

    return true;
}

std::string RemoveFileExtension(const std::string& file_name) {
    size_t dot_pos = file_name.find_first_of('.');

    if (dot_pos == std::string::npos) {
        return file_name;
    }

    return file_name.substr(0, dot_pos);
}

void TestFileCompression(const std::string& file_name) {
    Archiver archiver;
    auto archive_name = file_name + ".arc";
    std::string dir = "/home/gareton/repos/HuffArchiver/archiver/tests/mock/";

    std::vector<std::unique_ptr<ReaderInterface>> readers;
    readers.emplace_back(std::make_unique<FileReader>(dir + file_name));

    archiver.Compress(std::move(readers), std::make_unique<FileWriter>(dir), archive_name);
    archiver.Decompress(std::make_unique<FileReader>(dir + archive_name),
                        std::make_unique<FileWriter>(dir + "decompressed/"));

    ASSERT_TRUE(AreFilesEqual(dir + file_name, dir + "decompressed/" + file_name));
}

void TestFilesCompression(const std::vector<std::string>& file_names, const std::string& archive_name = "archive.arc") {
    Archiver archiver;
    std::vector<std::unique_ptr<ReaderInterface>> readers;

    for (const auto& file_name : file_names) {
        readers.emplace_back(std::make_unique<FileReader>("mock/" + file_name));
    }

    archiver.Compress(std::move(readers), std::make_unique<FileWriter>("mock/"), archive_name);

    archiver.Decompress(std::make_unique<FileReader>("mock/" + archive_name),
                        std::make_unique<FileWriter>("mock/decompressed/"));

    for (const auto& file_name : file_names) {
        ASSERT_TRUE(AreFilesEqual("mock/" + file_name, "mock/decompressed/" + file_name));
    }
}

// TEST(Archiver, TheSimplestTest) {
//     TestFileCompression("T");
// }

// TEST(Archiver, ArchiverTest1) {
//     TestFileCompression("test_1.bin");
// }

// TEST(Archiver, ArchiverTest2) {
//     TestFileCompression("Zadachnik-Kostrikin.pdf");
// }

// TEST(Archiver, MultipleFilesCompressionTest) {
//     TestFilesCompression({"T", "test_1.bin", "Zadachnik-Kostrikin.pdf"});
// }

// TEST(Archiver, TextFilesCompressionTest) {
//     TestFilesCompression({"T", "test_1.bin", "Zadachnik-Kostrikin.pdf"});
// }

TEST(Archiver, StrangeFilenameTest) {
    // auto path = std::filesystem::u8path("/home/gareton/repos/HuffArchiver/archiver/tests/mock/  Очень при очень плохое  имя файла  ありがとう 😂");
    // auto path = std::filesystem::u8path("mock/Очень при очень плохое  имя файла");
    // std::cout << (std::filesystem::exists(path) ? "exists" : "not exists") << std::endl;
    // std::ifstream in(path);
    // std::cout << (in.good() ? "yey" : "oh no") << std::endl;

    TestFileCompression("  Очень при очень плохое  имя файла  ありがとう 😂");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
