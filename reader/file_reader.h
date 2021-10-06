#pragma once
#include "reader_interface.h"

#include <fstream>
#include <optional>

class FileReader : public ReaderInterface {
public:
    explicit FileReader(const std::string& file_path);
    FileReader(const FileReader& o) = delete;
    FileReader& operator=(const FileReader& o) = delete;
    FileReader(FileReader&& o) = default;
    FileReader& operator=(FileReader&& o) = default;

    bool HasNextByte() const override;
    bool HasNextBit() const override;
    const std::string& GetFileName() const override;

    unsigned char ReadNextByte() override;
    bool ReadNextBit() override;
    void Reset() override;

private:
    std::ifstream file_;
    std::string filename_;
    size_t file_size_ = 0;
    size_t bytes_read_ = 0;
    size_t bit_pos_ = 0;
    unsigned char buffer_byte_ = 0;
};
