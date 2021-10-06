#pragma once
#include "writer_interface.h"

#include <string>
#include <fstream>

class FileWriter : public WriterInterface {
public:
    explicit FileWriter(std::string directory);
    FileWriter(const FileWriter& o) = delete;
    FileWriter& operator=(const FileWriter& o) = delete;
    FileWriter(FileWriter&& o) = default;
    FileWriter& operator=(FileWriter&& o) = default;

    void OpenFile(const std::string& filename) override;
    void CloseFile() override;
    void WriteByte(unsigned char byte) override;
    void WriteBit(bool bit) override;
    void Flush() override;

private:
    std::string directory_;
    std::ofstream file_;
    unsigned char buffer_byte_ = 0;
    char bit_pos_ = 0;
};